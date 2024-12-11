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
    Effect *current;
    EffectData data;
    unsigned int lastFrameChange;
    unsigned int slowRate;
    bool halt;
    bool singleStep;
} EffectState;

bool FX_setEffect(const char *effectName);

EffectState *FX_getState();

#endif
