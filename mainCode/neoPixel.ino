#include <Adafruit_NeoPixel.h>

#define PIN 2

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);

//void setup() {
//  strip.begin();
//  strip.show(); // Initialize all pixels to 'off'
//}
//
//void loop() {
//  stateIdle();
//  delay(2000);
//  stateActivated();
//  delay(2000);
//  stateBuddyFound();
//  delay(2000);
//  stateLocked();
//  delay(2000);
//}

void stateIdle() {
  colorWipe(strip.Color(137/8, 14/8, 14/8));
}

void stateActivated() {
  colorWipe(strip.Color(41/8, 242/8, 232/8));
}

void stateBuddyFound() {
  colorWipe(strip.Color(247/8, 153/8, 12/8));
}

void stateLocked() {
  colorWipe(strip.Color(27/8, 12/8, 247/8));
}

void halfHalf(uint32_t c, uint32_t d) {
  for (uint16_t i=0; i < strip.numPixels()/2; i++) {
    strip.setPixelColor(i, c);
    strip.show();
  }
  for (uint16_t i=strip.numPixels()/2; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, d);
    strip.show();
  }
}

void colorWipe(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
  }
}
