#include "gamepad.h"
#include "enum.h"
#include "effects/_effects.h"
#include "globals.h"
#include "display.h"

#include <Arduino.h>
#include <Wire.h>

#define GP_CMD_CLEAR 0x00
#define GP_CMD_ENABLE_PAIRING 0x01
#define GP_CMD_DISABLE_PAIRING 0x02

#define GP_NO_CONTROLLER 0x02
#define GP_HAS_CHANGES 0x01
#define GP_NO_CHANGES 0x00

bool gpPairingEnabled = false;
unsigned long lastClear = 0;
unsigned long lastEnableToggle = 0;

GP_Locks gpLocks;
GP_Status gp;

unsigned char gamepadBuffer[sizeof(GP_Status) + 10] = {};

bool GP_update() {
    uint8_t quantity = Wire.requestFrom(0x55, sizeof(GP_Status) + 1);
    Wire.readBytes(gamepadBuffer, quantity);
    Wire.begin();

    switch (gamepadBuffer[0]) {
        case GP_NO_CHANGES:
            return true;
        case GP_HAS_CHANGES:
            break;
        case GP_NO_CONTROLLER:
        default:
            return false;
    }

    if ((sizeof(GP_Status) + 1) != quantity) {
        return false;
    }

    memcpy(&gp, gamepadBuffer + 1, sizeof(GP_Status));

    EffectState *state = FX_getState();
    bool hasEventHandler = !state->halt && state->current;

    bool *p_lock = &gpLocks.buttonY;
    bool *p_bool = &gp.buttonY;
    int *p_int = &gp.breakForce;

    for (GP_BUTTON type = BUTTON_Y; type < BREAK; type = (GP_BUTTON) ((int) type + 1)) {
        bool handled = false;

        if (hasEventHandler && *p_bool && !*p_lock) {
            handled = state->current->onButton(type);
        }

        if (handled) {
            *p_lock = true;
        } else {
            if (!*p_bool) {
                *p_lock = false;
            }
        }

        p_lock = p_lock + 1;
        p_bool = p_bool + 1;
    }

    GP_processButtons();

    return true;
}

void GP_processButtons() {
    EffectState *state = FX_getState();
    bool navEnabled = state->halt || state->current == nullptr;

    if (gp.miscHome && !gpLocks.miscHome) {
        state->halt = !state->halt;
        gpLocks.miscHome = true;
    }

    if (navEnabled) {
        if (gp.dpadUp && !gpLocks.dpadUp) {
            DSP_nextButton(-1);
            gpLocks.dpadUp = true;
        } else if (gp.dpadDown && !gpLocks.dpadDown) {
            DSP_nextButton(1);
            gpLocks.dpadDown = true;
        }

        if (gp.buttonA && !gpLocks.buttonA) {
            DSP_selectButton();
            gpLocks.buttonA = true;
        }
    }
}

GP_Status *GP_getState() {
    return &gp;
}

/*
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
*/

void GP_enablePairing() {
    if (millis() - lastEnableToggle > 1000 || !gpPairingEnabled) {
        Wire.beginTransmission(I2C_CONTROLLER);
        Wire.write(GP_CMD_ENABLE_PAIRING);
        Wire.endTransmission();

        lastEnableToggle = millis();
    }
}

void GP_disablePairing() {
    if (millis() - lastEnableToggle > 1000 || gpPairingEnabled) {
        Wire.beginTransmission(I2C_CONTROLLER);
        Wire.write(GP_CMD_DISABLE_PAIRING);
        Wire.endTransmission();

        lastEnableToggle = millis();
    }
}

void GP_clear() {
    if (millis() - lastClear > 1000) {
        Wire.beginTransmission(I2C_CONTROLLER);
        Wire.write(GP_CMD_CLEAR);
        Wire.endTransmission();

        lastClear = millis();
    }
}