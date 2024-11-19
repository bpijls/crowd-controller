import asyncio
from bleak import BleakClient, BleakScanner

SERVICE_UUID = "3796c365-5633-4744-bc65-cac7812ef6da"
BUTTON_CHARACTERISTIC_UUID = "640033f1-08e8-429c-bd45-49ed4a60114e"
ROTARY_CHARACTERISTIC_UUID = "2a9ceeec-2d26-4520-bffe-8b13f00d4044"

async def handle_device(device):
    address = device.address
    async with BleakClient(address) as client:
        if client.is_connected:
            print(f"Connected to {address}")
            try:
                while True:
                    # Read button characteristic
                    button_data = await client.read_gatt_char(BUTTON_CHARACTERISTIC_UUID)
                    button_state = int(button_data.decode())

                    # Read rotary characteristic
                    rotary_data = await client.read_gatt_char(ROTARY_CHARACTERISTIC_UUID)
                    rotary_position = int(rotary_data.decode())

                    # Process the data and send to virtual MIDI port
                    print(f"Device {address}: Button={button_state}, Rotary={rotary_position}")

                    # TODO: Implement sending data to virtual MIDI port here

                    await asyncio.sleep(0.1)  # Adjust the polling interval as needed
            except asyncio.CancelledError:
                print(f"Disconnected from {address}")
        else:
            print(f"Failed to connect to {address}")

async def main():
    # Create a list to store matched devices
    matched_devices = []

    def detection_callback(device, advertisement_data):
        uuids = advertisement_data.service_uuids or []
        if SERVICE_UUID.lower() in [uuid.lower() for uuid in uuids]:
            if device not in matched_devices:
                matched_devices.append(device)
                print(f"Found device: {device.name} ({device.address})")

    # Create a scanner and register the detection callback
    scanner = BleakScanner()
    scanner.register_detection_callback(detection_callback)

    # Start scanning
    print("Scanning for devices...")
    await scanner.start()
    await asyncio.sleep(5)  # Adjust the scan duration as needed
    await scanner.stop()

    if not matched_devices:
        print("No devices found with the specified service UUID.")
        return

    # Connect to each device concurrently
    print("Connecting to devices...")
    tasks = [asyncio.create_task(handle_device(device)) for device in matched_devices]

    try:
        await asyncio.gather(*tasks)
    except KeyboardInterrupt:
        print("Program interrupted by user. Disconnecting...")
        for task in tasks:
            task.cancel()
        await asyncio.gather(*tasks, return_exceptions=True)

if __name__ == "__main__":
    asyncio.run(main())
