#ifndef MAIN_H
#define MAIN_H

#include "globals.h"

class INA226;

// Temperature Sensor Values
struct ControllerStatus {
    bool buttonY;
    bool buttonB;
    bool buttonA;
    bool buttonX;
    bool dpadLeft;
    bool dpadRight;
    bool dpadUp;
    bool dpadDown;
    bool shoulderL1;
    bool shoulderL2;
    bool shoulderR1;
    bool shoulderR2;

    bool miscHome;
    bool miscStart;
    bool miscSelect;
    bool miscSystem;
    bool miscBack;
    bool miscCapture;
    bool buttonTR;
    bool buttonTL;

    int breakForce;
    int throttleForce;
    int stickLX;
    int stickLY;
    int stickRX;
    int stickRY;
};

void fetchBusVoltageValue(INA226 sensor, TUBE_SECTION section, float *oldvalue);

void fetchCurrentValue(INA226 sensor, TUBE_SECTION section, float *oldvalue);

void fetchTemperatureValue(const unsigned char *sensor, TUBE_SECTION section, float *oldvalue);

void processControllerInputs();

void processAnalogValue(int offset, int *value, Button type);

#endif
