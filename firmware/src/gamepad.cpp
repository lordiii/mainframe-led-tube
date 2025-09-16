#include "gamepad.h"
#include "display.h"
#include "effects/_effects.h"
#include "enum.h"
#include "globals.h"
#include "led.h"

#include <Arduino.h>
#include <Wire.h>

#define GP_CMD_CLEAR 0x00
#define GP_CMD_ENABLE_PAIRING 0x01
#define GP_CMD_DISABLE_PAIRING 0x02

#define GP_NO_CONTROLLER 0x02
#define GP_HAS_CHANGES 0x01
#define GP_NO_CHANGES 0x00

#define GP_BTN_CNT sizeof(GP_Locks) / sizeof(bool)
#define GP_MAX_KEYBINDS 8

bool gpPairingEnabled = false;
unsigned long lastClear = 0;
unsigned long lastEnableToggle = 0;

GP_Locks gpLocks;
GP_Status gp;

unsigned char gamepadBuffer[sizeof(GP_Status) + 10] = {};

KeybindFn keybindings[GP_BTN_CNT][GP_MAX_KEYBINDS];
unsigned char bindCount[GP_BTN_CNT];

void GP_unregisterKeybind(GP_BUTTON btn, KeybindFn fn) {
    unsigned char *cnt = &bindCount[btn];

    int i;
    for (i = 0; i < *cnt; i++) {
        if ((int) keybindings[btn][i] == (int) fn) {
            break;
        }
    }

    int remaining = GP_MAX_KEYBINDS - i - 1;
    if (remaining > 0) {
        memmove(&keybindings[btn][i], &keybindings[btn][i + 1], remaining);

        (*cnt)--;
    }
}

void GP_registerKeybind(GP_BUTTON btn, KeybindFn fn) {
    unsigned char *cnt = &bindCount[btn];

    for (int i = 0; i < *cnt; i++) {
        if (keybindings[btn][i] == fn) {
            return;
        }
    }

    if (*cnt < GP_MAX_KEYBINDS) {
        keybindings[btn][*cnt] = fn;
        (*cnt)++;
    }
}

void GP_clearKeybindings(GP_BUTTON btn) {
    bindCount[btn] = 0;
}

void GP_clearKeybindings() {
    for (GP_BUTTON btn = BUTTON_Y; btn <= STICK_R_Y; btn = (GP_BUTTON) ((int) btn + 1)) {
        GP_clearKeybindings(btn);
    }
}

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

    bool *p_lock = &gpLocks.buttonY;
    bool *p_bool = &gp.buttonY;
    int *p_int = &gp.breakForce;

    LED_animationStop();

    EffectState *state = FX_getState();

    GP_BUTTON type = BUTTON_Y;
    for (; type < BREAK; type = (GP_BUTTON) ((int) type + 1)) {
        if (*p_bool && !*p_lock) {
            unsigned char bindingCnt = bindCount[type];
            KeybindFn *bindings = keybindings[type];

            for (int i = 0; i < bindingCnt; i++) {
                bindings[i](type, &gp, state->current);
            }

            *p_lock = true;
        } else if (!*p_bool) {
            *p_lock = false;
        }

        p_lock = p_lock + 1;
        p_bool = p_bool + 1;
    }

    for (; type <= STICK_R_Y; type = (GP_BUTTON) ((int) type + 1)) {
        if (*p_int != 0) {
            unsigned char bindingCnt = bindCount[type];
            KeybindFn *bindings = keybindings[type];

            for (int i = 0; i < bindingCnt; i++) {
                bindings[i](type, &gp, state->current);
            }
        }

        p_int = p_int + 1;
    }

    LED_animationStart();

    return true;
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