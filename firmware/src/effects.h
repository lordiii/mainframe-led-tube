#ifndef EFFECTS_H
#define EFFECTS_H

#include "globals.h"
#include "led.h"

#include <OctoWS2811.h>

typedef bool (*EffectCallback)(unsigned long);

enum Button {
    BUTTON_Y,
    BUTTON_B,
    BUTTON_A,
    BUTTON_X,
    BUTTON_TL,
    BUTTON_TR,
    DPAD_LEFT,
    DPAD_RIGHT,
    DPAD_UP,
    DPAD_DOWN,
    SHOULDER_L1,
    SHOULDER_L2,
    SHOULDER_R1,
    SHOULDER_R2,
    MISC_HOME,
    MISC_START,
    MISC_SELECT,
    MISC_SYSTEM,
    MISC_BACK,
    MISC_CAPTURE,
    THROTTLE,
    BREAK,
    STICK_L_X,
    STICK_L_Y,
    STICK_R_X,
    STICK_R_Y
};

struct Effect {
    const char *name = "";
    EffectCallback callback = nullptr;

    void (*resetData)() = []() {
    };

    void (*onButtonPress)(Button button) = [](Button button) {
    };

    void (*onAnalogButton)(Button button, int value) = [](Button button, int value) {
    };
};

struct EffectBeam {
    LED_Ring *last;
};

struct EffectSideBeam {
    LED_Pixel *last;
};

/*
struct EffectHelix {
    int pixel;
};
*/

extern Effect *effects;
extern const int effectCount;

void setCurrentEffect(Effect *effect);

bool effectBeam(unsigned long delta);

bool effectSideBeam(unsigned long delta);

//bool effectHelix(unsigned long delta);

//bool effectFilledHelix(unsigned long delta);

#include "gol.h"
#include "tetris.h"

// Data Union
union EffectData {
    EffectBeam beam;
    EffectSideBeam sideBeam;
};

struct EffectState {
    Effect *current = nullptr;
    EffectData *data = new EffectData;
    unsigned int lastFrameChange = 0;
    unsigned int slowRate = 0;
    float brightness = 1.0f;
    bool halt = false;
    bool singleStep = false;
};

extern EffectState *state;

#endif
