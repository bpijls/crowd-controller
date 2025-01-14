
#include "LEDBehavior.h"
#include "LEDRingController.h"

void SpringyBehavior::update(float deltaTime)
{
    if (pController == nullptr)
    {
        return;
    }

    spring.update(deltaTime);
    float position = abs(spring.position);
    brightness = static_cast<uint8_t>(position); // Map position to brightness
    pController->setBrightness(brightness);
}

void HeartbeatBehavior::update(float deltaTime)
{
    if (pController == nullptr)
    {
        return;
    }

    timeElapsed += deltaTime;
    float pulse = (sinf((timeElapsed / heartbeatInterval) * 3.14159f * 0.5f) - 0.5) * 2; // Pulsate -1 - 1
    pulse = pulse > 0 ? pulse : 0;                                                       // only "beat" on the positive values
    brightness = static_cast<uint8_t>(pulse * 255);
    pController->setBrightness(brightness);
}

void ColorCycleBehavior::update(float deltaTime)
{
    if (pController == nullptr)
    {
        return;
    }

    // Cycle through the color wheel
    if (millis() % 2000 == 0)
    {
        hue += 1;
    }

    uint32_t color = pController->getStrip().Color(hue++, 255 - hue, hue / 2);

    pController->setColor(color);
}

uint32_t GaugeBehavior::getColor(uint8_t iPixel)
{
    if (pController == nullptr)
    {
        return 0;
    }

    uint8_t nPixels = pController->nTotalPixels;
    uint8_t segmentSize = 127 / 3;

    uint8_t segment = value / segmentSize;
    uint8_t remainder = value % segmentSize;
    uint8_t nActivePixels = map(remainder, 0, segmentSize, 0, nPixels);

    uint8_t red =0;
    uint8_t green = 0;
    uint8_t blue = 0;

    red = (segment > 0 || (segment == 0 && iPixel < nActivePixels)) ? 255 : 0;
    blue = (segment > 1 || (segment == 1 && iPixel < nActivePixels)) ? 255 : 0;
    green = (segment > 2 || (segment == 2 && iPixel < nActivePixels)) ? 255 : 0;

    return pController->getStrip().Color(red, green, blue);
}

void GaugeBehavior::update(float deltaTime)
{
    if (pController == nullptr)
    {
        return;
    }

    for (uint8_t i = 0; i < pController->nTotalPixels; ++i)
    {
        uint32_t color = getColor(i);
        pController->setPixelColor(i, color);
    }

    pController->setBrightness(255);
}