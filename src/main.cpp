
// #include <FastLED.h>
#include "LPD8806.h"
#include "SPI.h"

// Example to control LPD8806-based RGB LED Modules in a strip!
// NOTE: WILL NOT WORK ON TRINKET OR GEMMA due to floating-point math
/*****************************************************************************/

const int NUM_PIXELS = 240;
LPD8806 strip = LPD8806(NUM_PIXELS);
uint32_t pixelBuffer[NUM_PIXELS];// use a local pixel buffer to help with effects/reveals

void setup()
{
  // Serial.begin(9600);
  // Start up the LED strip
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();
}

// function prototypes, do not remove these!
// void colorChase(uint32_t c, uint8_t wait);
void colorWipe(uint32_t c, uint8_t wait);
void colorEffects(uint32_t c);
void ditherRevealBuffer(uint8_t wait);
// void dither(uint32_t c, uint8_t wait);
void scanner(uint8_t r, uint8_t g, uint8_t b, uint8_t wait);
void wave(uint32_t c, int repeat, int cycles, uint8_t wait);
void waveReveal(uint32_t c, int cycles, uint8_t wait);
void rainbowWipe(uint8_t wait);
void rainbowCycle(int repeat, uint8_t wait);
void revealAndPlayEffect(uint32_t color);
void getWaveBuffer(uint32_t *buffer, uint32_t c, int cycles, int offset);
void getRainbowBuffer(uint32_t *buffer, int offset);
void randomReveal(uint8_t wait);
void wipeRevealBuffer(uint8_t wait);
void colorScanner(uint32_t color, uint8_t wait);
void theaterChaseRainbowBuffer();
void theaterChaseRainbow(uint8_t wait);

uint32_t Wheel(uint16_t WheelPos);

uint8_t delayMsec = 10;

uint32_t BlueColor = strip.Color(0, 127, 0);
uint32_t RedColor = strip.Color(127, 0, 0);
uint32_t GreenColor = strip.Color(0, 0, 127);
uint32_t CyanColor = strip.Color(0, 127, 127);
uint32_t MagentaColor = strip.Color(127, 127, 0);
uint32_t YellowColor = strip.Color(127, 0, 127);
uint32_t BlackColor = strip.Color(0, 0, 0);
uint32_t WhiteColor = strip.Color(127, 127, 127);

uint32_t colorArray[6] = {BlueColor, RedColor, GreenColor, CyanColor, MagentaColor, YellowColor};

byte extractbyte(uint32_t r, uint8_t pos)
{
  return (r >> (8 * pos)) & 0xFF;
}

byte getRGB(uint32_t color, byte rgb)
{
  if (rgb == 'r')
  {
    return extractbyte(color, 1) & 0x7f; // bit mask to 7bits
  }
  if (rgb == 'g')
  {
    return extractbyte(color, 0) & 0x7f; // bit mask to 7bits
  }
  if (rgb == 'b')
  {
    return extractbyte(color, 2) & 0x7f; // bit mask to 7bits
  }
}

void loop()
{
  uint32_t randColor = colorArray[rand() % 6];
  revealAndPlayEffect(randColor);
}

void revealAndPlayEffect(uint32_t color)
{
  int effect = rand() % 4;
  // int effect = 3;
  if (effect == 0)
  {
    getWaveBuffer(pixelBuffer, color, 5, 0);
    randomReveal(delayMsec);
    wave(color, 2, 5, delayMsec);
  }
  if (effect == 1)
  {
    getRainbowBuffer(pixelBuffer, 0);
    randomReveal(delayMsec);
    rainbowCycle(2, delayMsec);
  }
  if (effect == 2)
  {
    colorScanner(color, delayMsec);
  }
  if (effect == 3)
  {
    // generate a buffer
    theaterChaseRainbowBuffer();
    // reveal the buffer...
    randomReveal(delayMsec);
    theaterChaseRainbow(delayMsec);
  }
}

