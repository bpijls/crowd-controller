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
LEDRingController dialRing(strip, 0, 15);    // LEDs 0-15
LEDRingController buttonRing(strip, 16, 23); // LEDs 16-23

PushButton dialButton(DIAL_BUTTON_PIN);
PushButton pushButton(PUSH_BUTTON_PIN);

int BLEUpdateIntervalMs = 10;
int lastBLEUpdateTime = 0;

BLEServer *pServer;
BLEService *pService;

SpringyBehavior springy(0.0f, 1.5f, 2.0f, 2.0f);
HeartbeatBehavior heartbeat(1.0f);  // 1-second interval

void startAdvertising()
{
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    dialRing.setBehavior(&heartbeat);
    dialRing.setColor(Adafruit_NeoPixel::Color(0, 0, 255));
    pAdvertising->start();
}

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        Serial.println("Client connected!");
        dialRing.setColor(Adafruit_NeoPixel::Color(0, 255, 0));
    }

    void onConnectionFailed(BLEServer* pServer) {
        Serial.println("Connection failed!");
        startAdvertising();
    }

    void onDisconnect(BLEServer* pServer) {
        Serial.println("Client disconnected!");
        startAdvertising();
    }
};

void updateEncoder()
{
    encoder.tick();    

    if (encoder.getDirection() != RotaryEncoder::Direction::NOROTATION)
    {
        int pos = encoder.getPosition();
        pRotaryCharacteristic->setValue(String(pos).c_str());
        Serial.println(String("Rotary value: ") + pos);
    }
}

void initBLE()
{
    // Initialize BLE
    Serial.println("Starting BLE work!");
    BLEDevice::init("AI EDM Controller");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    pService = pServer->createService(SERVICE_UUID);

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
    pAdvertising->setMinPreferred(0x06); // Helps with iPhone connection issues
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    dialRing.setBehavior(&heartbeat);
    dialRing.setColor(0x0000FF);
    Serial.println("BLE device started, waiting for connections...");
}

void resetBLEConnection()
{
    Serial.println("Resetting BLE connection...");
    if (pServer) {
        pServer->disconnect(pServer->getConnId());
        BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
        pAdvertising->start();
        Serial.println("Advertising restarted.");
    }
}

void sendBLEData()
{
    pButtonCharacteristic->setValue(String(pushButton.isPressed()).c_str());
    pRotaryCharacteristic->setValue(String(encoder.getPosition()).c_str());
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting Crowd Controller...");
    pushButton.begin();
    dialButton.begin();
    initBLE();
}

void loop()
{
    TimeKeeper::update();
    updateEncoder();
    dialButton.update();
    pushButton.update();

    dialRing.update(TimeKeeper::getDeltaTime());
    buttonRing.update(TimeKeeper::getDeltaTime());

    if (millis() - lastBLEUpdateTime > BLEUpdateIntervalMs)
    {
        sendBLEData();
        lastBLEUpdateTime = millis();
    }

    if (dialButton.wasPressed())
    {
        Serial.println("Dial button pressed!");
        resetBLEConnection();        
    }

    if (dialButton.wasReleased())
    {
        Serial.println("Dial button released!");
    }
}
