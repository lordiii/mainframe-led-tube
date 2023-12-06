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

const int effectCount = 8;
Effect effects[effectCount] = {
    {"off", &effectOff},
    {"test-led", &effectTestLEDs},
    {"strobe", &effectStrobe},
    {"rainbow-strobe", &effectRainbowStrobe},
    {"police", &effectPolice},
    {"solid-white", &effectSolidWhite},
    {"beam", &effectBeam},
    {"gol", &effectGOL, &initializeGOLData}};

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

    memset(state->data, 0, sizeof(EffectData));

    applyBrightness(0);

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
    unsigned long delta = millis() - state->lastFrameChange;

    if ((!state->halt && state->slowRate == 0) || state->singleStep || (state->slowRate != 0 && delta > state->slowRate && !state->halt))
    {
        bool updated = false;

        if (state->current != nullptr)
        {
            updated = state->current->callback(delta);
        }

        if (updated)
        {
            state->lastFrameChange = millis();
        }

        state->singleStep = false;
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
    EffectTestAll *data = &state->data->testAll;

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
    EffectStrobe *data = &state->data->strobe;

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
    EffectRainbowStrobe *data = &state->data->rainbowStrobe;

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
    EffectPolice *data = &state->data->police;

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

bool effectBeam(unsigned long delta)
{
    EffectBeam *data = &state->data->beam;

    if (delta > 30)
    {
        for (int i = 1; i <= 12; i++)
        {
            fadeRingToBlack(data->lastRing - i, 0.9f);
        }

        setRingColor(data->lastRing, 0xFFFFFF);
        data->lastRing++;

        return true;
    }

    return false;
}

//
//
// Game of life
//
//
void copperhead(int centerRing, int centerPixel)
{
    EffectGOL *data = &state->data->gol;

    data->state[calculatePixelId(centerRing + 1, centerPixel)] = true;
    data->state[calculatePixelId(centerRing, centerPixel)] = true;
    data->state[calculatePixelId(centerRing - 1, centerPixel)] = true;
    data->state[calculatePixelId(centerRing - 2, centerPixel)] = true;

    data->state[calculatePixelId(centerRing + 1, centerPixel - 2)] = true;
    data->state[calculatePixelId(centerRing - 2, centerPixel - 2)] = true;

    data->state[calculatePixelId(centerRing + 2, centerPixel - 3)] = true;
    data->state[calculatePixelId(centerRing, centerPixel - 3)] = true;
    data->state[calculatePixelId(centerRing - 1, centerPixel - 3)] = true;
    data->state[calculatePixelId(centerRing - 3, centerPixel - 3)] = true;

    data->state[calculatePixelId(centerRing + 2, centerPixel - 4)] = true;
    data->state[calculatePixelId(centerRing - 3, centerPixel - 4)] = true;

    data->state[calculatePixelId(centerRing + 2, centerPixel - 6)] = true;
    data->state[calculatePixelId(centerRing - 3, centerPixel - 6)] = true;

    data->state[calculatePixelId(centerRing + 2, centerPixel - 7)] = true;
    data->state[calculatePixelId(centerRing + 1, centerPixel - 7)] = true;
    data->state[calculatePixelId(centerRing - 2, centerPixel - 7)] = true;
    data->state[calculatePixelId(centerRing - 3, centerPixel - 7)] = true;

    data->state[calculatePixelId(centerRing + 2, centerPixel - 8)] = true;
    data->state[calculatePixelId(centerRing + 1, centerPixel - 8)] = true;
    data->state[calculatePixelId(centerRing, centerPixel - 8)] = true;
    data->state[calculatePixelId(centerRing - 1, centerPixel - 8)] = true;
    data->state[calculatePixelId(centerRing - 2, centerPixel - 8)] = true;
    data->state[calculatePixelId(centerRing - 3, centerPixel - 8)] = true;

    data->state[calculatePixelId(centerRing + 1, centerPixel - 9)] = true;
    data->state[calculatePixelId(centerRing - 2, centerPixel - 9)] = true;

    data->state[calculatePixelId(centerRing, centerPixel - 10)] = true;
    data->state[calculatePixelId(centerRing - 1, centerPixel - 10)] = true;

    data->state[calculatePixelId(centerRing, centerPixel - 11)] = true;
    data->state[calculatePixelId(centerRing - 1, centerPixel - 11)] = true;
}

void initializeGOLData()
{
    EffectGOL *data = &state->data->gol;

    copperhead(30, 15);
    copperhead(10, 0);
    copperhead(20, 10);
    copperhead(50, 22);

    for (int i = 0; i < LED_TOTAL_RINGS; i++)
    {
        for (int j = 0; j < LED_PER_RING; j++)
        {
            setPixelColor(i, j, data->state[calculatePixelId(i, j)] ? 0xFFFFFF : 0x000000);
        }
    }
}

bool calculateGOLCell(int ring, int pixel)
{
    int cnt = 0;
    bool populated = getPixelColor(ring, pixel) > 0;

    // Top Left
    if (getPixelColor(ring + 1, pixel - 1) > 0)
    {
        cnt++;
    }
    // Top Center
    if (getPixelColor(ring + 1, pixel) > 0)
    {
        cnt++;
    }
    // Top Right
    if (getPixelColor(ring + 1, pixel + 1) > 0)
    {
         cnt++;
    }
    // Center Left
    if (getPixelColor(ring, pixel - 1) > 0)
    {
        cnt++;
    }
    // Center Right
    if (getPixelColor(ring, pixel + 1) > 0)
    {
        cnt++;
    }
    // Bottom Left
    if (getPixelColor(ring - 1, pixel - 1) > 0)
    {
        cnt++;
    }
    // Bottom Center
    if (getPixelColor(ring - 1, pixel) > 0)
    {
        cnt++;
    }
    // Bottom Right
    if (getPixelColor(ring - 1, pixel + 1) > 0)
    {
        cnt++;
    }

    return ((cnt == 2 && populated) || cnt == 3);
}

bool effectGOL(unsigned long delta)
{
    if (delta > 10)
    {
        EffectGOL *data = &state->data->gol;

        for (int i = 0; i < LED_TOTAL_RINGS; i++)
        {
            for (int j = 0; j < LED_PER_RING; j++)
            {
                data->state[calculatePixelId(i, j)] = calculateGOLCell(i, j);
            }
        }

        for (int i = 0; i < LED_TOTAL_RINGS; i++)
        {
            for (int j = 0; j < LED_PER_RING; j++)
            {
                setPixelColor(i, j, data->state[calculatePixelId(i, j)] ? 0xFFFFFF : 0x000000);
            }
        }

        return true;
    }

    return false;
}