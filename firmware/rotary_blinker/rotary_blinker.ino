
#include <RotaryEncoder.h>
#include <Adafruit_NeoPixel.h>
#include "push_button.h"
#include "led_behavior.h"
#include "led_ring_controller.h"
#include "config.h"
#include "time_keeper.h"

// Define the static strip instance
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, WS2812_PIN, NEO_GRB + NEO_KHZ800);

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder encoder(ROTARY_PIN_IN1, ROTARY_PIN_IN2, RotaryEncoder::LatchMode::TWO03);

// Create instances for specific LED ranges
LEDRingController dialRing(strip, 0, 15);     // LEDs 0-15
LEDRingController buttonRing(strip, 16, 23);  // LEDs 16-23

PushButton dialButton(DIAL_BUTTON_PIN);
PushButton button(BUTTON_PIN);

uint8_t encoderSwitchStatus = HIGH;
uint8_t switchStatus = HIGH;

uint8_t dialMode = 0;

// Assign a springy behavior
SpringyBehavior springy(0.0f, 1.5f, 2.0f, 2.0f);
HeartbeatBehavior heartbeat(1.0f);  // 1-second interval

void setup() {
  Serial.begin(9600);
  
  // strip.setBrightness(20);
  // Set colors for segments
  dialRing.setColor(Adafruit_NeoPixel::Color(255, 0, 0));    // Red
  buttonRing.setColor(Adafruit_NeoPixel::Color(0, 255, 0));  // Green

  buttonRing.setBehavior(&springy);
  dialRing.setBehavior(&heartbeat);
  dialButton.begin();
  button.begin();
}

void loop() {
  TimeKeeper::update();

  updateEncoder();
  dialButton.update();
  button.update();

  if (button.wasPressed())
    springy.spring.perturb(255);


  if (dialButton.wasPressed()) {
    dialMode = !dialMode;
    uint32_t dialColor = dialMode ? Adafruit_NeoPixel::Color(255, 0, 0) : Adafruit_NeoPixel::Color(0, 0, 255);
    dialRing.setColor(dialColor);
  }

  strip.clear();
  dialRing.update(TimeKeeper::getDeltaTime());
  buttonRing.update(TimeKeeper::getDeltaTime());

  Serial.println(String(springy.spring.position,3) + "\t" + buttonRing.getBrightness());
}

void updateEncoder() {
  // put your main code here, to run repeatedly:
  static int pos = 0;
  encoder.tick();

  int newPos = encoder.getPosition();
  if (pos != newPos) {
    pos = newPos;
    //dialRing.setGauge(pos % 255);
    //Serial.println(String(dialRing.nActivePixels) + "," + String(pos));
    if (dialMode) 
      springy.spring.damping = pos/10.0;
    else
      springy.spring.stiffness = pos/10.0;
    
    //Serial.println(String(springy.spring.stiffness, 3) + "," + String(springy.spring.damping, 3));
     
  }
}
