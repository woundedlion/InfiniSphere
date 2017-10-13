  #include <FastLED.h>
#include "led.h"
#include "images.h"

namespace {
  const unsigned int RPM = 480;
  const unsigned int NUM_PIXELS = 288;
  const unsigned int H = NUM_PIXELS / 2;
  POVDisplay<NUM_PIXELS, RPM> pov;
}

void setup() {
  delay(1000);
  Serial.begin(9600);
}

void loop() {
  while (true) { 
          pov.show<Rotate<64, H> >(60000);

      pov.show<DotTrails<100, H> >(120000);
      pov.show<StarsFade<64, H> >(120000);
      pov.show<WaveTrails<100, H> >(120000);
      pov.show<RingTrails<64, H> >(120000);
      pov.show<Spiral<64, H> >(60000);
      pov.show<Fire<100, H, 120, 120> >(120000);
      pov.show<Rotate<64, H> >(60000);
      pov.show<PaletteFall<36, H, 5000, true> >(60000);
  }
}


