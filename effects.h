#include <FastLED.h>
#include "rotate.h"
void trail_rainbow(CHSV& p, uint8_t falloff = 32) {
  CHSV& c = p;
  c.h += falloff;
  c.s = 255;
  c.v = qsub8(c.v, falloff);
}


template <int W, int H>
class DotTrails {
  public:
    DotTrails() {
      random16_add_entropy(random());    
      memset(leds, 0, sizeof(leds));
      for (int i = 0; i < H - 1; ++i) {
        bool rev = i % 2 == 0;
        int x = random() % W;
        dots[i] = Dot(x, rev, (random8() % 30) + 20);
      }
    }
    
    CRGB get_pixel(int x, int y) const {
      return leds[x][y];
    }
        
    static bool show_bg() { return false; }    
    static CRGB bg_color() { return CRGB::Black; }    

    void advance_col(int x) {
      for (int y = 0; y < H; ++y) {
        if (dots[y].x == x) {
          if (dots[y].drawn) {
            dots[y].drawn = false;
          } else {
            leds[x][y] = CHSV(hue - 12, 0, 255);
            move(dots[y]);
          }
        } else {
          trail_rainbow(leds[x][y], 12);
        }
      }
    } 

    void advance_frame() {
    }
    
    int width() const { return W; }
    int height() const { return H; }

private:

  struct Dot {
    Dot() : 
      x(0),
      rev(0),
      ttl(0),
      drawn(0){}

    Dot(uint8_t x, bool rev, int ttl) : 
      x(x),
      rev(rev),
      ttl(ttl),
      drawn(0){}
 
    uint8_t x;
    bool rev;
    int ttl;
    bool drawn;
  };

  void move(Dot& dot) {
    if (dot.ttl == 0) {
      dot.x = random() % W;
      dot.ttl = (random8() % 30) + 20;
    } else if (dot.rev) {
      dot.x = (dot.x - 1 + W) % W;
      dot.ttl--;
    } else {
      dot.x = (dot.x + 1) % W;
      dot.drawn = true;
      dot.ttl--;
    }
  }

  CHSV leds[W][H];
  Dot dots[H];
  uint8_t hue = HUE_RED;
};

template <int W, int H>
class RingTrails {
  public:
    RingTrails() {
      memset(leds, 0, sizeof(leds));
    }
    
    CRGB get_pixel(int x, int y) const {
      return leds[x][y];
    }
        
    static bool show_bg() { return false; }    
    static CRGB bg_color() { return CRGB::Black; }    

    void advance_col(int x) {
      if (x == 0 || x == W /2 ) {
        uint8_t dl = beatsin8(8, 1, 2);
        uint8_t dg = beatsin8(7, 1, 2, 16384);
        uint8_t dp = beatsin8(9, 1, 2, 32768);
        projection.rotate(dl, dg, dp);    
      }

      for (int y = 0; y < H; ++y) {
        Point p = projection.project(x, y);
        if (p.y == H / 2) {
          leds[x][y] = CHSV(hue - 32, 0, 255);
        } else {
          trail_rainbow(leds[x][y]);
        }
      }
    } 

    void advance_frame() {
    }
    
    int width() const { return W; }
    int height() const { return H; }

private:

  typedef typename Projection<W, H>::Point Point;
  Projection<W, H> projection;

  CHSV leds[W][H];
  uint8_t hue = HUE_RED;
};

