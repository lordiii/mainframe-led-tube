#ifndef PICTOGRAM_H
#define PICTOGRAM_H

#include "_effects.h"
#include "led.h"

class FX_Pictogram : FX {
public:
    FX_Pictogram() : FX("pictogram") {
    }

    bool render(unsigned long delta) override;

    void resetData() override;

    bool registerKeybindings() override { return false; };

private:
    LED_Pixel *last = nullptr;
};

#endif
