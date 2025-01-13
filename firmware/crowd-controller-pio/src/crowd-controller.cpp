#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <RotaryEncoder.h>
#include "config.h"

#include "Adafruit_NeoPixel.h"
#include "LEDRingController.h"
#include "PushButton.h"
#include "TimeKeeper.h"

// Define the static strip instance
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, WS2812_PIN, NEO_GRB + NEO_KHZ800);

// GPIO pins for the button and rotary encoder
BLECharacteristic *pButtonCharacteristic;
BLECharacteristic *pRotaryCharacteristic;
BLECharacteristic *pWS2812Characteristic;

RotaryEncoder encoder(ROTARY_PIN_IN1, ROTARY_PIN_IN2, RotaryEncoder::LatchMode::TWO03);

// Create instances for specific LED ranges
LEDRingController dialRing(strip, 0, 15);     // LEDs 0-15
LEDRingController buttonRing(strip, 16, 23);  // LEDs 16-23

PushButton dialButton(DIAL_BUTTON_PIN);
PushButton button(PUSH_BUTTON_PIN);

void updateEncoder() {
  // put your main code here, to run repeatedly:
  static int pos = 0;
  encoder.tick();

  int newPos = encoder.getPosition();
  
  if (pos != newPos) {
    pos = newPos;    
    String rotaryValue = String(pos);
    pRotaryCharacteristic->setValue(rotaryValue.c_str());
    Serial.println(String("Rotary value: ") + pos);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  // Initialize GPIO pins
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize BLE
  BLEDevice::init("AI EDM Controller");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create characteristics without PROPERTY_NOTIFY
  pButtonCharacteristic = pService->createCharacteristic(
    BUTTON_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ);

  pRotaryCharacteristic = pService->createCharacteristic(
    ROTARY_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ);

  pWS2812Characteristic = pService->createCharacteristic(
    WS2813_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE);

  // Set initial values
  pButtonCharacteristic->setValue("0");
  pRotaryCharacteristic->setValue("0");

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // Helps with iPhone connection issues
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("BLE device started, waiting for connections...");
}

void loop() {
  static int lastButtonState = HIGH;

  int buttonState = digitalRead(BUTTON_PIN);

  updateEncoder();

  // Check for button press
  if (buttonState != lastButtonState) {
    lastButtonState = buttonState;

    // Update the button characteristic
    pButtonCharacteristic->setValue(buttonState == LOW ? "1" : "0");
    Serial.print("Button state changed: ");
    Serial.println(buttonState == LOW ? "Pressed" : "Released");
  }

  delay(10);  // Adjust delay as needed
}


