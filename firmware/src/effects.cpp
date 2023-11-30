#include <OctoWS2811.h>

#include "effects.h"

// OctoWS2811 settings
#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF

// Check if we got all universes
void fillLEDs(OctoWS2811 leds, unsigned int color)
{
    for (int i = 0; i < leds.numPixels(); i++)
    {
        leds.setPixel(i, color);
    }
}

bool wasOn = false;
bool effectStrobe(OctoWS2811 leds, unsigned long delta)
{
    if (delta > 5 && !wasOn)
    {
        fillLEDs(leds, 0xFFFFFF);
        wasOn = true;

        return true;
    }

    if (delta > 50 && wasOn)
    {
        fillLEDs(leds, 0x000000);
        wasOn = false;

        return true;
    }

    return false;
}

int lastColor = BLUE;
bool effectRainbowStrobe(OctoWS2811 leds, unsigned long delta)
{
    if (delta > 5 && !wasOn)
    {
        wasOn = true;

        switch (lastColor)
        {
        case RED:
            lastColor = GREEN;
            break;
        case GREEN:
            lastColor = BLUE;
            break;
        case BLUE:
        default:
            lastColor = RED;
            break;
        }

        fillLEDs(leds, lastColor);

        return true;
    }

    if (delta > 25 && wasOn)
    {
        fillLEDs(leds, 0x000000);
        wasOn = false;

        return true;
    }

    return false;
}

bool effectTestLEDs(OctoWS2811 leds, unsigned long delta)
{
    if (delta > 500)
    {
        switch (lastColor)
        {
        case RED:
            lastColor = GREEN;
            break;
        case GREEN:
            lastColor = BLUE;
            break;
        case BLUE:
        default:
            lastColor = RED;
            break;
        }

        fillLEDs(leds, lastColor);

        return true;
    }

    return false;
}

bool effectOff(OctoWS2811 leds, unsigned long delta)
{
    if (delta > 10000)
    {
        fillLEDs(leds, 0x000000);

        return true;
    }

    return false;
}

int blinkTimes = 0;
bool effectPolice(OctoWS2811 leds, unsigned long delta)
{
    if (delta > 25 && blinkTimes >= 2)
    {
        if (lastColor == BLUE)
        {
            lastColor = RED;
        }
        else
        {
            lastColor = BLUE;
        }

        fillLEDs(leds, 0x000000);

        blinkTimes = 0;
        return true;
    }

    if (delta > 50 && blinkTimes < 2)
    {
        if (wasOn)
        {
            wasOn = false;
            blinkTimes++;
            fillLEDs(leds, 0x000000);
        }
        else
        {
            wasOn = true;
            fillLEDs(leds, lastColor);
        }

        return true;
    }

    return false;
}