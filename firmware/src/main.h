#ifndef MAIN_H
#define MAIN_H

#include "enum.h"

class INA226;

// Temperature Sensor Values
struct SensorValues {
    float temperatureTop;
    float temperatureCenter;
    float temperatureBottom;

    float currentSensorBottom;
    float currentSensorCenter;
    float currentSensorTop;

    float busVoltageBottom;
    float busVoltageCenter;
    float busVoltageTop;
};

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

extern SensorValues *sensorValues;
extern ControllerStatus *controller;

void fetchBusVoltageValue(INA226 sensor, TUBE_SECTION section, float *oldvalue);

void fetchCurrentValue(INA226 sensor, TUBE_SECTION section, float *oldvalue);

void fetchTemperatureValue(const unsigned char *sensor, TUBE_SECTION section, float *oldvalue);

void processControllerInputs();

void processAnalogValue(int offset, int *value, Button type);

#endif
