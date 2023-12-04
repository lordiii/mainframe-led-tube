#include "effects.h"
#include "globals.h"
#include "effects_lib.h"

#include <OctoWS2811.h>

#define LED_STRIP_AMOUNT (LED_TOTAL_RINGS / LED_RINGS_PER_SEGMENT)
#define LED_PER_STRIP (LED_PER_RING * LED_RINGS_PER_SEGMENT)

// Setup LEDs
const byte pinList[LED_STRIP_AMOUNT] = LED_PINS;
DMAMEM int displayMemory[LED_PER_STRIP * 6];
int drawingMemory[LED_PER_STRIP * 6];

OctoWS2811 leds = OctoWS2811(LED_PER_STRIP, displayMemory, drawingMemory, LED_CONFIGURATION, LED_STRIP_AMOUNT, pinList);
EffectState *state = new EffectState;

int effectCount = 0;
Effect **effects = (Effect **)malloc(256);

void initOctoWS2811() {
    leds.begin();
    leds.show();

    state->current = nullptr;
    state->effectData = nullptr;
    state->lastFrameChange = 0;
    state->brightness = 0.75f;
}

void registerEffect(String name, EffectCallback callback)
{
    Effect *effect = new Effect;
    effect->callback = callback;
    effect->name = name;

    effects[effectCount] = effect;
    effectCount++;
}

void setBrightness(float value) {
    state->brightness = value;

    for(int i = 0; i < leds.numPixels(); i++)
    {
        leds.setPixel(i, applyBrightness(leds.getPixel(i)));
    }
}

void setCurrentEffect(Effect *effect)
{
    noInterrupts();
    state->lastFrameChange = 0;
    state->current = effect;

    if(state == nullptr)
    {
        fillLEDs(0x000000);
    }
    interrupts();
}

void renderFrame()
{
    bool updated = false;

    if(state->current != nullptr) {
        unsigned long delta = millis() - state->lastFrameChange;
        updated = state->current->callback(delta);
    }

    if (updated)
    {
        state->lastFrameChange = millis();
    }

    leds.show();
}

bool wasOn = false;
bool effectStrobe(unsigned long delta)
{
    if (delta > 5 && !wasOn)
    {
        fillLEDs(0xFFFFFF);
        wasOn = true;

        return true;
    }

    if (delta > 50 && wasOn)
    {
        fillLEDs(0x000000);
        wasOn = false;

        return true;
    }

    return false;
}

int lastColor = BLUE;
bool effectRainbowStrobe(unsigned long delta)
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

        fillLEDs(lastColor);

        return true;
    }

    if (delta > 25 && wasOn)
    {
        fillLEDs(0x000000);
        wasOn = false;

        return true;
    }

    return false;
}

bool effectTestLEDs(unsigned long delta)
{
    if (delta > 250)
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

        fillLEDs(lastColor);

        return true;
    }

    return false;
}

int blinkTimes = 0;
bool effectPolice(unsigned long delta)
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

        fillLEDs(0x000000);

        blinkTimes = 0;
        return true;
    }

    if (delta > 50 && blinkTimes < 2)
    {
        if (wasOn)
        {
            wasOn = false;
            blinkTimes++;
            fillLEDs(0x000000);
        }
        else
        {
            wasOn = true;
            fillLEDs(lastColor);
        }

        return true;
    }

    return false;
}

bool effectSolidWhite(unsigned long delta)
{
    if (delta > 10000)
    {
        fillLEDs(0xFFFFFF);

        return true;
    }

    return false;
}

bool effectBeam(unsigned long delta)
{
    if(delta > 30)
    {
        fadeAllToBlack(50);
        return true;
    }

    return false;
}