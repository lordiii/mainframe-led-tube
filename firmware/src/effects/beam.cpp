#include "beam.h"
#include "_effects.h"
#include "led.h"
#include "globals.h"

void FX_resetBeam(EffectData *effectData) {
    FX_Beam *data = &(effectData->beam);

    data->last = LED_getRing(0);
}

void FX_resetSideBeam(EffectData *effectData) {
    FX_SideBeam *data = &(effectData->sideBeam);

    data->last = LED_getPixel(LED_getRing(0), 0);
}

bool FX_beam(EffectData *effectData, unsigned long delta) {
    FX_Beam *data = &(effectData->beam);

    if (delta > 30) {
        LED_clear(); // fade?

        LED_fillRing(&Color_Yellow, data->last);
        data->last = data->last->next;

        return true;
    }

    return false;
}

bool FX_sideBeam(EffectData *effectData, unsigned long delta) {
    FX_SideBeam *data = &(effectData->sideBeam);

    if (delta > 30) {
        LED_clear(); // fade?

        LED_Pixel *p = data->last;
        for (int i = 0; i < LED_TOTAL_RINGS; i++) {
            LED_setColor(&Color_Yellow, p);
            p = LED_getPixel(p->ring->next, p->i);
        }

        data->last = data->last->next;

        return true;
    }

    return false;
}