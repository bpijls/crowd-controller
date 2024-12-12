import asyncio
import mido
from bleak import BleakClient, BleakScanner

SERVICE_UUID = "3796c365-5633-4744-bc65-cac7812ef6da"
BUTTON_CHARACTERISTIC_UUID = "640033f1-08e8-429c-bd45-49ed4a60114e"
ROTARY_CHARACTERISTIC_UUID = "2a9ceeec-2d26-4520-bffe-8b13f00d4044"
WS2813_CHARACTERISTIC_UUID = "dcfd575f-b5d4-42c2-bf57-c5141fe2eaa9"

class CrowdController:
    def __init__(self, address, midi_port, control):
        self.address = address
        self.midi_port = midi_port
        self.client = BleakClient(address)
        self.control = control
        self.button_state = 0  # 0 = OFF, 127 = ON
        self.previous_button_read = 0  # Tracks the previous button reading


    async def connect(self):
        print(f"Connecting to {self.address}...")
        await self.client.connect()
        if self.client.is_connected:
            print(f"Connected to {self.address}")
        else:
            print(f"Failed to connect to {self.address}")

    async def disconnect(self):
        print(f"Disconnecting from {self.address}...")
        await self.client.disconnect()
        print(f"Disconnected from {self.address}")

    async def poll_characteristics(self):
        try:
            while self.client.is_connected:
                # Read button characteristic
                button_data = await self.client.read_gatt_char(BUTTON_CHARACTERISTIC_UUID)
                current_button_state = int(button_data.decode())

                # Toggle button state only on a single press
                if current_button_state == 1 and self.previous_button_state == 0:
                    self.button_state = 127 if self.button_state == 0 else 0

                self.previous_button_state = current_button_state  # Update the previous reading

                # Read rotary characteristic
                rotary_data = await self.client.read_gatt_char(ROTARY_CHARACTERISTIC_UUID)
                rotary_position = int(rotary_data.decode())

                # Process the data and send to virtual MIDI port
                print(f"Device {self.address}: Button={self.button_state}, Rotary={rotary_position}")
             
                cc_message_1 = mido.Message('control_change', channel=0, control=self.control+1, value=self.button_state)
                self.midi_port.send(cc_message_1) 
                print(cc_message_1)
                
                cc_message_2 = mido.Message('control_change', channel=0, control=self.control+2, value=rotary_position % 127)
                self.midi_port.send(cc_message_2)
                print(cc_message_2)

                await asyncio.sleep(0.1)  # Adjust the polling interval as needed
        except asyncio.CancelledError:
            print(f"Polling stopped for {self.address}")

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

async def main():
    midi_port = mido.open_output("CrowdController")
    matched_devices = await discover_devices(SERVICE_UUID)

    if not matched_devices:
        print("No devices found with the specified service UUID.")
        return

    controllers = [CrowdController(device.address, midi_port, control=i+10) for i, device in enumerate(matched_devices[:8])] # Limit to 8 devices
    # i * 10  for every new controller, gives every instance a range of 10 (1-9, 11-19, 21-29 etc.)

    try:
        # Connect to all controllers and start polling
        for controller in controllers:
            await controller.connect()

        poll_tasks = [asyncio.create_task(controller.poll_characteristics()) for controller in controllers]
        await asyncio.gather(*poll_tasks)
    except KeyboardInterrupt:
        print("Program interrupted by user. Disconnecting...")
    finally:
        # Disconnect all controllers
        for controller in controllers:
            await controller.disconnect()

if __name__ == "__main__":
    asyncio.run(main())
