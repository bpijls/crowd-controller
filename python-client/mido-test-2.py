import mido
import time

# Open de virtuele MIDI-uitgang
midi_port = mido.open_output("CrowdController")

# Instellen van MIDI-parameters
cc_numbers = [21, 22, 23, 24, 25, 26, 27, 28, 29]  # Gebruik CC 21 t/m 27 voor tracks
channels = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]  # Gebruik kanaal 1 (0-indexed)
toggle_states = [0, 0, 0, 0, 0, 0, 0, 0, 0]  # Houd de aan/uit-status bij voor elke track

while True:
    for i, cc in enumerate(cc_numbers):
        # Wissel tussen aan/uit
        toggle_states[i] = 127 if toggle_states[i] == 0 else 0
        
        # Maak het MIDI Control Change-bericht
        cc_message = mido.Message(
            'control_change',
            channel=channels[i],  # Specificeer kanaal
            control=cc,  # CC-nummer
            value=toggle_states[i]  # Waarde (127 voor aan, 0 voor uit)
        )
        
        # Stuur het MIDI-bericht
        midi_port.send(cc_message)
        print(f"Track {i + 4} {'ON' if toggle_states[i] == 127 else 'OFF'} - Sent CC {cc} with value {toggle_states[i]}")

        # Wacht 1 seconde voor elke toggle (voor demonstratie)
        time.sleep(1)
