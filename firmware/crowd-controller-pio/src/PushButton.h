#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <Arduino.h>

class PushButton {
private:
    uint8_t pin;             // Pin where the button is connected
    bool currentState;       // Current state of the button
    bool lastState;          // Previous state of the button
    bool lastReading;        // Last reading from the button
    unsigned long lastDebounceTime; // Time when the button state last changed
    unsigned long debounceDelay;   // Debounce delay in milliseconds

public:
    // Constructors
    PushButton(uint8_t buttonPin, unsigned long debounce = 50)
        : pin(buttonPin), debounceDelay(debounce), currentState(false), lastState(false), lastDebounceTime(0) {
    }

    // Initialize the button with internal pull-up resistor
    void begin() {
        pinMode(pin, INPUT_PULLUP);
        currentState = digitalRead(pin);
        lastState = currentState;
    }

    // Update the button state (call this in the main loop)
    void update() {
        bool reading = digitalRead(pin);


        if (reading != lastReading) {
            lastDebounceTime = millis(); // Reset the debounce timer   
        }

        // Check if the button state has changed            
        if ((millis() - lastDebounceTime) > debounceDelay) {
            lastState = currentState;
            if (reading != currentState) {             
                currentState = reading;                
            }
        }

        lastReading = reading;
    }

    // Check if the button is currently pressed
    bool isPressed() const {
        return !currentState; // Adjusted for pull-up logic
    }

    // Check if the button state has changed
    bool stateChanged() const {
        return currentState != lastState;
    }

    // Check if the button was just pressed
    bool wasPressed() const {
        return (!currentState && lastState); // Adjusted for pull-up logic
    }

    // Check if the button was just released
    bool wasReleased() const {
        return (currentState && !lastState); // Adjusted for pull-up logic
    }
};

#endif
