#ifndef BEAM_H
#define BEAM_H

#include "_effects.h"
#include "led.h"
#include "enum.h"

class FX_Beam : FX {
public:
    FX_Beam() : FX("beam") {
    }

    bool render(unsigned long delta);

    void resetData();

    void onButton(GP_BUTTON button);

    void onAnalogButton(GP_BUTTON button, int value);

private:
    LED_Ring *last;
};

class FX_SideBeam : FX {
public:
    FX_SideBeam() : FX("side-beam") {
    }

    bool render(unsigned long delta);

    void resetData();

    void onButton(GP_BUTTON button);

    void onAnalogButton(GP_BUTTON button, int value);

private:
    LED_Pixel *last;

};

#endif