/*

template <int W, int H>
class Spirograph {
  public:
    Spirograph() {
      memset(leds, 0, sizeof(leds));
    }
    
    CRGB get_pixel(int x, int y) const {
      return leds[x][y];
    }
        
    static bool show_bg() { return true; }    
    static CRGB bg_color() { return CRGB::Black; }    

    void advance_col(int x) {} 
    void advance_frame() {
      int d = 5;
      int r = 2;
      int p = 5;
      int x = 10 + (d * (cos8(t) - 128) / 128 + p * (cos8(d * t / r) - 128) / 128);
      int y = 10 + (d * (sin8(t) - 128) / 128 - p * (sin8(d * t/ r) - 128) / 128);
      Serial.println(x);
      Serial.println(y);
      leds[x][y] = CRGB::Blue;
      t++;
    }
    
    int width() const { return W; }
    int height() const { return H; }

private:

  CRGB leds[W][H];
  uint8_t t = 0;
};

template <int W, int H, const unsigned char (*DATA)[20][3]>
class Image
{
  public:
    
    Image()
    {}
    
    CRGB get_pixel(int x, int y) const {
      uint32_t c = pgm_read_dword(&DATA[x][y][0]); 
      return CRGB(c & 0xff, (c & 0xff00) >> 8, (c & 0xff0000) >> 16);
    }

    static bool show_bg() { return true; }    
    static CRGB bg_color() { return CRGB::Black; }    

    void advance_col(int x) {} 
    void advance_frame() {} 

    int width() const { return W; }
    int height() const { return H; }

  private:

    const unsigned char (*data_)[H][3];  
};

template <int W, int H, int S>
class Grid
{
  public:
    Grid()
    {}
    
    CRGB get_pixel(int x, int y) const {
      unsigned long c = 0;
      if (x % S == 0 || y % S == 0) {
        c = CRGB::Green;
      } 
      return c;
    }
        
    static bool show_bg() { return true; }    
    static CRGB bg_color() { return CRGB::Black; }    

    void advance_col(int x) {} 
    void advance_frame() {} 
    
    int width() const { return W; }
    int height() const { return H; }

};

template <int W, int H, int S>
class Snake
{
  public:
    Snake() : 
    head_x_(0),
    head_y_(0),
    head_x2_(W / 2),
    head_y2_(H / 2),
    x_timer_(100),
    y_timer_(100)
    {}
    
    CRGB get_pixel(int x, int y) const {
       if (is_snake(x, y, 0, 0)) {
          return CRGB::Red;
       }
       else if (is_snake(x, y, 5, 10)) {
          return CRGB::Purple;
       }
       else if (is_snake(x, y, 10, 5)) {
          return CRGB::Pink;
       }
       else if (is_snake(x, y, 5, 15)) {
          return CRGB::Yellow;
       }
       else if (is_snake(x, y, 15, 5)) {
          return CRGB::Blue;
       }
       else if (is_snake(x, y, 10, 15)) {
          return CRGB::Yellow;
       }
       else if (is_snake(x, y, 15, 10)) {
          return CRGB::Blue;
       }
       return CRGB::Black;
    }
        
    static bool show_bg() { return true; }    
    static CRGB bg_color() { return CRGB::Black; }    

    void advance_col(int x) {
      if (x_timer_) {
        head_x_ = (head_x_ + 1) % W;
        head_x2_ = (head_x2_ + 1) % W;
      }
      if (y_timer_) {
        head_y_ = addmod8(head_y_, 1, H);
        head_y2_ = addmod8(head_y2_, 1, H);
      }
    } 
    void advance_frame() {} 
    
    int width() const { return W; }
    int height() const { return H; }

private:

    bool is_snake(int x, int y, int x_offset, int y_offset) const {
       int head_x = (head_x_ + x_offset) % W;
       int head_y = (head_y_ + y_offset) % H;
       int head_x2 = (head_x2_ + x_offset) % W;
       int head_y2 = (head_y2_ + y_offset) % H;
       int x2 = W + 1 - x;
       int y2 = H + 1 - y;
       return (
              (x == head_x && y == head_y)
           || (x2 == head_x && y2 == head_y)
           || (x == head_x && y2 == head_y)
           || (x2 == head_x && y == head_y)
           || (x == head_x2 && y == head_y2)
           || (x2 == head_x2 && y2 == head_y2)
           || (x == head_x2 && y2 == head_y2)
           || (x2 == head_x2 && y == head_y2)
           
          );  
    }
  
    int head_x_;
    int head_y_;
    int head_x2_;
    int head_y2_;
    CEveryNMillis x_timer_;
    CEveryNMillis y_timer_;
};
*/
template <int W, int H>
class Kaleidoscope
{
  public:
    Kaleidoscope() {
      memset(leds_, 0, sizeof(leds_));
      fill_rainbow(palette_.entries, 16, 0, 256 / 16);
    }
    
