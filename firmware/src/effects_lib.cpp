#include "effects_lib.h"
#include "globals.h"
#include "effects.h"

int applyBrightness(int color)
{
    return applyBrightnessByAmount(color, state->brightness);
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

int applyBrightnessByAmount(int color, float amount)
{
    uint8_t r = color >> 16;
    uint8_t g = color >> 8;
    uint8_t b = color;

    r *= amount;
    g *= amount;
    b *= amount;

    return (r << 16) | (g << 8) | b;
}

unsigned int calculatePixelId(unsigned int ring, unsigned int pixel)
{
    pixel = pixel % LED_PER_RING;
    ring = ring % LED_TOTAL_RINGS;

    return (ring * LED_PER_RING) + pixel;
}

void setPixelColor(unsigned int ring, unsigned int pixel, int color)
{
    leds.setPixel(calculatePixelId(ring, pixel), applyBrightness(color));
}

void fadePixelToBlack(unsigned int ring, unsigned int pixel, unsigned char strength)
{
    setPixelColor(ring, pixel, applyBrightnessByAmount(calculatePixelId(ring, pixel), 1.0f - (strength / 100)));
}

void fadeRingToBlack(unsigned int ring, unsigned char strength)
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
        fadeRingToBlack(i, strength);
    }
}

void setRingColor(unsigned int ring, int color)
{
    for (int i = 0; i < LED_PER_RING; i++)
    {
        setPixelColor(ring, i, color);
    }
}