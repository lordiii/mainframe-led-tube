#ifndef BEAM_H
#define BEAM_H

#include "_effects.h"
#include "led.h"
#include "enum.h"

class FX_Beam : FX {
public:
    FX_Beam() : FX("beam") {
    }

    bool render(unsigned long delta) override;

    void resetData() override;

    bool registerKeybindings() override { return false; };

private:
    LED_Ring *last = nullptr;
};

class FX_SideBeam : FX {
public:
    FX_SideBeam() : FX("side-beam") {
    }

    bool render(unsigned long delta) override;

    void resetData() override;

    bool registerKeybindings() override { return false; };

private:
    LED_Pixel *last = nullptr;
};

#endif
