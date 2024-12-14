#ifndef MAIN_H
#define MAIN_H

#include "globals.h"

class INA226;

// Temperature Sensor Values


void fetchBusVoltageValue(INA226 sensor, TUBE_SECTION section, float *oldvalue);

void fetchCurrentValue(INA226 sensor, TUBE_SECTION section, float *oldvalue);

void fetchTemperatureValue(const unsigned char *sensor, TUBE_SECTION section, float *oldvalue);

void processControllerInputs();

void processAnalogValue(int offset, int *value, GP_BUTTON type);

#endif
