#ifndef _LED_BEHAVIOR_H
#define _LED_BEHAVIOR_H

#include <stdint.h>
#include <arduino.h>
#include "SpringyValue.h" // Assuming SpringyValue is a standalone class

class LEDRingController;

class LEDBehavior
{
public:
    uint8_t brightness;
    LEDRingController *pController;

    LEDBehavior() : brightness(0) {}

    virtual void update(float deltaTime) = 0;
    virtual ~LEDBehavior() {}
};

class SpringyBehavior : public LEDBehavior
{
public:
    SpringyValue spring;
    SpringyBehavior(float initialPosition, float dampingFactor, float springStiffness, float objectMass)
        : spring(initialPosition, dampingFactor, springStiffness, objectMass) {}

    void update(float deltaTime) override;
};

class HeartbeatBehavior : public LEDBehavior
{
private:
    float timeElapsed;
    float heartbeatInterval;

public:
    HeartbeatBehavior(float interval)
        : timeElapsed(0), heartbeatInterval(interval) {}

    void update(float deltaTime) override;
};

class ColorCycleBehavior : public LEDBehavior
{
private:
    uint8_t hue;

public:
    ColorCycleBehavior() {}

    void update(float deltaTime) override;
};

class GaugeBehavior : public LEDBehavior
{
private:
    uint8_t value;

public:
    GaugeBehavior(uint8_t initialValue)
        : value(initialValue) {}

    void update(float deltaTime) override;

    void setValue(uint8_t newValue)
    {
        value = newValue;
        value = constrain(value, 0, 127);
    }

    uint32_t getColor(uint8_t iPixel);
};

#endif