#include "main.h"

#include "globals.h"
#include "cli.h"
#include "led.h"
#include "gamepad.h"
#include "display.h"

#include <Entropy.h>
#include <sensors.h>
#include <Wire.h>

// Scheduled Tasks
unsigned long taskReadSensorTemperature = 0;
unsigned long taskReadSensorCurrent = 0;
unsigned long taskReadControllerInput = 0;

unsigned long taskTimes[3] = {};

void setup() {
    delay(1000);

    Entropy.Initialize();

    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);

    Serial.begin(115200);

    pinMode(PIN_PW_ON, OUTPUT);
    pinMode(PIN_PS_GOOD, INPUT);

    digitalWrite(PIN_PW_ON, HIGH);

    Wire.begin();

    initCLI();
    LED_init();
    SENSOR_init();
    DSP_init();
}

void loop() {
    unsigned long time = millis();
    if ((time - taskReadSensorTemperature) > 1000) {
        taskReadSensorTemperature = time;

        SENSOR_update(true, false);
        taskTimes[0] = millis() - taskReadSensorTemperature;
    }

    time = millis();
    if ((time - taskReadSensorCurrent) > 100) {
        taskReadSensorCurrent = time;

        SENSOR_update(false, true);
        taskTimes[1] = millis() - taskReadSensorCurrent;
    }

    time = millis();
    if ((time - taskReadControllerInput) > 10) {
        taskReadControllerInput = time;

        if (!GP_update()) {
            GP_clear();
            GP_enablePairing();
        } else {
            GP_disablePairing();
        }

        taskTimes[2] = millis() - taskReadControllerInput;
    }

    processCLI();

    if (Wire.getReadError()) {
        Wire.clearReadError();
    }

    if (Wire.getWriteError()) {
        Wire.clearWriteError();
    }
}
