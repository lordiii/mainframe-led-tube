#ifndef FIRMWARE_TFT_H
#define FIRMWARE_TFT_H

#include <Arduino.h>

enum TUBE_SECTION {
    TOP,
    CENTER,
    BOTTOM
};

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