    CRGB get_pixel(int x, int y) const {
      return leds_[x][y];
    }
        
    static bool show_bg() { return false; }    
    static CRGB bg_color() { return CRGB::Black; }    

    void advance_col(int x) {  
      nscale8(&leds_[x][0], H, 60);      
    }
    
    void advance_frame() {
      for (uint8_t i = 0; i < counts_[count_]; ++i) {
        int x = ((i * offset_) + tx_) % W; 
        int y = ty_;
        int x2 = W - 1 - x;
        int y2 =  H - 1 - y;
        leds_[x][y] = palette_[addmod8(palette_offset_, i * (16 / counts_[count_]), 16)];
        leds_[x2][y2] = leds_[x][y], palette_[addmod8(palette_offset_ , 16 - (i * (16 / counts_[count_])), 16)];
      }
      tx_ = (tx_+ 2) % W; 
      ty_ = (ty_ + inc_y_) % H;
      if (ty_ == H - 1 || ty_ == 0) {
        inc_y_ = 0 - inc_y_;
        tx_ = (tx_ + 2) % W; 
      }

      EVERY_N_MILLISECONDS(100) {
         palette_offset_ = addmod8(palette_offset_, 1, 16);
      }
      EVERY_N_MILLISECONDS(3000) {
        count_ = addmod8(count_, 1, sizeof(counts_));
        offset_ = W / counts_[count_];
      }
    }
    
    int width() const { return W; }
    int height() const { return H; }

private:
  
  CRGB leds_[W][H];
  uint8_t counts_[9] = {1, 2, 4, 5, 6, 8, 10, 12, 20};
  uint8_t count_ = 0;
  int offset_ = W / counts_[count_];
  int tx_ = 0;
  int ty_ = 0;
  int inc_y_ = 4;
  CRGBPalette16 palette_;
  uint8_t palette_offset_ = 0;
};
/*
template <int W, int H, int S>
class Plaid
{
  public:
    Plaid() :
    color_shift_timer_(100),
    c1_(CHSV(random8(), 255, 255)),
    c2_(CHSV(random8(), 255, 255)),
    c3_(CHSV(HUE_RED, 0, 0))
    {}
    
    CRGB get_pixel(int x, int y) const {
      CHSV c(c3_);
      if (x % 4 == 0) {
        c = c1_;
        if (y % 4 == 0) {
          return blend(c, c2_, 128);
        }
      } else if (y % 4 == 0) {
        return c2_;
      }
      return c;
    }
        
    static bool show_bg() { return true; }    
    static CRGB bg_color() { return CRGB::Black; }    

    void advance_col(int x) {} 
    
    void advance_frame() {
      if (color_shift_timer_) {
        c1_.hue += 1;
        c2_.hue += 1;
//        c3_.hue += 1;
      }
    } 
    
    int width() const { return W; }
    int height() const { return H; }

private:

  CEveryNMillis color_shift_timer_;
  CHSV c1_;
  CHSV c2_;
  CHSV c3_;
};
*/

template <unsigned int W, unsigned int H>
class Spiral
{
  public:
    Spiral()
    {
      fill_rainbow(palette_.entries, 16, 0, 256 / 16);
    }
    
    CRGB get_pixel(unsigned int x, unsigned int y) const {
      if ((x + y) % (spread_ + 1) == 0) {
        return palette_[(uint8_t)(((x + y) / (spread_ + 1)) % 16)];
      }
      return CRGB::Black;
    }
        
    static bool show_bg() { return true; }    
    static CRGB bg_color() { return CRGB::Black; }    
   
    void advance_col(int x) {} 
    void advance_frame() {
      EVERY_N_MILLIS(5000) {
        if (spread_++ > 5) {
          spread_ = 0;
        }
      }
    }
  
    int width() const { return W; }
    int height() const { return H; }

  private:
        
