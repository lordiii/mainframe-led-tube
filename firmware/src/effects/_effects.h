#ifndef EFFECTS_H
#define EFFECTS_H

#include "main.h"
#include "enum.h"

class FX {
public:
    const char *name;

    virtual bool render(unsigned long delta) = 0;

    virtual void resetData() = 0;

    virtual void onButton(GP_BUTTON button) = 0;

    virtual void onAnalogButton(GP_BUTTON button, int value) = 0;

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

bool FX_setEffect(const char *effectName);

EffectState *FX_getState();

#endif
