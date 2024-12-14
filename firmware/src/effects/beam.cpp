#include "beam.h"
#include "led.h"
#include "globals.h"

bool FX_Beam::render(unsigned long delta) {
    if (delta > 30) {
        LED_clear(); // fade?

        LED_fillRing(&Color_Yellow, this->last);
        this->last = this->last->next;

        return true;
    }

    return false;
}

void FX_Beam::resetData() {
    this->last = LED_getRing(0);
}

void FX_Beam::onButton(GP_BUTTON button) {
}

void FX_Beam::onAnalogButton(GP_BUTTON button, int value) {
}

bool FX_SideBeam::render(unsigned long delta) {
    if (delta > 30) {
        LED_clear(); // fade?

        LED_Pixel *p = this->last;
        for (int i = 0; i < LED_TOTAL_RINGS; i++) {
            LED_setColor(&Color_Yellow, p);
            p = LED_getPixel(p->ring->next, p->i);
        }

        this->last = this->last->next;

        return true;
    }

    return false;
}

void FX_SideBeam::resetData() {
    this->last = LED_getPixel(LED_getRing(0), 0);
}

void FX_SideBeam::onButton(GP_BUTTON button) {
}

void FX_SideBeam::onAnalogButton(GP_BUTTON button, int value) {
}
