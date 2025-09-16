#ifndef DUMMY_H
#define DUMMY_H

#include "_effects.h"
#include "led.h"
#include "enum.h"

class FX_Dummy : FX {
public:
    FX_Dummy() : FX("dummy") {
    }

    bool render(unsigned long delta) override;

    void resetData() override;

    bool registerKeybindings() override { return false; };

private:
    LED_Pixel *last = nullptr;
};

#endif