    CRGBPalette16 palette_;
    uint8_t spread_ = 0;
};

/*
template <int W, int H>
class Stars
{
  public:
    Stars() :
    hue_(0)
    {}
    
    CRGB get_pixel(int x, int y) const {
      return random8() > 250 ? CRGB(CHSV(hue_, 255, 255)) : CRGB::Black;
    }
        
    static bool show_bg() { return true; }    
    static CRGB bg_color() { return CRGB::Black; }    

    void advance_col(int x) {} 
    void advance_frame() {
      hue_++;
    } 
    
    int width() const { return W; }
    int height() const { return H; }
    
    private:
    
      uint8_t hue_;
};
*/
template <int W, int H>
class StarsFade
{
  public:
    StarsFade() :
    hue_(0)
    {
      memset(leds_, 0, sizeof(leds_));
    }
    
    CRGB get_pixel(int x, int y) const {
      return leds_[x][y];
    }
        
    static bool show_bg() { return true; }    
    static CRGB bg_color() { return CRGB::Black; }    

    void advance_col(int x) {
      nscale8(&leds_[x][0], H, 200);      
      uint8_t xn = map8(random8(), 0, W - 1);
      uint8_t yn = map8(random8(), 0, H - 1);
      leds_[xn][yn] = CRGB(CHSV(hue_, 255, 255));
    }
    
    void advance_frame() {
      hue_++;
    } 
    
    int width() const { return W; }
    int height() const { return H; }
    
    private:

      CRGB leds_[W][H];
      uint8_t hue_;
};


template <int W, int H>
class Spinner
{
  public:
    Spinner() :
      spin_timer_(75),
      pos_(0)
    {
      memset(palette1_, 0, sizeof(palette1_));
      memset(palette2_, 0, sizeof(palette1_));
      palette1_[0] = palette1_[8] = CRGB::Pink;
      palette1_[1] = palette1_[9] = CRGB::Purple;
      palette2_[0] = palette2_[8] = CRGB::Blue;
      palette2_[1] = palette2_[9] = CRGB::Purple; 
    }
    
    CRGB get_pixel(int x, int y) const {
      if (!swap) {
        if (y % (p[i] * 2) < p[i] ) {
          return palette1_[addmod8(x, pos_, 16)];
        } else {
          return palette2_[(uint8_t)(pos_ - x) % 16];
        }
      } else {
        if (y % (p[i] * 2) < p[i] ) {
          return palette2_[(uint8_t)(pos_ - x) % 16];
        } else {
          return palette1_[addmod8(x, pos_, 16)];
        }
      }
    }
   
    static bool show_bg() { return false; }    
    static CRGB bg_color() { return CRGB::Black; }    

    void advance_col(int x) {
      if (spin_timer_) {
        pos_ = (pos_ + 1) % W;
      }
    } 

    void advance_frame() {
      EVERY_N_SECONDS(5) {
        if ((!swap && (i == sizeof(p) - 1)) 
        || (swap && (i == 0)) ) {
          swap = !swap;
        } else {
          i += swap ? -1 : 1;
        }
      }
    } 
           
    int width() const { return W; }
    int height() const { return H; }
           
  private:
       
    CRGBPalette16 palette1_;
    CRGBPalette16 palette2_;
    CEveryNMillis spin_timer_;
    int pos_;
    bool swap = false;
    uint8_t p[7] = { 144, 72, 36, 18, 9};
    uint8_t i = 0;
};

template <int W, int H, uint8_t COOL, uint8_t SPARK>
class Fire
{
  public:
  
    Fire() {
      random16_add_entropy(random());    
      memset(heat_, 0 , sizeof(heat_));
    }
  
    CRGB get_pixel(int x, int y) const {   
      return HeatColor(heat_[x][y]);
    }

    static bool show_bg() { return false; }    
    static CRGB bg_color() { return CRGB::Black; }    

    void advance_col(int x) {
      cool(x);
      rise(x);
      spark(x);    
    } 

    void advance_frame() {} 

    int width() const { return W; }
    int height() const { return H; }    
  
