
#include <RotaryEncoder.h>
#include <Adafruit_NeoPixel.h>

#define ROTARY_PIN_IN1 9
#define ROTARY_PIN_IN2 8
#define BUTTON_PIN_IN 7
#define WS2812_DATA_PIN 4

#define NUMPIXELS 32  // Popular NeoPixel ring size

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder encoder(ROTARY_PIN_IN1, ROTARY_PIN_IN2, RotaryEncoder::LatchMode::TWO03);
Adafruit_NeoPixel pixels(NUMPIXELS, WS2812_DATA_PIN, NEO_GRB + NEO_KHZ800);

void updateEncoder() {
  // put your main code here, to run repeatedly:
  static int pos = 0;
  encoder.tick();

  int newPos = encoder.getPosition();
  if (pos != newPos) {
    Serial.print("pos:");
    Serial.print(newPos);
    Serial.print(" dir:");
    Serial.println((int)(encoder.getDirection()));
    pos = newPos;
    setPixels(abs(pos) / 4);
  }  // if
}

void updateButton() {
  static int status = 1;
  int newStatus = digitalRead(BUTTON_PIN_IN);
  if (status != newStatus) {
    delay(50);
    Serial.print(String("Button is ") + String(newStatus ? "UP" : "DOWN") + "!\n");
    status = newStatus;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (! Serial);

  // END of Trinket-specific code.
  //pixels.setBrightness(100);
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear();
  pixels.show();
  pinMode(BUTTON_PIN_IN, INPUT_PULLUP);
}

void setPixels(int gaugeValue) {
  pixels.clear();
  gaugeValue = gaugeValue % NUMPIXELS/2 + 1;
  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  for (int i = 0; i < gaugeValue; i++) {  // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(0, 150, 0));
  }

  pixels.show();  // Send the updated pixel colors to the hardware.
}


void loop() {
  pixels.clear();  // Set all pixel colors to 'off'

  updateEncoder();
  updateButton();
}
