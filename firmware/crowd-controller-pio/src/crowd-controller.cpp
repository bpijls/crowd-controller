#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <RotaryEncoder.h>
#include <ArduinoJson.h>

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
int dialPosition = 0;
int oldDialPosition = 0;
int ccDialPosition = 0;

BLEServer *pServer;
BLEService *pService;

SpringyBehavior springy(20.0f, 180.0f, 15.0f, 60.0f);
HeartbeatBehavior heartbeat(1.0f);  // 1-second interval
ColorCycleBehavior colorCycle;
GaugeBehavior gauge(0);

void startAdvertising()
{
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    dialRing.setBehavior(&heartbeat);
    dialRing.setColor(Adafruit_NeoPixel::Color(0, 0, 255));
    pAdvertising->start();
}


void handleSerialInput()
{
    if (Serial.available())
    {
        char c = Serial.read();
        switch (c)
        {
            case 's':
                springy.spring.stiffness += 0.1f;
                break;
            case 'S':
                springy.spring.stiffness -= 0.1f;
                break;
            case 'd':
                springy.spring.damping += 0.1f;
                break;
            case 'D':
                springy.spring.damping -= 0.1f;
                break;
            case 'm':
                springy.spring.mass += 0.1f;
                break;
            case 'M':
                springy.spring.mass -= 0.1f;
                break;
        }

        Serial.print("Stiffness: ");
        Serial.print(springy.spring.stiffness);
        Serial.print(" Damping: ");
        Serial.print(springy.spring.damping);
        Serial.print(" Mass: ");
        Serial.println(springy.spring.mass);
    }
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

class WS2812Callbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        if (!value.empty()) {
            StaticJsonDocument<128> doc;
            DeserializationError error = deserializeJson(doc, value);

            if (!error) {
                int index = doc["index"].as<int>();
                int r = doc["r"].as<int>();
                int g = doc["g"].as<int>();
                int b = doc["b"].as<int>();

                if (index >= 0 && index < NUM_LEDS) {
                    buttonRing.setColor(Adafruit_NeoPixel::Color(r, g, b));
                    Serial.printf("LED %d set to R:%d G:%d B:%d\n", index, r, g, b);
                } else {
                    Serial.printf("Invalid LED index: %d\n", index);
                }
            } else {
                Serial.println("Failed to parse LED command");
            }
        }
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
        WS2812_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE);

    pWS2812Characteristic->setCallbacks(new WS2812Callbacks());

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

    if (encoder.getDirection() != RotaryEncoder::Direction::NOROTATION)
    {
        dialPosition = encoder.getPosition();
        ccDialPosition += (dialPosition - oldDialPosition) * DIAL_SCALE_FACTOR;
        oldDialPosition = dialPosition;
        ccDialPosition = constrain(ccDialPosition, 0, 127);
        dialRing.setBehavior(&gauge);
        gauge.setValue(ccDialPosition);
        Serial.println(ccDialPosition);
    }
    

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
        resetBLEConnection();        
    }

    handleSerialInput();
}
