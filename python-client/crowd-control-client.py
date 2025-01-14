import asyncio
import mido
from bleak import BleakClient, BleakScanner
import json

SERVICE_UUID = "3796c365-5633-4744-bc65-cac7812ef6da"
BUTTON_CHARACTERISTIC_UUID = "640033f1-08e8-429c-bd45-49ed4a60114e"
ROTARY_CHARACTERISTIC_UUID = "2a9ceeec-2d26-4520-bffe-8b13f00d4044"
WS2813_CHARACTERISTIC_UUID = "dcfd575f-b5d4-42c2-bf57-c5141fe2eaa9"

dial_scale_factor = 4
controllers = []
colors = [
    (255, 0, 0),  # Red
    (0, 255, 0),  # Green
    (0, 0, 255),  # Blue
    (255, 255, 0),  # Yellow
    (255, 0, 255),  # Magenta
    (0, 255, 255),  # Cyan
    (255, 255, 255),  # White
]

class CrowdController:
    def __init__(self, address, midi_port, control):
        self.address = address
        self.midi_port = midi_port
        self.client = BleakClient(address)
        self.control = control
        self.last_rotary_position = 0
        self.rotary_position = 0
        self.midi_cc = 0

    async def connect(self):
        print(f"Connecting to {self.address}...")
        try:
            await self.client.connect()
            if self.client.is_connected:
                print(f"Connected to {self.address}")
                color = colors[self.control % len(colors)]
                await self.send_led_command(led_index=0, r=color[0], g=color[1], b=color[2])
            else:
                print(f"Failed to connect to {self.address}")
        except Exception as e:
            print(f"Error connecting to {self.address}: {e}")

    async def disconnect(self):
        print(f"Disconnecting from {self.address}...")
        try:
            await self.client.disconnect()
            print(f"Disconnected from {self.address}")
        except Exception as e:
            print(f"Error disconnecting from {self.address}: {e}")

    async def reconnect(self):
        while not self.client.is_connected:
            print(f"Attempting to reconnect to {self.address}...")
            try:
                await self.client.connect()
                if self.client.is_connected:
                    print(f"Reconnected to {self.address}")
                    color = colors[self.control % len(colors)]
                    await self.send_led_command(led_index=0, r=color[0], g=color[1], b=color[2])
                    
            except Exception as e:
                print(f"Reconnection failed for {self.address}: {e}")
            await asyncio.sleep(5)

    async def poll_characteristics(self):
        try:
            while True:
                if self.client.is_connected:
                    try:
                        # Read button characteristic
                        button_data = await self.client.read_gatt_char(BUTTON_CHARACTERISTIC_UUID)
                        button_state = int(button_data.decode())

                        # Read rotary characteristic
                        rotary_data = await self.client.read_gatt_char(ROTARY_CHARACTERISTIC_UUID)
                        self.last_rotary_position = self.rotary_position
                        self.rotary_position = int(rotary_data.decode()) 
                        self.midi_cc += (self.rotary_position - self.last_rotary_position) * dial_scale_factor

                        # Constrain rotary position to 0-127
                        self.midi_cc = min(127, max(0, self.midi_cc))

                        # Send MIDI messages
                        cc_message = mido.Message('control_change', channel=0, control=self.control+8, value=button_state * 127)
                        self.midi_port.send(cc_message)

                        cc_message = mido.Message('control_change', channel=0, control=self.control, value=self.midi_cc)
                        self.midi_port.send(cc_message)

                    except Exception as e:
                        print(f"Error reading characteristics from {self.address}: {e}")
                else:
                    print(f"Client {self.address} disconnected. Attempting to reconnect...")
                    await self.reconnect()

                await asyncio.sleep(0.1)
        except asyncio.CancelledError:
            print(f"Polling stopped for {self.address}")

    async def send_led_command(self, led_index, r, g, b):
        """
        Send an LED command to the ESP32 via the WS2813_CHARACTERISTIC_UUID.

        Args:
            led_index (int): The index of the LED to control.
            r (int): Red value (0-255).
            g (int): Green value (0-255).
            b (int): Blue value (0-255).
        """
        if self.client.is_connected:
            command = json.dumps({
                "index": led_index,
                "r": r,
                "g": g,
                "b": b
            }).encode('utf-8')
            try:
                await self.client.write_gatt_char(WS2813_CHARACTERISTIC_UUID, command)
                print(f"Sent LED command: {command}")
            except Exception as e:
                print(f"Error sending LED command: {e}")
        else:
            print(f"Cannot send LED command. Device {self.address} not connected.")


async def discover_devices(service_uuid):
    matched_devices = []

    def detection_callback(device, advertisement_data):
        uuids = advertisement_data.service_uuids or []
        if service_uuid.lower() in [uuid.lower() for uuid in uuids]:
            if device not in matched_devices:
                matched_devices.append(device)
                print(f"Found device: {device.name} ({device.address})")

    scanner = BleakScanner()
    scanner.register_detection_callback(detection_callback)

    print("Scanning for devices...")
    await scanner.start()
    await asyncio.sleep(5)  # Adjust the scan duration as needed
    await scanner.stop()

    return matched_devices

async def monitor_new_devices(service_uuid, midi_port):
    global controllers

    while True:
        new_devices = await discover_devices(service_uuid)
        for device in new_devices:
            if not any(controller.address == device.address for controller in controllers):
                print(f"Adding new device: {device.address}")
                new_controller = CrowdController(device.address, midi_port, control=len(controllers))
                controllers.append(new_controller)
                await new_controller.connect()                
                asyncio.create_task(new_controller.poll_characteristics())

        await asyncio.sleep(10)  # Adjust the interval for scanning

async def main():
    global controllers
    midi_port = mido.open_output("CrowdController")
    matched_devices = await discover_devices(SERVICE_UUID)

    if not matched_devices:
        print("No devices found with the specified service UUID.")
        return

    controllers = [CrowdController(device.address, midi_port, control=i) for i, device in enumerate(matched_devices[:8])] # Limit to 8 devices

    try:
        # Connect to all controllers and start polling
        for controller in controllers:
            await controller.connect()                        

        poll_tasks = [asyncio.create_task(controller.poll_characteristics()) for controller in controllers]
        monitor_task = asyncio.create_task(monitor_new_devices(SERVICE_UUID, midi_port))

        await asyncio.gather(*poll_tasks, monitor_task)
    except KeyboardInterrupt:
        print("Program interrupted by user. Disconnecting...")
    finally:
        # Disconnect all controllers
        for controller in controllers:
            await controller.disconnect()

if __name__ == "__main__":
    asyncio.run(main())