  private:
  
    inline void cool(int x) {
       for (int y = 0; y < H; ++y) {
         heat_[x][y] = max(0, heat_[x][y] - random(0, ((COOL * 10) / H) + 2));          
      }
    }

    inline void rise(uint8_t x) {
      for (int y = H - 1; y >= 2; --y) {
        heat_[x][H - 1 - y] = (heat_[x][H - 1 - y + 1] + heat_[x][H - 1 - y + 2] + heat_[x][H - 1 - y + 2]) / 3;
      }       
    }
 
    inline void spark(uint8_t x) {
      if (random8() < SPARK) {
        int y = random8(3);
        heat_[x][H - 1 - y] = min(255, heat_[x][H - 1 - y] + random8(160, 255));
      }
    }
    
    uint8_t heat_[W][H];
};

template <int W, int H, int DURATION, bool BG>
class PaletteFall
{
  public:

    PaletteFall() :
      timer_(DURATION),
      palette_idx_(0),
      palette_(RainbowColors_p),
      palette_offset_(0)
    {
    }

    CRGB get_pixel(int x, int y) const {
      return palette_[addmod8(H - 1 - y, palette_offset_, 16)];
    }

    static bool show_bg() { return BG; }    
    static CRGB bg_color() { return CRGB::Black; }    

    void advance_col(int x) {
    }
    
    void advance_frame() {
      palette_offset_ = addmod8(palette_offset_, 1, 16);
      if (timer_) {
        switch_palette();
      }
    } 

    int width() const { return W; }
    int height() const { return H; }

  private:

    void switch_palette() {
      switch (palette_idx_) {
        case 0:
          palette_ = HeatColors_p;
          break;
        case 2:
          palette_ = PartyColors_p;
          break;
        case 3:
          palette_ = RainbowColors_p;
          break;
        case 4:
          palette_ = RainbowStripeColors_p;
          break;
      }
      palette_idx_ = addmod8(palette_idx_, 1, 5);
    }

    CEveryNMillis timer_;
    uint8_t palette_idx_ = 0;
    CRGBPalette16 palette_;
    uint8_t palette_offset_;
};

template <int W, int H, uint8_t HUE>
class TheMatrix
{
  public:

    TheMatrix()
    {
      memset8(pixels_, 0, sizeof(pixels_));
      random16_add_entropy(random());
    }

    CRGB get_pixel(int x, int y) const {
      return blend(CHSV(HUE + (3 * y), 255, 40), 
                        CHSV(HUE + (3 * (H - 1 - y)), 255, 255),
                        pixels_[x][y]);
    }
    static bool show_bg() { return true; }    
    static CRGB bg_color() { return CRGB::Black; }    

    void advance_col(int x) {
      fall(x);
      generate(x);
    }
    
    void advance_frame() {
    } 

    int width() const { return W; }
    int height() const { return H; }

  private:

    void fall(int x) {
      memmove8(&pixels_[x][1], &pixels_[x][0], H - 1);
      pixels_[x][0] = 0;
    }
    
    void generate(int x) {
      if (random8() < 15) {
        pixels_[x][2] = 255;
        pixels_[x][1] = 100;
        pixels_[x][0] = 30;
      }
    }
    
    uint8_t pixels_[W][H];
};

