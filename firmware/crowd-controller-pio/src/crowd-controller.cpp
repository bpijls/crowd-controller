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

SpringyBehavior springy(10.0f, 0.5f, 2.0f, 0.01f);
HeartbeatBehavior heartbeat(1.0f);  // 1-second interval
ColorCycleBehavior colorCycle;

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
    pAdvertising->setMinPreferred(0x12);
    startAdvertising();

    Serial.println("BLE device started, waiting for connections...");
}

void resetBLEConnection()
{
    Serial.println("Resetting BLE connection...");
    if (pServer) {
        pServer->disconnect(pServer->getConnId());
        startAdvertising();
        Serial.println("Advertising restarted.");
    }
}

void sendBLEData()
{    
    pButtonCharacteristic->setValue(String(pushButton.isPressed()).c_str());
    pRotaryCharacteristic->setValue(String(encoder.getPosition()/2).c_str());
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting Crowd Controller...");
    pushButton.begin();
    dialButton.begin();
    buttonRing.setBehavior(&springy);
    buttonRing.setColor(Adafruit_NeoPixel::Color(255, 0, 0));
    initBLE();
}

void loop()
{
    TimeKeeper::update();
    
    dialButton.update();
    pushButton.update();
    encoder.tick();

    dialRing.update(TimeKeeper::getDeltaTime());
    buttonRing.update(TimeKeeper::getDeltaTime());

    if (millis() - lastBLEUpdateTime > BLEUpdateIntervalMs)
    {
        sendBLEData();
        lastBLEUpdateTime = millis();
    }

    if (pushButton.wasPressed())
    {
        Serial.println("Push button pressed!");
        springy.spring.perturb(255);
    }

    if (dialButton.wasPressed())
    {
        Serial.println("Dial button pressed!");
        //resetBLEConnection();        
    }
}
