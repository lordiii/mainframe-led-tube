#include "_effects.h"
#include "led.h"
#include "beam.h"

#include <Arduino.h>

EffectState *state = new EffectState;

const int fxCnt = 2;
Effect *effects = new Effect[fxCnt]{
        {"beam",      &FX_beam,     &FX_resetBeam},
        {"side-beam", &FX_sideBeam, &FX_resetSideBeam},
        //{"helix", &effectHelix},
        //{"gol", &effectGOL, &initializeGOLData},
        //{"tetris", &renderTetrisFrame, &initializeTetris, &onTetrisButtonPress, &onTetrisAnalogButton}
};

EffectState *FX_getState() {
    return state;
}

bool FX_setEffect(const char *effectName) {
    state->current = nullptr;

    // Reset State
    state->lastFrameChange = 0;
    memset(state->data, 0, sizeof(EffectData));
    LED_clear();

    Effect *effect = nullptr;
    for (int i = 0; i < fxCnt; i++) {
        effect = &effects[i];

        if (strcmp(effect->name, effectName) == 0) {
            break;
        } else {
            effect = nullptr;
        }
    }

    // Reset Data
    if (effect != nullptr) {
        if (effect->resetData != nullptr) {
            effect->resetData(state->data);
        }

        state->current = effect;
        return true;
    } else {
        return false;
    }
}


/*double calculateHelixPosition(double x, double pos, double count)
{
    const static double centerOffset = ((((double)LED_TOTAL_RINGS) / 2.0) - 0.5);

    double y = x + (pos / count) * ((double)LED_PER_RING);
    y = y / (((double)LED_PER_RING) / 2.0);
    y = y * PI;
    y = sin(y);
    y = (y * centerOffset) + centerOffset;

    return y;
}

bool effectFilledHelix(unsigned long delta)
{
    EffectHelix* data = &state->data->helix;

    if (delta > 25)
    {
        clearLEDs();

        for (double i = 0; i < (LED_PER_RING * 10); i += 0.1)
        {
            double y1 = calculateHelixPosition(i, 0, 6);
            double y2 = calculateHelixPosition(i, 1, 6);
            double y3 = calculateHelixPosition(i, 2, 6);
            double y4 = calculateHelixPosition(i, 3, 6);
            double y5 = calculateHelixPosition(i, 4, 6);
            double y6 = calculateHelixPosition(i, 5, 6);

            setPixelColor((int)y1, data->pixel + ((int)i), applyBrightness(0x0000FF));
            setPixelColor((int)y2, data->pixel + ((int)i), applyBrightness(0xFF0000));
            setPixelColor((int)y3, data->pixel + ((int)i), applyBrightness(0x00FF00));
            setPixelColor((int)y4, data->pixel + ((int)i), applyBrightness(0xFF00FF));
            setPixelColor((int)y5, data->pixel + ((int)i), applyBrightness(0x00FFFF));
            setPixelColor((int)y6, data->pixel + ((int)i), applyBrightness(0xFFFF00));
        }

        data->pixel++;
        data->pixel = data->pixel % LED_PER_RING;

        return true;
    }

    return false;
}

bool effectHelix(unsigned long delta)
{
    EffectHelix* data = &state->data->helix;

    if (delta > 25)
    {
        clearLEDs();

        for (int i = 0; i < LED_PER_RING; i++)
        {
            double y1 = calculateHelixPosition(i, 0, 6);
            double y2 = calculateHelixPosition(i, 1, 6);
            double y3 = calculateHelixPosition(i, 2, 6);
            double y4 = calculateHelixPosition(i, 3, 6);
            double y5 = calculateHelixPosition(i, 4, 6);
            double y6 = calculateHelixPosition(i, 5, 6);

            setPixelColor(floor(y1), data->pixel + i, applyBrightness(0x0000FF));
            setPixelColor(floor(y2), data->pixel + i, applyBrightness(0xFF0000));
            setPixelColor(floor(y3), data->pixel + i, applyBrightness(0x00FF00));
            setPixelColor(floor(y4), data->pixel + i, applyBrightness(0xFF00FF));
            setPixelColor(floor(y5), data->pixel + i, applyBrightness(0x00FFFF));
            setPixelColor(floor(y6), data->pixel + i, applyBrightness(0xFFFF00));
        }

        data->pixel++;
        data->pixel = data->pixel % LED_PER_RING;

        return true;
    }

    return false;
}

//
//
// Games
//
//
bool effectGOL(unsigned long delta)
{
    EffectGOL* data = &state->data->gol;

    if (delta > 10)
    {
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
*/