void randomReveal(uint8_t wait)
{
  int randRevealIndex = rand() % 2;
  if (randRevealIndex == 0)
  {
    wipeRevealBuffer(wait);
  }
  else if (randRevealIndex == 1)
  {
    ditherRevealBuffer(wait);
  }
}

void wipeRevealBuffer(uint8_t wait)
{
  int i;
  for (i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, pixelBuffer[i]);
    strip.show(); // write all the pixels out
    delay(wait);
  }
}

void colorScanner(uint32_t color, uint8_t wait)
{
  byte r = getRGB(color, 'r');
  byte g = getRGB(color, 'g');
  byte b = getRGB(color, 'b');
  scanner(r, b, g, wait);
}

void getRainbowBuffer(uint32_t *buffer, int offset)
{
  int i;
  for (i = 0; i < strip.numPixels(); i++)
  {
    buffer[i] = Wheel(((i * 384 / strip.numPixels()) + offset) % 384);
  }
}

// Cycle through the color wheel, equally spaced around the belt
void rainbowCycle(int repeat, uint8_t wait)
{
  uint16_t i, j;
  for (j = 0; j < 384 * repeat; j++)
  { // 5 cycles of all 384 colors in the wheel
    getRainbowBuffer(pixelBuffer, j);
    for (int i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, pixelBuffer[i]);
    }
    strip.show();
    delay(wait);
  }
}

