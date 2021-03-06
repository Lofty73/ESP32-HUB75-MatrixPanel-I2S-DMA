// How to use this library with a FM6126 panel, thanks goes to:
// https://github.com/hzeller/rpi-rgb-led-matrix/issues/746

#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

MatrixPanel_I2S_DMA dma_display;

#include <FastLED.h>

////////////////////////////////////////////////////////////////////
// Reset Panel
// FM6126 support is still experimental
//
// pinout for ESP38 38pin module
// http://arduinoinfo.mywikis.net/wiki/Esp32#KS0413_keyestudio_ESP32_Core_Board
//

// HUB75E pinout
// R1 | G1
// B1 | GND
// R2 | G2
// B2 | E
//  A | B
//  C | D
// CLK| LAT
// OE | GND

#define R1 25
#define G1 26
#define BL1 27
#define R2 14 // 21 SDA
#define G2 12 // 22 SDL
#define BL2 13
#define CH_A 23
#define CH_B 19
#define CH_C 5
#define CH_D 17
#define CH_E -1 // assign to any available pin if using two panels or 64x64 panels with 1/32 scan (i.e. 32 works fine)
#define CLK 16
#define LAT 4
#define OE 15

// End of default setup for RGB Matrix 64x32 panel
///////////////////////////////////////////////////////////////

int time_counter = 0;
int cycles = 0;

CRGB currentColor;
CRGBPalette16 palettes[] = {HeatColors_p, LavaColors_p, RainbowColors_p, RainbowStripeColors_p, CloudColors_p};
CRGBPalette16 currentPalette = palettes[0];


CRGB ColorFromCurrentPalette(uint8_t index = 0, uint8_t brightness = 255, TBlendType blendType = LINEARBLEND) {
  return ColorFromPalette(currentPalette, index, brightness, blendType);
}

void setup(){

    // If you experience ghosting, you will need to reduce the brightness level, not all RGB Matrix
    // Panels are the same - some seem to display ghosting artefacts at lower brightness levels.
    // In the setup() function do something like:

    dma_display.setPanelBrightness(30); // SETS THE BRIGHTNESS HERE. 60 OR LOWER IDEAL.

    /**
     * be sure to specify 'FM6126A' as last parametr to the begin(),
     * it would reset 6126 registers and enables the matrix
     */
    dma_display.begin(R1, G1, BL1, R2, G2, BL2, CH_A, CH_B, CH_C, CH_D, CH_E, LAT, OE, CLK, FM6126A);
}

void loop(){
   for (int x = 0; x <  dma_display.width(); x++) {
        for (int y = 0; y <  dma_display.height(); y++) {
            int16_t v = 0;
            uint8_t wibble = sin8(time_counter);
            v += sin16(x * wibble * 3 + time_counter);
            v += cos16(y * (128 - wibble)  + time_counter);
            v += sin16(y * x * cos8(-time_counter) / 8);

            currentColor = ColorFromPalette(currentPalette, (v >> 8) + 127); //, brightness, currentBlendType);
            dma_display.drawPixelRGB888(x, y, currentColor.r, currentColor.g, currentColor.b);
        }
    }

    time_counter += 1;
    cycles++;

    if (cycles >= 1024) {
        time_counter = 0;
        cycles = 0;
        currentPalette = palettes[random(0,sizeof(palettes)/sizeof(palettes[0]))];
    }
}