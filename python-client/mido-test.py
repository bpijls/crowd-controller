import mido
import time
import math

# Open the virtual MIDI port
midiPort = mido.open_output("CrowdController")

# send sin wave control change messages in different frequencies for 8 controllers (1-8)
# to test the CrowdController
while True:
    for i in range(1,9):
        cValue = (math.sin(time.time() * i) + 1)*63 # Sine wave between 0 and 127
        print(f"Control Change {i}: {int(cValue)}") # Print the value
        cc_message = mido.Message('control_change', channel=1, control=i, value=(int(cValue))) # Create the message
        midiPort.send(cc_message) # Send the message
    time.sleep(0.1) # Sleep for 0.1 seconds