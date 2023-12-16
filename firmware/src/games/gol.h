#ifndef FIRMWARE_GOL_H
#define FIRMWARE_GOL_H

#include <globals.h>

bool calculateGOLCell(int ring, int pixel);

void initializeGOLData();

void addCopperhead(int centerRing, int centerPixel);

struct EffectGOL {
    bool state[LED_TOTAL_RINGS * LED_PER_RING];
};

bool effectGOL(unsigned long delta);

#endif //FIRMWARE_GOL_H
