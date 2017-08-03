#include <FastLED.h>
#include <fixmath.h>
#include "led.h"
#include "images.h"

namespace {
  const unsigned int RPM = 480;
  const int NUM_PIXELS = 288;
  const int H = NUM_PIXELS / 2;
  
  CRGB leds[NUM_PIXELS];
  POVDisplay<NUM_PIXELS> pov(leds, RPM);
}

void setup() {
  Serial.begin(9600);
}

void loop() {
  while (true) { 
//    pov.show<StarsFade<40, 20> >(900000);
//    pov.show<Spirograph<40, 20> >(900000);
//    pov.show<RotateWave<64, 20> >(900000);

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
  }
}


