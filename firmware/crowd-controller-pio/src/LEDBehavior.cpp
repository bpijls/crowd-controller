
#include "LEDBehavior.h"
#include "LEDRingController.h"

void SpringyBehavior::update(float deltaTime, LEDRingController& controller)  {
  spring.update(deltaTime);
  float position = abs(spring.position);
  uint8_t brightness = static_cast<uint8_t>(position);  // Map position to brightness
  controller.setBrightness(brightness);
}


void HeartbeatBehavior::update(float deltaTime, LEDRingController& controller)  {
  timeElapsed += deltaTime;
  float pulse = (sinf((timeElapsed / heartbeatInterval) * 3.14159f * 0.5f)-0.5)*2;  // Pulsate -1 - 1
  pulse = pulse > 0 ? pulse : 0; // only "beat" on the positive values
  brightness = static_cast<uint8_t>(pulse * 255);
  controller.setBrightness(brightness);
}