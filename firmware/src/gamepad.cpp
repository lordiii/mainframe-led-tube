#include "gamepad.h"
#include "enum.h"
#include "effects/_effects.h"
#include "globals.h"
#include "display.h"

#include <Arduino.h>
#include <Wire.h>

GamepadStatus gamepad;
unsigned char gamepadBuffer[27] = {};

bool GP_update() {
    memset(gamepadBuffer, 0, sizeof(gamepadBuffer));

    uint8_t quantity = Wire.requestFrom(0x55, sizeof(gamepadBuffer));
    Wire.readBytes(gamepadBuffer, quantity);
    Wire.begin();

    if (gamepadBuffer[0] == 0xFF && gamepadBuffer[1] == 0xFF) {
        memset(gamepadBuffer, 0, sizeof(gamepadBuffer));
        return false;
    }

    GP_button(&gamepad.dpadLeft, DPAD_LEFT, 0b00001000, gamepadBuffer[0]);
    GP_button(&gamepad.dpadRight, DPAD_RIGHT, 0b00000100, gamepadBuffer[0]);
    GP_button(&gamepad.dpadDown, DPAD_DOWN, 0b00000010, gamepadBuffer[0]);
    GP_button(&gamepad.dpadUp, DPAD_UP, 0b00000001, gamepadBuffer[0]);

    GP_button(&gamepad.buttonY, BUTTON_Y, 0b10000000, gamepadBuffer[1]);
    GP_button(&gamepad.buttonB, BUTTON_B, 0b01000000, gamepadBuffer[1]);
    GP_button(&gamepad.buttonA, BUTTON_A, 0b00100000, gamepadBuffer[1]);
    GP_button(&gamepad.buttonX, BUTTON_X, 0b00010000, gamepadBuffer[1]);
    GP_button(&gamepad.shoulderL1, SHOULDER_L1, 0b00001000, gamepadBuffer[1]);
    GP_button(&gamepad.shoulderR2, SHOULDER_R2, 0b00000100, gamepadBuffer[1]);
    GP_button(&gamepad.shoulderR1, SHOULDER_R1, 0b00000010, gamepadBuffer[1]);
    GP_button(&gamepad.shoulderL2, SHOULDER_L2, 0b00000001, gamepadBuffer[1]);

    GP_button(&gamepad.miscHome, MISC_HOME, 0b10000000, gamepadBuffer[2]);
    GP_button(&gamepad.miscStart, MISC_START, 0b01000000, gamepadBuffer[2]);
    GP_button(&gamepad.miscSelect, MISC_SELECT, 0b00100000, gamepadBuffer[2]);
    GP_button(&gamepad.miscSystem, MISC_SYSTEM, 0b00010000, gamepadBuffer[2]);
    GP_button(&gamepad.miscBack, MISC_BACK, 0b00001000, gamepadBuffer[2]);
    GP_button(&gamepad.miscCapture, MISC_CAPTURE, 0b00000100, gamepadBuffer[2]);
    GP_button(&gamepad.buttonTR, BUTTON_TR, 0b00000010, gamepadBuffer[2]);
    GP_button(&gamepad.buttonTL, BUTTON_TL, 0b00000001, gamepadBuffer[2]);

    GP_analog(3, &gamepad.breakForce, BREAK);
    GP_analog(7, &gamepad.throttleForce, THROTTLE);
    GP_analog(11, &gamepad.stickLX, STICK_L_X);
    GP_analog(15, &gamepad.stickLY, STICK_L_Y);
    GP_analog(19, &gamepad.stickRX, STICK_R_X);
    GP_analog(23, &gamepad.stickRY, STICK_R_Y);

    GP_processButtons();

    return true;
}

void GP_processButtons() {
    EffectState *state = FX_getState();
    bool navEnabled = state->halt || state->current == nullptr;

    if (gamepad.miscHome.value && !gamepad.miscHome.locked) {
        state->halt = !state->halt;
        gamepad.miscHome.locked = true;
    }

    if (navEnabled) {
        if (gamepad.dpadUp.value && !gamepad.dpadUp.locked) {
            DSP_nextButton(-1);
            gamepad.dpadUp.locked = true;
        } else if (gamepad.dpadDown.value && !gamepad.dpadDown.locked) {
            DSP_nextButton(1);
            gamepad.dpadDown.locked = true;
        }

        if (gamepad.buttonA.value && !gamepad.buttonA.locked) {
            DSP_selectButton();
            gamepad.buttonA.locked = true;
        }
    }
}

GamepadStatus *GP_getState() {
    return &gamepad;
}

void GP_button(GP_ButtonInput *target, GP_BUTTON type, unsigned char mask, unsigned char source) {
    bool pressed = (source & mask) > 0;
    bool handled = false;

    EffectState *state = FX_getState();
    if (!state->halt && state->current != nullptr && pressed && !target->value) {
        handled = state->current->onButton(type);
    }
    target->value = pressed;

    if (!handled) {
        if (!target->value) {
            target->locked = target->value;
        }
    } else {
        target->locked = true;
    }
}

void GP_analog(int offset, GP_AnalogInput *target, GP_BUTTON type) {
    bool handled = false;

    target->value = ((int) (gamepadBuffer[offset + 0])) << 24 | ((int) gamepadBuffer[offset + 1]) << 16 |
                    ((int) gamepadBuffer[offset + 2]) << 8 | ((int) gamepadBuffer[offset + 3]);

    EffectState *state = FX_getState();
    if (!state->halt && state->current != nullptr && target->value != 0) {
        handled = state->current->onAnalogButton(type, target->value);
    }

    if (!handled) {
        if (!target->value) {
            target->locked = target->value;
        }
    } else {
        target->locked = true;
    }
}

void GP_enablePairing() {
    Wire.beginTransmission(I2C_CONTROLLER);
    Wire.write(0x01);
    Wire.endTransmission();
}

void GP_disablePairing() {
    Wire.beginTransmission(I2C_CONTROLLER);
    Wire.write(0x02);
    Wire.endTransmission();
}

void GP_clear() {
    Wire.beginTransmission(I2C_CONTROLLER);
    Wire.write(0x01);
    Wire.endTransmission();
}