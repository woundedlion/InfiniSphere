  #include <FastLED.h>
#include <fixmath.h>
#include "led.h"
#include "images.h"

namespace {
  const unsigned int RPM = 480;
  const unsigned int NUM_PIXELS = 288;
  const unsigned int H = NUM_PIXELS / 2;
  
  CRGB leds[NUM_PIXELS];
  POVDisplay<NUM_PIXELS> pov(leds, RPM);
}

void setup() {
  delay(1000);
  Serial.begin(9600);
}

void loop() {
  while (true) { 


    while (true) {
      pov.show<DotTrails<100, H> >(120000);
      pov.show<RingTrails<64, H> >(120000);
      pov.show<Fire<100, H, 120, 120> >(120000);
      pov.show<StarsFade<64, H> >(120000);
      pov.show<Spiral<64, H> >(120000);
      pov.show<Rotate<64, H> >(120000);
      pov.show<PaletteFall<36, H, 5000, true> >(60000);
    }
/*
    pov.show<RingTrails<576, H> >(60000);
    pov.show<Kaleidoscope<288, H> >(60000);
    
    pov.show<Fire<288, H, 150, 120> >(30000);
    pov.show<Plaid<288, H, 2> >(100000);
    pov.show<DotTrails<288, H> >(600000);
    
    pov.show<TheMatrix<288, H , 135> >(60000);
    pov.show<DotTrails<288, H> >(60000);
    pov.show<RingTrails<576, H> >(60000);
    pov.show<Kaleidoscope<288, H> >(60000);
    pov.show<Rotate<288, H> >(30000);
    pov.show<Spinner<288, H> >(50000);
    
    pov.show<Spiral<4, H, 0> >(5000);
    pov.show<Spiral<8, H, 0> >(5000);
    pov.show<Spiral<16, H, 0> >(5000);
    pov.show<Spiral<288, H, 0> >(20000);
    pov.show<Spiral<288, H, 1> >(20000);
    pov.show<Spiral<288, H, 2> >(10000);
    pov.show<Spiral<288, H, 3> >(10000);
    
    pov.show<Plaid<288, H, 2> >(10000);
    pov.show<Snake<288, H, 1> >(30000);
    pov.show<Stars<288, H> >(30000);
    pov.show<Fire<288, H, 150, 120> >(30000);
    pov.show<Burnout<288, H, 0, 5> >(30000);
    pov.show<PaletteFall<288, H, 5000, true> >(30000);
    pov.show<PaletteFall<1, H, 5000, false> >(30000);
    */
  }
}


