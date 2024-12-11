#include "main.h"

#include "effects/_effects.h"
#include "tft.h"
#include "globals.h"
#include "cli.h"
#include "led.h"

#include <Entropy.h>
#include <sensors.h>
#include <Wire.h>

// Scheduled Tasks
unsigned long taskReadSensors = 0;
unsigned long taskActivityLed = 0;
unsigned long taskReadCurrent = 0;
unsigned long taskReadControllerInput = 0;

// Sensor Values
ControllerStatus controller;

void setup()
{
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

uint8_t controllerBuffer[27] = {};
bool toggleTemperatureReadWrite = false;
bool activityLedState = false;

void loop()
{
    unsigned long time = millis();

    if ((time - taskReadSensors) > 1000)
    {
        taskReadSensors = time;
        SENSOR_update(true, true);
    }

    if ((time - taskReadCurrent) > 100)
    {
        taskReadCurrent = time;
        SENSOR_update(false, true);
    }

    if ((time - taskReadControllerInput) > 20)
    {
        memset(controllerBuffer, 0, sizeof(controllerBuffer));
        uint8_t quantity = Wire.requestFrom(0x55, sizeof(controllerBuffer));
        Wire.readBytes(controllerBuffer, quantity);
        processControllerInputs();
        Wire.begin();

        taskReadControllerInput = time;
    }

    processCLI();

    if (Wire.getReadError())
    {
        Wire.clearReadError();
    }

    if (Wire.getWriteError())
    {
        Wire.clearWriteError();
    }
}

void processButton(bool* value, Button type, uint8_t mask, uint8_t source)
{
    EffectState* state = FX_getState();
    if (state->current == nullptr || state->current->onButtonPress == nullptr) return;

    bool pressed = (source & mask) > 0;

    if (pressed && !*value)
    {
        state->current->onButtonPress(type);
    }
    *value = pressed;
}

void processAnalogValue(int offset, int* value, Button type)
{
    EffectState* state = FX_getState();
    if (state->current == nullptr || state->current->onAnalogButton == nullptr) return;

    *value = ((int)(controllerBuffer[offset + 0])) << 24 | ((int)controllerBuffer[offset + 1]) << 16 |
        ((int)controllerBuffer[offset + 2]) << 8 | ((int)controllerBuffer[offset + 3]);

    if (*value != 0)
    {
        state->current->onAnalogButton(type, *value);
    }
}

void processControllerInputs()
{
    processButton(&controller.dpadLeft, DPAD_LEFT, 0b00001000, controllerBuffer[0]);
    processButton(&controller.dpadRight, DPAD_RIGHT, 0b00000100, controllerBuffer[0]);
    processButton(&controller.dpadDown, DPAD_DOWN, 0b00000010, controllerBuffer[0]);
    processButton(&controller.dpadUp, DPAD_UP, 0b00000001, controllerBuffer[0]);

    processButton(&controller.buttonY, BUTTON_Y, 0b10000000, controllerBuffer[1]);
    processButton(&controller.buttonB, BUTTON_B, 0b01000000, controllerBuffer[1]);
    processButton(&controller.buttonA, BUTTON_A, 0b00100000, controllerBuffer[1]);
    processButton(&controller.buttonX, BUTTON_X, 0b00010000, controllerBuffer[1]);
    processButton(&controller.shoulderL1, SHOULDER_L1, 0b00001000, controllerBuffer[1]);
    processButton(&controller.shoulderR2, SHOULDER_R2, 0b00000100, controllerBuffer[1]);
    processButton(&controller.shoulderR1, SHOULDER_R1, 0b00000010, controllerBuffer[1]);
    processButton(&controller.shoulderL2, SHOULDER_L2, 0b00000001, controllerBuffer[1]);

    processButton(&controller.miscHome, MISC_HOME, 0b10000000, controllerBuffer[2]);
    processButton(&controller.miscStart, MISC_START, 0b01000000, controllerBuffer[2]);
    processButton(&controller.miscSelect, MISC_SELECT, 0b00100000, controllerBuffer[2]);
    processButton(&controller.miscSystem, MISC_SYSTEM, 0b00010000, controllerBuffer[2]);
    processButton(&controller.miscBack, MISC_BACK, 0b00001000, controllerBuffer[2]);
    processButton(&controller.miscCapture, MISC_CAPTURE, 0b00000100, controllerBuffer[2]);
    processButton(&controller.buttonTR, BUTTON_TR, 0b00000010, controllerBuffer[2]);
    processButton(&controller.buttonTL, BUTTON_TL, 0b00000001, controllerBuffer[2]);

    processAnalogValue(3, &controller.breakForce, BREAK);
    processAnalogValue(7, &controller.throttleForce, THROTTLE);
    processAnalogValue(11, &controller.stickLX, STICK_L_X);
    processAnalogValue(15, &controller.stickLY, STICK_L_Y);
    processAnalogValue(19, &controller.stickRX, STICK_R_X);
    processAnalogValue(23, &controller.stickRY, STICK_R_Y);
}
