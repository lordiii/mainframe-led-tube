#include "gamepad.h"
#include "enum.h"
#include "effects/_effects.h"

#include <Arduino.h>
#include <Wire.h>

ControllerStatus controller;
unsigned char controllerBuffer[27] = {};

void GP_update() {
    memset(controllerBuffer, 0, sizeof(controllerBuffer));

    uint8_t quantity = Wire.requestFrom(0x55, sizeof(controllerBuffer));
    Wire.readBytes(controllerBuffer, quantity);
    Wire.begin();

    GP_button(&controller.dpadLeft, DPAD_LEFT, 0b00001000, controllerBuffer[0]);
    GP_button(&controller.dpadRight, DPAD_RIGHT, 0b00000100, controllerBuffer[0]);
    GP_button(&controller.dpadDown, DPAD_DOWN, 0b00000010, controllerBuffer[0]);
    GP_button(&controller.dpadUp, DPAD_UP, 0b00000001, controllerBuffer[0]);

    GP_button(&controller.buttonY, BUTTON_Y, 0b10000000, controllerBuffer[1]);
    GP_button(&controller.buttonB, BUTTON_B, 0b01000000, controllerBuffer[1]);
    GP_button(&controller.buttonA, BUTTON_A, 0b00100000, controllerBuffer[1]);
    GP_button(&controller.buttonX, BUTTON_X, 0b00010000, controllerBuffer[1]);
    GP_button(&controller.shoulderL1, SHOULDER_L1, 0b00001000, controllerBuffer[1]);
    GP_button(&controller.shoulderR2, SHOULDER_R2, 0b00000100, controllerBuffer[1]);
    GP_button(&controller.shoulderR1, SHOULDER_R1, 0b00000010, controllerBuffer[1]);
    GP_button(&controller.shoulderL2, SHOULDER_L2, 0b00000001, controllerBuffer[1]);

    GP_button(&controller.miscHome, MISC_HOME, 0b10000000, controllerBuffer[2]);
    GP_button(&controller.miscStart, MISC_START, 0b01000000, controllerBuffer[2]);
    GP_button(&controller.miscSelect, MISC_SELECT, 0b00100000, controllerBuffer[2]);
    GP_button(&controller.miscSystem, MISC_SYSTEM, 0b00010000, controllerBuffer[2]);
    GP_button(&controller.miscBack, MISC_BACK, 0b00001000, controllerBuffer[2]);
    GP_button(&controller.miscCapture, MISC_CAPTURE, 0b00000100, controllerBuffer[2]);
    GP_button(&controller.buttonTR, BUTTON_TR, 0b00000010, controllerBuffer[2]);
    GP_button(&controller.buttonTL, BUTTON_TL, 0b00000001, controllerBuffer[2]);

    GP_analog(3, &controller.breakForce, BREAK);
    GP_analog(7, &controller.throttleForce, THROTTLE);
    GP_analog(11, &controller.stickLX, STICK_L_X);
    GP_analog(15, &controller.stickLY, STICK_L_Y);
    GP_analog(19, &controller.stickRX, STICK_R_X);
    GP_analog(23, &controller.stickRY, STICK_R_Y);
}

void GP_button(bool *target, GP_BUTTON type, unsigned char mask, unsigned char source) {
    bool pressed = (source & mask) > 0;

    EffectState *state = FX_getState();
    if (state->current != nullptr && pressed && !*target) {
        state->current->onButton(type);
    }
    *target = pressed;
}

void GP_analog(int offset, int *value, GP_BUTTON type) {
    *value = ((int) (controllerBuffer[offset + 0])) << 24 | ((int) controllerBuffer[offset + 1]) << 16 |
             ((int) controllerBuffer[offset + 2]) << 8 | ((int) controllerBuffer[offset + 3]);

    EffectState *state = FX_getState();
    if (state->current != nullptr && *value != 0) {
        state->current->onAnalogButton(type, *value);
    }
}