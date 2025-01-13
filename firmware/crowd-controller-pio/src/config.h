#ifndef _CONFIG_H
#define _CONFIG_H

#define ROTARY_PIN_IN1 1
#define ROTARY_PIN_IN2 0
#define PUSH_BUTTON_PIN 7
#define DIAL_BUTTON_PIN 10
#define WS2812_PIN 3

#define NUM_LEDS 24   // Popular NeoPixel ring size

// UUIDs for the service and characteristics
#define SERVICE_UUID "3796c365-5633-4744-bc65-cac7812ef6da"
#define BUTTON_CHARACTERISTIC_UUID "640033f1-08e8-429c-bd45-49ed4a60114e"
#define ROTARY_CHARACTERISTIC_UUID "2a9ceeec-2d26-4520-bffe-8b13f00d4044"
#define WS2813_CHARACTERISTIC_UUID "dcfd575f-b5d4-42c2-bf57-c5141fe2eaa9"

#endif