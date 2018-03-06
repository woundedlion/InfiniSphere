#include <Arduino.h>
#include "effects.h"

template <int NUM_PIXELS, int RPM>
class POVDisplay
{
  public:
    POVDisplay()
    {
      FastLED.addLeds<APA102, 11, 13, BGR, DATA_RATE_MHZ(12)>(leds, NUM_PIXELS);
    }

    
    template <typename Effect>
    void show(unsigned long duration)
    { 
      t = micros();
      effect = new Effect();
      x = 0;
      IntervalTimer timer;
      timer.begin(show_col, 1000000 / (RPM / 60) / effect->width()); 
      delay(duration);
      timer.end();
      delete effect;
      effect = NULL;
    }

  private:

    static inline void show_col() {
      noInterrupts();
      for (unsigned int y = 0; y < NUM_PIXELS / 2; ++y) {
        leds[NUM_PIXELS / 2 - y - 1] = effect->get_pixel(x, y);
        leds[NUM_PIXELS / 2 + y] =  
          effect->get_pixel((x + (effect->width() / 2)) % effect->width(), y);
      }
      FastLED.show();
      if (effect->show_bg()) {
        FastLED.showColor(effect->bg_color());
      }
      effect->advance_col(x);
      ++x;
      if (x == effect->width() / 2) {
        effect->advance_frame();
      } else if (x == effect->width()) {
        x = 0;
        effect->advance_frame();
        Serial.println(micros() - t);
        t = micros();
      }
      interrupts();
    }

    static unsigned long t;
    static CRGB leds[NUM_PIXELS];
    static Effect *effect;
    static int x;
};

template<int NUM_PIXELS, int RPM>
unsigned long POVDisplay<NUM_PIXELS, RPM>::t = 0;

template<int NUM_PIXELS, int RPM>
int POVDisplay<NUM_PIXELS, RPM>::x = 0;

template<int NUM_PIXELS, int RPM>
Effect * POVDisplay<NUM_PIXELS, RPM>::effect = NULL;

template<int NUM_PIXELS, int RPM>
CRGB POVDisplay<NUM_PIXELS, RPM>::leds[NUM_PIXELS];
