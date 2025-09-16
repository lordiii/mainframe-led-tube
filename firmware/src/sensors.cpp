#include "sensors.h"
#include "globals.h"

#include <INA226.h>
#include <DallasTemperature.h>
#include <OneWire.h>

// Current sensors
INA226 currentSensorTop(CURRENT_SENSOR_TOP_ADDRESS);
INA226 currentSensorCenter(CURRENT_SENSOR_CENTER_ADDRESS);
INA226 currentSensorBottom(CURRENT_SENSOR_BOTTOM_ADDRESS);

// Temperature probes
OneWire oneWire(24);
DallasTemperature tempSensors(&oneWire);

bool temperatureToggle = false;

SensorValues top = {
        &currentSensorTop,
        (unsigned char[]) TEMPERATURE_SENSOR_TOP_ADDRESS,
        0,
        0,
        0
};

SensorValues center = {
        &currentSensorCenter,
        (unsigned char[]) TEMPERATURE_SENSOR_CENTER_ADDRESS,
        0,
        0,
        0
};

SensorValues bottom = {
        &currentSensorBottom,
        (unsigned char[]) TEMPERATURE_SENSOR_BOTTOM_ADDRESS,
        0,
        0,
        0
};

OneWire *SENSOR_OneWire_Get() {
    return &oneWire;
}

void SENSOR_init() {
    SENSOR_init_ina(top.currentSensor);
    SENSOR_init_ina(center.currentSensor);
    SENSOR_init_ina(bottom.currentSensor);
}

void SENSOR_init_ina(INA226 *ina) {
    ina->reset();
    ina->begin();
    ina->setMaxCurrentShunt(10, 0.006);
    ina->setAverage(3);
}

void SENSOR_update(const bool temperature, const bool current) {
    SENSOR_update_values(&top, temperature, current);
    SENSOR_update_values(&center, temperature, current);
    SENSOR_update_values(&bottom, temperature, current);
}

void SENSOR_update_values(SensorValues *dst, const bool temperature, const bool current) {
    if (temperature) {
        if (temperatureToggle) {
            dst->temperature = round(tempSensors.getTempC(dst->temperatureProbeId) * 100.0f) / 100.0f;
        } else {
            tempSensors.requestTemperaturesByAddress(dst->temperatureProbeId);
        }

        temperatureToggle = !temperatureToggle;
    }

    if (current) {
        dst->busVoltage = round(dst->currentSensor->getBusVoltage() * 100.0f) / 100.0f;
        dst->current = round((dst->currentSensor->getShuntVoltage() / 0.006f) * 100.0f) / 100.0f;
    }
}

SensorValues *SENSOR_getValues(const TUBE_SECTION section) {
    switch (section) {
        case TOP_SECTION:
            return &top;
        case CENTER_SECTION:
            return &center;
        case BOTTOM_SECTION:
            return &bottom;
        default:
            return nullptr;
    }
}