/*

template <int W, int H, uint8_t HUE, uint8_t BURNRATE>
class Burnout
{
  public:

    Burnout() :
    timer_(30),
    burn_idx_(0)
    {
      random16_add_entropy(random());
      memset8(pixels_, INIT, sizeof(pixels_));
      fill_seq(burn_, W * H);
      shuffle(burn_, W * H);
    }

    CRGB get_pixel(int x, int y) const {
      if (pixels_[x][y] & BURN) {
          return CHSV(HUE + (3 * y), 255, 255);
      }
      if (pixels_[x][y] == INIT) {
          return CHSV(HUE + (3 * y), 255, 40);
      }
      return CRGB::Black;
    }
    
    static bool show_bg() { return true; }    
    static CRGB bg_color() { return CRGB::Black; }    

    void advance_col(int x) {
      if (timer_) {
        burnout();
      }
      fall(x);
    }
    
    void advance_frame() {
    } 

    int width() const { return W; }
    int height() const { return H; }

  private:

    enum State {
      NONE,
      INIT,
      BURN = 0x80,
    };
    
    void burnout() {
      if (burn_idx_ < W * H) {
        *((uint8_t *)pixels_ + burn_[burn_idx_++]) = BURN;
      }
    }

    void fall(int x) {
      for (int y = 0; y < H; ++y) {
        if (pixels_[x][y] & BURN) {
          pixels_[x][y] ^= BURN;
          if (y < (H - 1)) {
            pixels_[x][y + 1] |= BURN;
            ++y;          
          }
        }
      }
    }

    void fill_seq(short *arr, int len) {
      for (int i = 0; i < len; ++i) {
        arr[i] = i;
      } 
    }

    void shuffle(short *arr, int len) {
      for (int i = 0; i < len - 1; ++i) {
        short j = random(i, len);
        short temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
      }
    }
    
    CEveryNMillis timer_;
    
    uint8_t pixels_[W][H];
    short burn_[W * H];
    int burn_idx_;
};

*/
template <int W, int H>
class Rotate
{
  public:

    Rotate() {
      memset(buf, 0, sizeof(buf));
      fill_rainbow(pal.entries, 16, 0, 256 / 16);
    }

    CRGB get_pixel(int x, int y) const {
      return buf[x][y];
    }

    bool show_bg() { return bg; }    
    static CRGB bg_color() { return CRGB::Black; }    

    void advance_col(int x) {
      if (x == 0 || x == W / 2) {
        projection.rotate(0, 5, 0);
      }
      for (int y = 0; y < H; ++y) {
        Point p = projection.project(x, y);
 //       if (p.y == 24 || p.y == H / 2 || p.y == H - 1 - 24) {
        if (p.y % (H / 6) == 0) {
          buf[x][y] =  ColorFromPalette(pal, (p.x + c_off) * 255 / W);
        } else {
          buf[x][y] = CRGB::Black;
        }
      }      
    }
    
    void advance_frame() {
        ++c_off;
        EVERY_N_SECONDS(10) {
//          bg = !bg;
        }
    }

    int width() const { return W; }
    int height() const { return H; }

  private:
  
    typedef typename Projection<W, H>::Point Point;
    Projection<W, H> projection;

    CRGB buf[W][H];
    CRGBPalette16 pal;
    uint8_t c_off = 0;
    bool bg = 0;
};
/*

template <int W, int H>
class RotateWave
{
  public:

    RotateWave() {
      memset(buf, 0, sizeof(buf));
      fill_rainbow(pal.entries, 16, 0, 256 / 16);
    }

    CRGB get_pixel(int x, int y) const {
      return buf[x][y];
    }

    bool show_bg() { return false; }    
    static CRGB bg_color() { return CRGB::Black; }    

    void advance_col(int x) {
      if (x == 0 || x == W / 2) {
        projection.rotate(0, 5, 0);
      }
      for (int y = 0; y < H; ++y) {
        Point p = projection.project(x, y);
        if (p.y == 2 || p.y == 18 || equals_wave(p)) {
          buf[x][y] =  ColorFromPalette(pal, (p.x + c_off) * 255 / W);
        } else {
          buf[x][y] = CRGB::Black;
        }
      }      
    }
    
    void advance_frame() {
        ++c_off;
    }

    int width() const { return W; }
    int height() const { return H; }

  private:
  
    typedef typename Projection<W, H>::Point Point;

    bool equals_wave(const Point& p) const {
      uint8_t freq = W / 3;
      int amp = beatsin8(5, 0, 3);
      int origin = H / 2;
      return p.y == map8(triwave8(map(mod8(p.x, freq), 0, freq - 1 , 0, 255)), origin - amp, origin + amp);
    }

    CRGB buf[W][H];
    CRGBPalette16 pal;
    int c_off = 0;
    Projection<W, H> projection;
};

*/

