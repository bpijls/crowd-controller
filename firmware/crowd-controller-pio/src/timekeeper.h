#ifndef _TIMEKEEPER_H
#define _TIMEKEEPER_H

class TimeKeeper {
private:
    static unsigned long previousMillis; // Stores the last recorded time
    static float deltaTime;              // Stores the time difference between updates in seconds

public:
    // Updates the timekeeper and calculates delta time
    static void update() {
        unsigned long currentMillis = millis();
        deltaTime = (currentMillis - previousMillis) / 1000.0f; // Convert milliseconds to seconds
        previousMillis = currentMillis;
    }

    // Returns the delta time in seconds
    static float getDeltaTime() {
        return deltaTime;
    }
};

// Define static members
unsigned long TimeKeeper::previousMillis = 0;
float TimeKeeper::deltaTime = 0;

#endif