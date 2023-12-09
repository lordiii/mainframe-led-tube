#include "effects_lib.h"
#include "globals.h"
#include "effects.h"
#include <Arduino.h>

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

void setPixelColor(int ring, int pixel, int r, int g, int b)
{
    leds.setPixel(calculatePixelId(ring, pixel), r, g, b);
}

void setPixelColor(int ring, int pixel, int color)
{
    leds.setPixel(calculatePixelId(ring, pixel), color);
}

void setRingColor(int ring, int color)
{
    for (int i = 0; i < LED_PER_RING; i++)
    {
        setPixelColor(ring, i, color);
    }
}

void fadePixelToBlack(int ring, int pixel, uint8_t scale)
{
    int color = getPixelColor(ring, pixel);

    uint8_t r = color >> 16;
    uint8_t g = color >> 8;
    uint8_t b = color;

    r = (((uint16_t)r) * scale) >> 8;
    g = (((uint16_t)g) * scale) >> 8;
    b = (((uint16_t)b) * scale) >> 8;

    setPixelColor(ring, pixel, r, g, b);
}

void fadeRingToBlack(int ring, uint8_t scale)
{
    for(int i = 0; i < LED_PER_RING; i++)
    {
        fadePixelToBlack(ring, i, scale);
    }
}

void fadeAllToBlack(uint8_t scale)
{
    for(int i = 0 ; i < LED_TOTAL_RINGS; i++)
    {
        fadeRingToBlack(i, scale);
    }
}

int randomColor()
{
    uint8_t r = random(0, 256);
    uint8_t g = random(0, 256);
    uint8_t b = random(0, 256);

    int color = (r << 16) | (g << 8) | b;
    return color == 0 ? randomColor() : color;
}