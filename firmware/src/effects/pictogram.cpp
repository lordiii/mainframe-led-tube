#include "_effects.h"
#include "led.h"
#include "globals.h"
#include "pictogram.h"

void arrow(LED_Pixel *pxl) {
    LED_Pixel *p = pxl;

    for (int i = 0; i < 6; i++) {
        LED_setColor(&Color_Cyan, p);
        p = p->previous;
    }

    p = LED_getPixel(pxl->ring->next, pxl->i);
    LED_setColor(&Color_Cyan, p);

    for (int i = 0; i < 2; i++) {
        p = LED_getPixel(p->ring->next, p->previous->i);
        LED_setColor(&Color_Cyan, p);
    }

    p = LED_getPixel(pxl->ring->previous, pxl->i);
    LED_setColor(&Color_Cyan, p);

    for (int i = 0; i < 2; i++) {
        p = LED_getPixel(p->ring->previous, p->previous->i);
        LED_setColor(&Color_Cyan, p);
    }
}

bool FX_Pictogram::render(unsigned long delta) {
    LED_clear();

    for (int i = 0; i < LED_PER_RING; i += LED_PER_RING / 4) {
        arrow(LED_getPixel(LED_getRing(49), i + this->last->i));
        arrow(LED_getPixel(LED_getRing(29), i + this->last->i));
        arrow(LED_getPixel(LED_getRing(9), i + this->last->i));
    }

    this->last = this->last->next;
    return true;
}

void FX_Pictogram::resetData() {
    this->last = LED_getPixel(LED_getRing(29), 0);
}