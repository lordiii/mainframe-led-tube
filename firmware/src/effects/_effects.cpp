#include "_effects.h"
#include "beam.h"
#include "display.h"
#include "led.h"
#include "pictogram.h"
#include "tetris.h"

#include <Arduino.h>

FX_Beam *beam = new FX_Beam();

FX_SideBeam *sideBeam = new FX_SideBeam();

FX_Pictogram *pictogram = new FX_Pictogram();

const int fxCnt = 4;
FX *effects[fxCnt] = {(FX *) beam, (FX *) sideBeam, nullptr, (FX *) pictogram};

EffectState state;

DSP_Element buttonStopEffect = {
        (DSP_Element_Data) {
                .btn = {true, {"Stop Effect", DSP_WHITE}, &FX_stopEffect}},
        BTN};
DSP_Page defaultEffectPage = {{"", DSP_WHITE}, &buttonStopEffect, 1};

void FX_init() {
    effects[2] = (FX *) TETRIS_getInstance();
}

EffectState *FX_getState() {
    return &state;
}

FX **FX_getEffects() {
    return effects;
}

int FX_getCount() {
    return fxCnt;
}

void FX_stopEffect(DSP_Btn *btn) {
    FX_resetState();
    DSP_onEffectMenuClick(&buttonStopEffect.data.btn);
}

void FX_resetState() {
    state.lastFrameChange = 0;
    state.current = nullptr;
    state.halt = false;
    state.singleStep = false;
    state.slowRate = 0;
    LED_clear();

    GP_clearKeybindings();
    DSP_addKeybindings();
}

void FX_toggleHalt(GP_BUTTON btn, GP_Status *gp) {
    state.halt = !state.halt;

    if (state.halt) {
        GP_clearKeybindings();
        GP_registerKeybind(MISC_HOME, FX_toggleHalt);
        DSP_addKeybindings();
    } else {
        DSP_removeKeybindings();
        if (state.current != nullptr) {
            state.current->registerKeybindings();
        }
    }
}

bool FX_setEffect(const char *effectName) {
    FX_resetState();

    FX *effect = nullptr;
    for (int i = 0; i < fxCnt; i++) {
        if (strcmp(effects[i]->name, effectName) == 0) {
            effect = effects[i];
            break;
        }
    }

    // Reset Data
    if (effect != nullptr) {
        effect->resetData();
        state.current = effect;

        if (effect->hasPage) {
            DSP_renderPage(&effect->displayPage);
        } else {
            defaultEffectPage.title.text = effect->name;
            DSP_renderPage(&defaultEffectPage);
        }

        if (effect->registerKeybindings()) {
            DSP_removeKeybindings();
            GP_registerKeybind(MISC_HOME, FX_toggleHalt);
        }

        return true;
    }

    return false;
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
    EffectHelix* data = &state.data->helix;

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
    EffectHelix* data = &state.data->helix;

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
    EffectGOL* data = &state.data->gol;

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
