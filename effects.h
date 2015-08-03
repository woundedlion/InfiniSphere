#include <FastLED.h>

template <uint8_t W, uint8_t H>
class Image
{
  public:
    Image(const unsigned char (*data)[H][3]) :
      data_(data)
    {}
    
    CRGB get_pixel(int x, int y) const {
      uint32_t c = pgm_read_dword(&data_[x][y][0]);
      return CRGB((c & 0xff) << 16, c & 0xff00, (c & 0xff0000) >> 16);
    }
    
    void advance_col(uint8_t x) {} 

    uint8_t width() const { return W; }
    uint8_t height() const { return H; }

  private:
    const unsigned char (*data_)[H][3];  
};

template <uint8_t W, uint8_t H, uint8_t S>
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
        
    void advance_col(uint8_t x) {} 
    
    uint8_t width() const { return W; }
    uint8_t height() const { return H; }

};

template <uint8_t W, uint8_t H>
class Spiral
{
  public:
    Spiral()
    {
      fill_rainbow(palette_.entries, 16, 0, 256/16);
    }
    
    CRGB get_pixel(int x, int y) const {
      return palette_[(x + y) % 15];
    }
        
    void advance_col(uint8_t x) {} 
    
    uint8_t width() const { return W; }
    uint8_t height() const { return H; }

  private:
        
    CRGBPalette16 palette_;
};

template <uint8_t W, uint8_t H>
class Stars
{
  public:
    Stars()
    {}
    
    CRGB get_pixel(int x, int y) const {
      return random8() > 250 ? CRGB::Goldenrod : CRGB::Black;
    }
        
    void advance_col(uint8_t x) {} 
    
    uint8_t width() const { return W; }
    uint8_t height() const { return H; }
};

template <uint8_t W, uint8_t H, uint8_t T, uint8_t RPM>
class TheMatrix
{
  public:
    TheMatrix()
    {
      random16_add_entropy(random());
      c_.setHue(random8());
      memset8(&pos_[0], 0, sizeof(pos_));
      for (int i = 0; i < W; ++i) {
         // random fall rate in pix/sec
         rate_[i] = random8(1,5);
      }
    }
    
    CRGB get_pixel(int x, int y) const {
      uint8_t dist = static_cast<uint8_t>(pos_[x]) - y;
      if (dist >= 0 && dist <= T) {
        // trails fade to black
        return CRGB(c_).fadeLightBy(255 * dist / T);
      } else {
        return CRGB::Black; 
      }
    }
               
    void advance_col(uint8_t x) {
      fall(x);    
    } 
    
    uint8_t width() const { return W; }
    uint8_t height() const { return H; }

  private:
   
    inline void fall(uint8_t x) {
      pos_[x] += rate_[x] * 60 / RPM;
    }
    
    float pos_[W];
    uint8_t rate_[W];
    CRGB c_;
};

template <uint8_t W, uint8_t H, uint8_t P>
class Spinner
{
  public:
    Spinner() :
      spin_timer_(125),
      pos_(0)
    {
      fill_rainbow(palette_.entries, P, 0, 256 / P);
    }
    
    CRGB get_pixel(int x, int y) const {    
      if (mod8(y, 2) == 0) {
        return palette_[addmod8(x, pos_, P)];
      } else {
        return palette_[P - 1 - addmod8(x, pos_, P)];        
      }
    }

    void advance_col(uint8_t x) {
      if (spin_timer_) {
        pos_ = addmod8(pos_, 1, 16);
      }
    } 
       
    uint8_t width() const { return W; }
    uint8_t height() const { return H; }
           
  private:
       
    CEveryNMillis spin_timer_;
    CRGBPalette16 palette_;
    uint8_t pos_;
};

template <uint8_t W, uint8_t H, uint8_t COOL, uint8_t SPARK>
class Fire
{
  public:
  
    Fire() {
      random16_add_entropy(random());    
    }
  
    CRGB get_pixel(int x, int y) const {   
      return HeatColor(heat_[x][y]);
    }

    void advance_col(uint8_t x) {
      cool(x);
      rise(x);
      spark(x);    
    } 

    uint8_t width() const { return W; }
    uint8_t height() const { return H; }    
  
  private:
  
    inline void cool(uint8_t x) {
       for (uint8_t y = 0; y < H; ++y) {
         heat_[x][y] = qsub8(heat_[x][y], random(0, ((COOL * 10) / H) + 2));          
      }
    }

    inline void rise(uint8_t x) {
      for (uint8_t y = H; y >= 2; --y) {
        heat_[x][H - y] = (heat_[x][H - y + 1] + heat_[x][H - y + 2] + heat_[x][H - y + 2]) / 3;
      }       
    }
 
    inline void spark(uint8_t x) {
      if (random8() < SPARK) {
        uint8_t y = random8(7);
        heat_[x][H - y] = qadd8(heat_[x][H - y], random8(160, 255));
      }
    }
    
    uint8_t heat_[W][H];
};