void rainbowWipe(uint8_t wait)
{
  getRainbowBuffer(pixelBuffer, 0);
  int i;
  for (i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, pixelBuffer[i]);
    strip.show(); // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait)
{
  int i;
  for (i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Chase a dot down the strip
// good for testing purposes
// void colorChase(uint32_t c, uint8_t wait)
// {
//   int i;

//   for (i = 0; i < strip.numPixels(); i++)
//   {
//     strip.setPixelColor(i, 0); // turn all pixels off
//   }

//   for (i = 0; i < strip.numPixels(); i++)
//   {
//     strip.setPixelColor(i, c); // set one pixel
//     strip.show();              // refresh strip display
//     delay(wait);               // hold image for a moment
//     strip.setPixelColor(i, 0); // erase pixel (but don't refresh yet)
//   }
//   strip.show(); // for last erased pixel
// }

void ditherRevealBuffer(uint8_t wait)
{

  // Determine highest bit needed to represent pixel index
  int hiBit = 0;
  int n = strip.numPixels() - 1;
  for (int bit = 1; bit < 0x8000; bit <<= 1)
  {
    if (n & bit)
      hiBit = bit;
  }

  int bit, reverse;
  for (int i = 0; i < (hiBit << 1); i++)
  {
    // Reverse the bits in i to create ordered dither:
    reverse = 0;
    for (bit = 1; bit <= hiBit; bit <<= 1)
    {
      reverse <<= 1;
      if (i & bit)
        reverse |= 1;
    }
    strip.setPixelColor(reverse, pixelBuffer[reverse]);
    strip.show();
    delay(wait);
  }
  delay(250); // Hold image for 1/4 sec
}

// An "ordered dither" fills every pixel in a sequence that looks
// sparkly and almost random, but actually follows a specific order.
// void dither(uint32_t c, uint8_t wait)
// {

//   // Determine highest bit needed to represent pixel index
//   int hiBit = 0;
//   int n = strip.numPixels() - 1;
//   for (int bit = 1; bit < 0x8000; bit <<= 1)
//   {
//     if (n & bit)
//       hiBit = bit;
//   }

//   int bit, reverse;
//   for (int i = 0; i < (hiBit << 1); i++)
//   {
//     // Reverse the bits in i to create ordered dither:
//     reverse = 0;
//     for (bit = 1; bit <= hiBit; bit <<= 1)
//     {
//       reverse <<= 1;
//       if (i & bit)
//         reverse |= 1;
//     }
//     strip.setPixelColor(reverse, c);
//     strip.show();
//     delay(wait);
//   }
//   delay(250); // Hold image for 1/4 sec
// }

// "Larson scanner" = Cylon/KITT bouncing light effect
void scanner(uint8_t r, uint8_t g, uint8_t b, uint8_t wait)
{
  int i, j, pos, dir;

  pos = 0;
  dir = 1;

  for (i = 0; i < ((strip.numPixels() - 1) * 2); i++)
  {
    // Draw 5 pixels centered on pos.  setPixelColor() will clip
    // any pixels off the ends of the strip, no worries there.
    // we'll make the colors dimmer at the edges for a nice pulse
    // look
    strip.setPixelColor(pos - 2, strip.Color(r / 4, g / 4, b / 4));
    strip.setPixelColor(pos - 1, strip.Color(r / 2, g / 2, b / 2));
    strip.setPixelColor(pos, strip.Color(r, g, b));
    strip.setPixelColor(pos + 1, strip.Color(r / 2, g / 2, b / 2));
    strip.setPixelColor(pos + 2, strip.Color(r / 4, g / 4, b / 4));

    strip.show();
    delay(wait);
    // If we wanted to be sneaky we could erase just the tail end
    // pixel, but it's much easier just to erase the whole thing
    // and draw a new one next time.
    for (j = -2; j <= 2; j++)
      strip.setPixelColor(pos + j, strip.Color(0, 0, 0));
    // Bounce off ends of strip
    pos += dir;
    if (pos < 0)
    {
      pos = 1;
      dir = -dir;
    }
    else if (pos >= strip.numPixels())
    {
      pos = strip.numPixels() - 2;
      dir = -dir;
    }
  }
}

// Sine wave effect
#define PI 3.14159265

void getWaveBuffer(uint32_t *buffer, uint32_t c, int cycles, int offset)
{
  float y;
  byte r, g, b, r2, g2, b2;

  // Need to decompose color into its r, g, b elements
  g = (c >> 16) & 0x7f;
  r = (c >> 8) & 0x7f;
  b = c & 0x7f;

  for (int i = 0; i < strip.numPixels(); i++)
  {
    y = sin(PI * (float)cycles * (float)(offset + i) / (float)strip.numPixels());
    if (y >= 0.0)
    {
      // Peaks of sine wave are white
      y = 1.0 - y; // Translate Y to 0.0 (top) to 1.0 (center)
      r2 = 127 - (byte)((float)(127 - r) * y);
      g2 = 127 - (byte)((float)(127 - g) * y);
      b2 = 127 - (byte)((float)(127 - b) * y);
    }
    else
    {
      // Troughs of sine wave are black
      y += 1.0; // Translate Y to 0.0 (bottom) to 1.0 (center)
      r2 = (byte)((float)r * y);
      g2 = (byte)((float)g * y);
      b2 = (byte)((float)b * y);
    }
    buffer[i] = strip.Color(r2, g2, b2);
  }
}

void wave(uint32_t c, int repeat, int cycles, uint8_t wait)
{
  for (int x = 0; x < (strip.numPixels() * repeat); x++)
  {
    getWaveBuffer(pixelBuffer, c, cycles, x);
    for (int i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, pixelBuffer[i]);
    }
    strip.show();
    delay(wait);
  }
}

// color wipe, but with the pixel bufffer
void waveReveal(uint32_t c, int cycles, uint8_t wait)
{
  // get first 'wave'
  getWaveBuffer(pixelBuffer, c, cycles, 0);
  for (int i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, pixelBuffer[i]);
    strip.show();
    delay(wait);
  }

  // reveal via either wipe, dither
}

/* Helper functions */

// Input a value 0 to 384 to get a color value.
// The colours are a transition r - g - b - back to r

uint32_t Wheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch (WheelPos / 128)
  {
  case 0:
    r = 127 - WheelPos % 128; // red down
    g = WheelPos % 128;       // green up
    b = 0;                    // blue off
    break;
  case 1:
    g = 127 - WheelPos % 128; // green down
    b = WheelPos % 128;       // blue up
    r = 0;                    // red off
    break;
  default:
    b = 127 - WheelPos % 128; // blue down
    r = WheelPos % 128;       // red up
    g = 0;                    // green off
    break;
  }
  return (strip.Color(r, g, b));
}
// void nscale8( uint32_t* pixelBuffer, uint16_t num_leds, uint8_t scale)
// {
//     for( uint16_t i = 0; i < num_leds; ++i) {
//         pixelBuffer[i].nscale8( scale);
//     }
// }

