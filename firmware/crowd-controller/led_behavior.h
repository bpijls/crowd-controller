#ifndef _LED_BEHAVIOR_H
#define _LED_BEHAVIOR_H

#include <stdint.h>
#include "springy_value.h"  // Assuming SpringyValue is a standalone class

class LEDRingController;


class LEDBehavior {
public:
  uint8_t brightness;

  LEDBehavior() : brightness(0) {}

  virtual void update(float deltaTime, LEDRingController& controller) = 0;
  virtual ~LEDBehavior() {}
};

class SpringyBehavior : public LEDBehavior {
public:
  SpringyValue spring;
  SpringyBehavior(float initialPosition, float dampingFactor, float springStiffness, float objectMass)
    : spring(initialPosition, dampingFactor, springStiffness, objectMass) {}

  void update(float deltaTime, LEDRingController& controller) override;
};

class HeartbeatBehavior : public LEDBehavior {
private:
  float timeElapsed;
  float heartbeatInterval;

public:
  HeartbeatBehavior(float interval)
    : timeElapsed(0), heartbeatInterval(interval) {}

  void update(float deltaTime, LEDRingController& controller) override;
};

#endif