#include "fx.h"

LED_RGB color = {.G = 0xFF, .R = 0xFF, .B = 0xFF};

LED_Ring *lastRing = nullptr;
LED_Pixel *lastPixel = nullptr;

bool horizontalMode = true;

void reset() {
    lastRing = SYSTEM.LED_getRing(0);
    lastPixel = lastRing->start;
}

bool render(unsigned long delta) {

    if (delta > 30) {
        SYSTEM.LED_clear();

        if(horizontalMode) {
            SYSTEM.LED_fillRing(color, lastRing);
            lastRing = lastRing->next;
        } else {
            LED_Pixel *p = lastPixel;
            for (int i = 0; i < LED_TOTAL_RINGS; i++) {
                *p->color = color;
                p = SYSTEM.LED_getPixel(p->ring->next, p->i);
            }

            lastPixel = lastPixel->next;
        }

        return true;
    }

    return false;
}

void onChangeMode(GP_BUTTON btn, GP_Status *gp) {
    horizontalMode = !horizontalMode;
}

void registerKeybindings() {
    SYSTEM.GP_registerKeybind(BUTTON_A, &onChangeMode);
}

void init() {
    SYSTEM.FX_register(&render, &reset, &registerKeybindings);
}