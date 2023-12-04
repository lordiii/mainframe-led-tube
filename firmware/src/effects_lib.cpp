#include "effects.h"

int applyBrightness(int color)
{
    uint8_t r = color >> 16;
    uint8_t g = color >> 8;
    uint8_t b = color;

    r *= state->brightness;
    g *= state->brightness;
    b *= state->brightness;

    return (r << 16) | (g << 8) | b;
}

// Check if we got all universes
void fillLEDs(int color)
{
    for (int i = 0; i < leds.numPixels(); i++)
    {
        leds.setPixel(i, applyBrightness(color));
    }
}