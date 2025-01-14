#include "Arduino.h"
#ifndef _LED_CONTROLLER_H
#define _LED_CONTROLLER_H
#include <Adafruit_NeoPixel.h>
#include <vector>

class LEDBehavior;  // Forward declaration of LEDBehavior

class LEDRingController {
private:
  Adafruit_NeoPixel& strip;
  uint8_t startIndex;                 // Starting LED index of the segment
  uint8_t endIndex;                   // Ending LED index of the segment
  std::vector<uint32_t> pixelValues;  // Stores the pixel values for the segment
  int16_t brightness;                 // Current brightness level
  LEDBehavior* behavior;              // Pointer to the assigned behavior  

public:

  uint8_t nTotalPixels = (endIndex - startIndex)+1;
  uint8_t nActivePixels = nTotalPixels;

  // Constructor
  LEDRingController(Adafruit_NeoPixel& ledStrip, uint8_t start, uint8_t end)
    : strip(ledStrip), startIndex(start), endIndex(end), brightness(255), behavior(nullptr) {
    pixelValues.resize(end - start + 1, 0);
  }

  // Set brightness for this specific segment
  void setBrightness(uint8_t newBrightness) {
    brightness = newBrightness;
    brightness = constrain(brightness, 0, 255);
  }

  // Set color for this specific segment
  void setColor(uint32_t color) {
    for (uint8_t i = 0; i < pixelValues.size(); ++i) {
      pixelValues[i] = color;
    }
  }

  // Update the LEDs in the segment with the stored values and brightness
  void update(float deltaTime) {

    // The behavior modifies the brihgtness
    if (behavior) {
      behavior->update(deltaTime, *this);
    }

    for (uint8_t i = 0; i < nActivePixels; ++i) {
      uint32_t color = pixelValues[i];
      uint8_t r = (uint8_t)((color >> 16) & 0xFF);
      uint8_t g = (uint8_t)((color >> 8) & 0xFF);
      uint8_t b = (uint8_t)(color & 0xFF);

      // Apply brightness scaling
      r = (r * brightness) / 255;
      g = (g * brightness) / 255;
      b = (b * brightness) / 255;

      strip.setPixelColor(startIndex + i, strip.Color(r, g, b));
    }

    strip.show();
  }

  // Clear the segment
  void clearSegment() {
    for (uint8_t i = 0; i < pixelValues.size(); ++i) {
      pixelValues[i] = 0;  // Turn off the LED
    }
  }

  // Assign a behavior to the LED segment
  void setBehavior(LEDBehavior* newBehavior) {
    behavior = newBehavior;
  }

  void setGauge(uint8_t value){
    nActivePixels = map(value, 0, 63, 0, nTotalPixels+1);
    nActivePixels = constrain(nActivePixels, 0, nTotalPixels);
  }

  // Getters for segment information
  uint8_t getStartIndex() const {
    return startIndex;
  }
  uint8_t getEndIndex() const {
    return endIndex;
  }
  uint8_t getBrightness() const {
    return brightness;
  }
  Adafruit_NeoPixel& getStrip() {
    return strip;
  }
  std::vector<uint32_t>& getPixelValues() {
    return pixelValues;
  }
};


#endif