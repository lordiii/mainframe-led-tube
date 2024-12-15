#ifndef PICTOGRAM_H
#define PICTOGRAM_H

#include "_effects.h"
#include "led.h"

class FX_Pictogram : FX {
public:
    FX_Pictogram() : FX("pictogram") {
    }

    bool render(unsigned long delta);

    void resetData();

    void onButton(GP_BUTTON button) {}

    void onAnalogButton(GP_BUTTON button, int value) {}

private:
    LED_Pixel *last;
};

#endif
