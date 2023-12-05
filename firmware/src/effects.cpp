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

const int effectCount = 7;
Effect effects[effectCount] = {
    {"off", &effectOff},
    {"test-led", &effectTestLEDs},
    {"strobe", &effectStrobe},
    {"rainbow-strobe", &effectRainbowStrobe},
    {"police", &effectPolice},
    {"solid-white", &effectSolidWhite},
    {"beam", &effectBeam}
};

void initOctoWS2811()
{
    leds.begin();
    leds.show();

    state->lastFrameChange = 0;
    state->brightness = 0.75f;
}

void setBrightness(float value)
{
    state->brightness = value;

    for (int i = 0; i < leds.numPixels(); i++)
    {
        leds.setPixel(i, applyBrightness(leds.getPixel(i)));
    }
}

void setCurrentEffect(Effect *effect)
{
    noInterrupts();
    state->lastFrameChange = 0;
    state->current = effect;

    if (state->current != nullptr)
    {
        state->current->resetData();
    }

    if (state == nullptr)
    {
        fillLEDs(0x000000);
    }
    interrupts();
}

void renderFrame()
{
    bool updated = false;

    if (state->current != nullptr)
    {
        unsigned long delta = millis() - state->lastFrameChange;
        updated = state->current->callback(delta);
    }

    if (updated)
    {
        state->lastFrameChange = millis();
    }

    leds.show();
}

//
//
// LED Test Effects
//
//
bool effectTestLEDs(unsigned long delta)
{
    EffectTestAll *data = &(state->data->testAll);

    if (delta > 250)
    {
        switch (data->lastColor)
        {
        case RED:
            data->lastColor = GREEN;
            break;
        case GREEN:
            data->lastColor = BLUE;
            break;
        case BLUE:
        default:
            data->lastColor = RED;
            break;
        }

        fillLEDs(data->lastColor);

        return true;
    }

    return false;
}

//
//
// Strobe Effects
//
//
bool effectStrobe(unsigned long delta)
{
    EffectStrobe *data = &(state->data->strobe);

    if (delta > 5 && !data->toggle)
    {
        fillLEDs(0xFFFFFF);
        data->toggle = true;

        return true;
    }

    if (delta > 50 && data->toggle)
    {
        fillLEDs(0x000000);
        data->toggle = false;

        return true;
    }

    return false;
}

bool effectRainbowStrobe(unsigned long delta)
{
    EffectRainbowStrobe *data = &(state->data->rainbowStrobe);

    if (delta > 5 && !data->toggle)
    {
        data->toggle = true;

        switch (data->lastColor)
        {
        case RED:
            data->lastColor = GREEN;
            break;
        case GREEN:
            data->lastColor = BLUE;
            break;
        case BLUE:
        default:
            data->lastColor = RED;
            break;
        }

        fillLEDs(data->lastColor);

        return true;
    }

    if (delta > 25 && data->toggle)
    {
        fillLEDs(0x000000);
        data->toggle = false;

        return true;
    }

    return false;
}

bool effectPolice(unsigned long delta)
{
    EffectPolice *data = &(state->data->police);

    if (delta > 25 && data->blinkTimes >= 2)
    {
        if (data->lastColor == BLUE)
        {
            data->lastColor = RED;
        }
        else
        {
            data->lastColor = BLUE;
        }

        fillLEDs(0x000000);

        data->blinkTimes = 0;
        return true;
    }

    if (delta > 50 && data->blinkTimes < 2)
    {
        if (data->toggle)
        {
            data->toggle = false;
            data->blinkTimes++;
            fillLEDs(0x000000);
        }
        else
        {
            data->toggle = true;
            fillLEDs(data->lastColor);
        }

        return true;
    }

    return false;
}

//
//
// Miscellanous
//
//
bool effectSolidWhite(unsigned long delta)
{
    fillLEDs(0xFFFFFF);
    return true;
}

bool effectOff(unsigned long delta)
{
    fillLEDs(0);
    return true;
}

unsigned int lastRing = 0;
bool effectBeam(unsigned long delta)
{
    if (delta > 30)
    {
        setRingColor(lastRing, 0xFFFFFF);
        lastRing++;

        return true;
    }

    return false;
}