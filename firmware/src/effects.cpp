#include "effects.h"
#include "globals.h"

#include <OctoWS2811.h>

// Setup LEDs
const byte pinList[LED_STRIP_AMOUNT] = LED_PINS;

DMAMEM int displayMemory[LED_PER_STRIP * 6];
int drawingMemory[LED_PER_STRIP * 6];

const int config = LED_CONFIGURATION;
OctoWS2811 leds(LED_PER_STRIP, displayMemory, drawingMemory, config, LED_STRIP_AMOUNT, pinList);

void initLeds()
{
    leds.begin();
    leds.show();
}

// OctoWS2811 settings
#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF

float brightness = 75.0f;
void setBrightness(float value) {
    brightness = value;
}

bool (*currentEffect)(OctoWS2811, unsigned long) = nullptr;
unsigned long lastFrameChange = 0;

void renderFrame()
{
    bool updated = false;

    if(currentEffect != nullptr) {
        unsigned long delta = millis() - lastFrameChange;
        updated = currentEffect(leds, delta);
    }

    if (updated)
    {
        lastFrameChange = millis();
    }

    leds.show();
}

void setCurrentEffect(bool (*function)(OctoWS2811,unsigned long))
{
    noInterrupts();
    lastFrameChange = 0;
    currentEffect = function;
    interrupts();
}

int applyBrightness(int color)
{
    uint8_t r = color >> 16;
    uint8_t g = color >> 8;
    uint8_t b = color;

    r *= brightness;
    g *= brightness;
    b *= brightness;

    return (r << 16) | (g << 8) | b;
}

// Check if we got all universes
void fillLEDs(OctoWS2811 leds, int color)
{
    for (int i = 0; i < leds.numPixels(); i++)
    {
        leds.setPixel(i, applyBrightness(color));
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
    if (delta > 1000)
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

bool effectSolidWhite(OctoWS2811 leds, unsigned long delta)
{
    if (delta > 10000)
    {
        fillLEDs(leds, 0xFFFFFF);

        return true;
    }

    return false;
}
