#ifndef SENSORS_H
#define SENSORS_H

#include <INA226.h>
#include "enum.h"

typedef struct SensorValues
{
    INA226* currentSensor;
    const unsigned char* temperatureProbeId;

    float current;
    float busVoltage;
    float temperature;
} SensorValues;

void SENSOR_init();
void SENSOR_init_ina(INA226* ina);
void SENSOR_update(bool temperature, bool current);
void SENSOR_update_values(SensorValues* dst, bool temperature, bool current);
SensorValues* getSensorValues(TUBE_SECTION section);

#endif
