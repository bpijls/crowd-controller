#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// UUIDs for the service and characteristics
#define SERVICE_UUID                  "3796c365-5633-4744-bc65-cac7812ef6da"
#define BUTTON_CHARACTERISTIC_UUID    "640033f1-08e8-429c-bd45-49ed4a60114e"
#define ROTARY_CHARACTERISTIC_UUID    "2a9ceeec-2d26-4520-bffe-8b13f00d4044"

// GPIO pins for the button and rotary encoder
#define BUTTON_PIN     7
#define ROTARY_PIN_A   9
#define ROTARY_PIN_B   8

BLECharacteristic *pButtonCharacteristic;
BLECharacteristic *pRotaryCharacteristic;

volatile int rotaryPosition = 0;
volatile int lastEncoded = 0;

void IRAM_ATTR updateRotary() {
  int MSB = digitalRead(ROTARY_PIN_A);
  int LSB = digitalRead(ROTARY_PIN_B);

  int encoded = (MSB << 1) | LSB;
  int sum = (lastEncoded << 2) | encoded;

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
    rotaryPosition++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
    rotaryPosition--;

  lastEncoded = encoded;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  // Initialize GPIO pins
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(ROTARY_PIN_A, INPUT_PULLUP);
  pinMode(ROTARY_PIN_B, INPUT_PULLUP);

  // Attach interrupts for rotary encoder
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_A), updateRotary, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_PIN_B), updateRotary, CHANGE);

  // Initialize BLE
  BLEDevice::init("AI EDM Controller");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create characteristics without PROPERTY_NOTIFY
  pButtonCharacteristic = pService->createCharacteristic(
    BUTTON_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ
  );

  pRotaryCharacteristic = pService->createCharacteristic(
    ROTARY_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ
  );

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
  static int lastRotaryPosition = 0;

  int buttonState = digitalRead(BUTTON_PIN);

  // Check for button press
  if (buttonState != lastButtonState) {
    lastButtonState = buttonState;

    // Update the button characteristic
    pButtonCharacteristic->setValue(buttonState == LOW ? "1" : "0");
    Serial.print("Button state changed: ");
    Serial.println(buttonState == LOW ? "Pressed" : "Released");
  }

  // Check for rotary encoder change
  if (rotaryPosition != lastRotaryPosition) {
    lastRotaryPosition = rotaryPosition;

    // Update the rotary characteristic
    String rotaryValue = String(rotaryPosition);
    pRotaryCharacteristic->setValue(rotaryValue.c_str());
    Serial.print("Rotary position changed: ");
    Serial.println(rotaryPosition);
  }

  delay(10);  // Adjust delay as needed
}
