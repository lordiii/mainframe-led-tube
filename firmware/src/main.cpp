#include "main.h"

#include "effects/_effects.h"
#include "tft.h"
#include "globals.h"
#include "cli.h"
#include "led.h"
#include "gamepad.h"

#include <Entropy.h>
#include <sensors.h>
#include <Wire.h>

// Scheduled Tasks
unsigned long taskReadSensors = 0;
unsigned long taskUpdateTFT = 0;
unsigned long taskReadCurrent = 0;
unsigned long taskReadControllerInput = 0;

void setup() {
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);

    Serial.begin(115200);

    pinMode(PIN_PW_ON, OUTPUT);
    pinMode(PIN_PS_GOOD, INPUT);

    digitalWrite(PIN_PW_ON, HIGH);

    Wire.begin();

    initCLI();
    LED_init();
    initTFT();

    Entropy.Initialize();

    FX_setEffect("beam");
}

void loop() {
    unsigned long time = millis();

    if ((time - taskReadSensors) > 1000) {
        taskReadSensors = time;
        SENSOR_update(true, false);
    }

    if ((time - taskReadCurrent) > 100) {
        taskReadCurrent = time;
        SENSOR_update(false, true);
    }

    if ((time - taskReadControllerInput) > 20) {
        GP_update();
        taskReadControllerInput = time;
    }

    processCLI();

    if ((time - taskUpdateTFT) > 250) {
        taskUpdateTFT = time;
        TFT_update();
    }

    if (Wire.getReadError()) {
        Wire.clearReadError();
    }

    if (Wire.getWriteError()) {
        Wire.clearWriteError();
    }
}