#define LIB8STATIC __attribute__ ((unused)) static inline
#define LIB8STATIC_ALWAYS_INLINE __attribute__ ((always_inline)) static inline

#if defined(__AVR__)

// AVR ATmega and friends Arduino

#define QADD8_C 0
#define QADD7_C 0
#define QSUB8_C 0
#define ABS8_C 0
#define ADD8_C 0
#define SUB8_C 0
#define AVG8_C 0
#define AVG7_C 0
#define AVG16_C 0
#define AVG15_C 0

#define QADD8_AVRASM 1
#define QADD7_AVRASM 1
#define QSUB8_AVRASM 1
#define ABS8_AVRASM 1
#define ADD8_AVRASM 1
#define SUB8_AVRASM 1
#define AVG8_AVRASM 1
#define AVG7_AVRASM 1
#define AVG16_AVRASM 1
#define AVG15_AVRASM 1

// Note: these require hardware MUL instruction
//       -- sorry, ATtiny!
#define SCALE8_C 0
#define SCALE16BY8_C 0
#define SCALE16_C 0
#define MUL8_C 0
#define QMUL8_C 0
#define EASE8_C 0
#define BLEND8_C 0
#define SCALE8_AVRASM 1
#define SCALE16BY8_AVRASM 1
#define SCALE16_AVRASM 1
#define MUL8_AVRASM 1
#define QMUL8_AVRASM 1
#define EASE8_AVRASM 1
#define CLEANUP_R1_AVRASM 1
#define BLEND8_AVRASM 1
#endif // end of !defined(LIB8_ATTINY)

// end of #elif defined(__AVR__)
/// This version of scale8 does not clean up the R1 register on AVR
/// If you are doing several 'scale8's in a row, use this, and
/// then explicitly call cleanup_R1.
LIB8STATIC_ALWAYS_INLINE uint8_t scale8_LEAVING_R1_DIRTY( uint8_t i, uint8_t scale)
{
#if SCALE8_C == 1
#if (FASTLED_SCALE8_FIXED == 1)
    return (((uint16_t)i) * ((uint16_t)(scale)+1)) >> 8;
#else
    return ((int)i * (int)(scale) ) >> 8;
#endif
#elif SCALE8_AVRASM == 1
    asm volatile(
#if (FASTLED_SCALE8_FIXED==1)
        // Multiply 8-bit i * 8-bit scale, giving 16-bit r1,r0
        "mul %0, %1          \n\t"
        // Add i to r0, possibly setting the carry flag
        "add r0, %0         \n\t"
        // load the immediate 0 into i (note, this does _not_ touch any flags)
        "ldi %0, 0x00       \n\t"
        // walk and chew gum at the same time
        "adc %0, r1          \n\t"
#else
        /* Multiply 8-bit i * 8-bit scale, giving 16-bit r1,r0 */
        "mul %0, %1    \n\t"
        /* Move the high 8-bits of the product (r1) back to i */
        "mov %0, r1    \n\t"
#endif
        /* R1 IS LEFT DIRTY HERE; YOU MUST ZERO IT OUT YOURSELF  */
        /* "clr __zero_reg__    \n\t" */
        : "+a" (i)      /* writes to i */
        : "a"  (scale)  /* uses scale */
        : "r0", "r1"    /* clobbers r0, r1 */
    );
    // Return the result
    return i;
#else
#error "No implementation for scale8_LEAVING_R1_DIRTY available."
#endif
}

/// ANSI unsigned short _Fract.  range is 0 to 0.99609375
///                 in steps of 0.00390625
// typedef uint8_t   fract8;   ///< ANSI: unsigned short _Fract

