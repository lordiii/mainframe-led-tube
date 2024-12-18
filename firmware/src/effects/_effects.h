#ifndef EFFECTS_H
#define EFFECTS_H

#include "display.h"
#include "enum.h"
#include "main.h"

class FX {
public:
    const char *name;

    DSP_Page displayPage = {};
    bool hasPage = false;

    bool usesButtons = false;

    virtual bool render(unsigned long delta) = 0;

    virtual void resetData() = 0;

    virtual bool registerKeybindings() = 0;

protected:
    explicit FX(const char *name) {
        this->name = name;
    }
};

typedef struct EffectState {
    FX *current;
    unsigned int lastFrameChange;
    unsigned int slowRate;
    bool halt;
    bool singleStep;
} EffectState;

void FX_init();

void FX_resetState();

void FX_stopEffect(DSP_Btn *btn);

bool FX_setEffect(const char *effectName);

EffectState *FX_getState();

FX **FX_getEffects();

int FX_getCount();

#endif
