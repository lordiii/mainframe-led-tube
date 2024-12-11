#ifndef TFT_H
#define TFT_H

#include "globals.h"

struct XY {
    short X;
    short Y;
};

void initTFT();

XY getStartPosition(TUBE_SECTION section);

void renderCurrentValue(TUBE_SECTION section, float value);

void renderVoltageValue(TUBE_SECTION section, float value);

void renderTemperatureValue(TUBE_SECTION section, float value);

void displayEffect(const char *effectName);

#endif