/// Clean up the r1 register after a series of *LEAVING_R1_DIRTY calls
LIB8STATIC_ALWAYS_INLINE void cleanup_R1()
{
#if CLEANUP_R1_AVRASM == 1
    // Restore r1 to "0"; it's expected to always be that
    asm volatile( "clr __zero_reg__  \n\t" : : : "r1" );
#endif
}

/// scale three one byte values by a fourth one, which is treated as
///         the numerator of a fraction whose demominator is 256
///         In other words, it computes r,g,b * (scale / 256)
///
///         THIS FUNCTION ALWAYS MODIFIES ITS ARGUMENTS IN PLACE

LIB8STATIC void nscale8x3( uint8_t& r, uint8_t& g, uint8_t& b, uint8_t scale)
{
#if SCALE8_C == 1
#if (FASTLED_SCALE8_FIXED == 1)
    uint16_t scale_fixed = scale + 1;
    r = (((uint16_t)r) * scale_fixed) >> 8;
    g = (((uint16_t)g) * scale_fixed) >> 8;
    b = (((uint16_t)b) * scale_fixed) >> 8;
#else
    r = ((int)r * (int)(scale) ) >> 8;
    g = ((int)g * (int)(scale) ) >> 8;
    b = ((int)b * (int)(scale) ) >> 8;
#endif
#elif SCALE8_AVRASM == 1
    r = scale8_LEAVING_R1_DIRTY(r, scale);
    g = scale8_LEAVING_R1_DIRTY(g, scale);
    b = scale8_LEAVING_R1_DIRTY(b, scale);
    cleanup_R1();
#else
#error "No implementation for nscale8x3 available."
#endif
}
// void fadeToBlackBy(int amnt){
//       for (int i=0; i < strip.numPixels()*3; i++) {

//         // random colored speckles that blink in and fade smoothly
//         fadeToBlackBy(leds, NUM_LEDS, 10);
//         int pos = random16(NUM_LEDS);
//         leds[pos] += CHSV(gHue + random8(64), 200, 255);
//         FastLED.show();
//       }
// }
// void confetti()
// {
//         for (int i=0; i < strip.numPixels()*3; i++) {

//         // random colored speckles that blink in and fade smoothly
//         fadeToBlackBy(10);
//         int pos = rand() % NUM_PIXELS;
//         pixelBuffer[pos] = CHSV(gHue + random8(64), 200, 255);
//         // FastLED.show();
//         strip.show()
//       }


// }


//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
// or as i like to call it - the RAINBOW VORTEX
void theaterChaseRainbow(uint8_t wait) {
  byte r, g, b;
  for (int j=0; j < 384; j++) {     // cycle all 384 colors in the wheel
    for (int q=0; q < 5; q++) {
      for (int i=0; i < strip.numPixels(); i=i+5) {
        strip.setPixelColor(i+q, Wheel( (i+(j*q)) % 384));    //turn every third pixel on
      }
      strip.show();
      
      delay(50);
      
      for (int i=0; i < strip.numPixels(); i=i+5) {
        uint32_t c = strip.getPixelColor(i+q);
        // Need to decompose color into its r, g, b elements
        b = (c >> 16) & 0x7f;
        r = (c >> 8) & 0x7f;
        g = c & 0x7f;
        if(i%5<1){
          strip.setPixelColor(i+q,  BlackColor);        //turn every third pixel off
        }else {
          strip.setPixelColor(i+q,  strip.Color((uint8_t)(r*0.25),(uint8_t)(b*0.25),(uint8_t)(g*0.25)));        //turn every third pixel off
        }
        // strip.setPixelColor(i+q,  Wheel( (i+j-(q)) % 384));        //turn every third pixel off
      }
    }
  }
}

// this needs review..
void theaterChaseRainbowBuffer(){
  for (int i=0; i < strip.numPixels(); i++) {
    if(i%4>0){
      pixelBuffer[i] = BlackColor;
    }else {
      pixelBuffer[i] = Wheel( i % 384);
    }
  }
}