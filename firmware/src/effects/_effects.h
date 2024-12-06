#ifndef EFFECTS_H
#define EFFECTS_H

#include "beam.h"
#include "main.h"

/*
struct EffectHelix {
    int pixel;
};
*/

//bool effectHelix(unsigned long delta);

//bool effectFilledHelix(unsigned long delta);


typedef union EffectData {
    FX_Beam beam;
    FX_SideBeam sideBeam;
} EffectData;

typedef struct Effect {
    const char *name;

    bool (*callback)(EffectData *, unsigned long);

    void (*resetData)(EffectData *);

    void (*onButtonPress)(Button button);

    void (*onAnalogButton)(Button button, int value);
} Effect;

typedef struct EffectState {
    Effect *current = nullptr;
    EffectData *data = new EffectData;
    unsigned int lastFrameChange = 0;
    unsigned int slowRate = 0;
    bool halt = false;
    bool singleStep = false;
} EffectState;

bool FX_setEffect(const char *effectName);

EffectState *FX_getState();

#endif
