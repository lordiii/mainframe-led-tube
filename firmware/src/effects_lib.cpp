#include "effects_lib.h"
#include "globals.h"
#include "effects.h"

int applyBrightness(int color)
{
    return fadePixelByAmount(color, state->brightness);
}

void fillLEDs(int color)
{
    for(int i = 0 ; i < LED_TOTAL_RINGS; i++)
    {
        for(int j = 0; j < LED_PER_RING; j++)
        {
            setPixelColor(i, j, color);
        }
    }
}

int fadePixelByAmount(int color, float amount)
{
    uint8_t r = color >> 16;
    uint8_t g = color >> 8;
    uint8_t b = color;

    r *= amount;
    g *= amount;
    b *= amount;

    return (r << 16) | (g << 8) | b;
}

int getPixelColor(int ring, int pixel)
{
    return leds.getPixel(calculatePixelId(ring, pixel));
}

int calculatePixelId(int ring, int pixel)
{
    ring = abs(ring);
    pixel = (pixel > 0) ? pixel : LED_PER_RING + pixel;

    pixel = pixel % LED_PER_RING;
    ring = ring % LED_TOTAL_RINGS;

    return (ring * LED_PER_RING) + pixel;
}

void setPixelColor(int ring, int pixel, int color)
{
    leds.setPixel(calculatePixelId(ring, pixel), applyBrightness(color));
}

void setRingColor(int ring, int color)
{
    for (int i = 0; i < LED_PER_RING; i++)
    {
        setPixelColor(ring, i, color);
    }
}

void fadePixelToBlack(int ring, int pixel, float strength)
{
    int color = leds.getPixel(calculatePixelId(ring, pixel));

    setPixelColor(ring, pixel, fadePixelByAmount(color, strength));
}

void fadeRingToBlack(int ring, float strength)
{
    for(int i = 0; i < LED_PER_RING; i++)
    {
        fadePixelToBlack(ring, i, strength);
    }
}

void fadeAllToBlack()
{
    for(int i = 0 ; i < LED_TOTAL_RINGS; i++)
    {
        fadeRingToBlack(i, 0.9f);
    }
}