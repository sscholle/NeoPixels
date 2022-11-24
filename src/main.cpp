
#include "LPD8806.h"
#include "SPI.h"

// Example to control LPD8806-based RGB LED Modules in a strip!
// NOTE: WILL NOT WORK ON TRINKET OR GEMMA due to floating-point math
/*****************************************************************************/

const int NUM_PIXELS = 240;
// Set the first variable to the NUMBER of pixels. 32 = 32 pixels in a row
// The LED strips are 32 LEDs per meter but you can extend/cut the strip
LPD8806 strip = LPD8806(NUM_PIXELS);

uint32_t pixelBuffer[NUM_PIXELS];

void setup()
{
  // Serial.begin(9600);
  // Start up the LED strip
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();
}

// function prototypes, do not remove these!
void colorChase(uint32_t c, uint8_t wait);
void colorWipe(uint32_t c, uint8_t wait);
void colorEffects(uint32_t c);
void ditherRevealBuffer(uint8_t wait);
void dither(uint32_t c, uint8_t wait);
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
  int effect = rand() % 3;
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

// void colorEffects(uint32_t color)
// {
//   byte r = getRGB(color, 'r');
//   byte g = getRGB(color, 'g');
//   byte b = getRGB(color, 'b');
//   scanner(r, b, g, delayMsec);
//   colorWipe(color, delayMsec);
//   colorWipe(BlackColor, delayMsec);
//   dither(color, delayMsec);
//   waveReveal(color, 5, delayMsec);
//   wave(color, 2, 5, delayMsec);
//   dither(BlackColor, delayMsec);
//   // colorWipe(BlackColor, delayMsec);
//   rainbowWipe(delayMsec);
//   rainbowCycle(5, delayMsec);
//   colorWipe(BlackColor, delayMsec);
// }

void colorScanner(uint32_t color, uint8_t wait)
{
  byte r = getRGB(color, 'r');
  byte g = getRGB(color, 'g');
  byte b = getRGB(color, 'b');
  scanner(r, b, g, wait);
}

// void basicEffects()
// {
//   scanner(0, 127, 0, delayMsec); // blue, fast
//   colorWipe(strip.Color(0, 127, 0), delayMsec); // blue
//   colorWipe(strip.Color(0, 0, 0), delayMsec); // blue
//   rainbowWipe(delayMsec);
//   rainbowCycle(5, delayMsec);                 // make it go through the cycle fairly fast
//   colorWipe(strip.Color(0, 0, 0), delayMsec); // black
// }

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
void colorChase(uint32_t c, uint8_t wait)
{
  int i;

  for (i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, 0); // turn all pixels off
  }

  for (i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, c); // set one pixel
    strip.show();              // refresh strip display
    delay(wait);               // hold image for a moment
    strip.setPixelColor(i, 0); // erase pixel (but don't refresh yet)
  }
  strip.show(); // for last erased pixel
}

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
void dither(uint32_t c, uint8_t wait)
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
    strip.setPixelColor(reverse, c);
    strip.show();
    delay(wait);
  }
  delay(250); // Hold image for 1/4 sec
}

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