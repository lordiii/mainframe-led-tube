#include "effects.h"
#include "globals.h"
#include "effects_lib.h"

#include <OctoWS2811.h>

#include <utility>

// Setup LEDs
const byte pinList[LED_STRIP_AMOUNT] = LED_PINS;
DMAMEM int displayMemory[LED_PER_STRIP * 6];
int drawingMemory[LED_PER_STRIP * 6];

void initOctoWS2811() {
    leds = OctoWS2811(LED_PER_STRIP, displayMemory, drawingMemory, LED_CONFIGURATION, LED_STRIP_AMOUNT, pinList);
    leds.begin();
    leds.show();

    state = (EffectState *) malloc(sizeof(EffectState));

    state->current = nullptr;
    state->effectData = nullptr;
    state->lastFrameChange = 0;
    state->brightness = 0.75f;

    effects = (EffectList *) malloc(sizeof(EffectList));
    effects->length = 0;
    effects->first = nullptr;
    effects->last = nullptr;
}

void registerEffect(String name, EffectCallback callback)
{
    Effect *effect = (Effect*) malloc(sizeof(Effect));

    effect->name = std::move(name);
    effect->callback = callback;
    effect->previous = effects->last;
    effect->next = nullptr;

    if(effects->length == 0)
    {
        effects->first = effect;
        effects->last = effect;
    } else
    {
        effects->last->next = effect;
        effects->last = effect;
    }

    effects->length++;
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
