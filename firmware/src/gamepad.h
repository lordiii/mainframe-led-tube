#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "enum.h"

typedef struct GP_ButtonInput {
    bool value;
    bool locked;
} GP_ButtonInput;

typedef struct GP_AnalogInput {
    int value;
    bool locked;
} GP_AnalogInput;

struct GamepadStatus {
    GP_ButtonInput buttonY;
    GP_ButtonInput buttonB;
    GP_ButtonInput buttonA;
    GP_ButtonInput buttonX;
    GP_ButtonInput dpadLeft;
    GP_ButtonInput dpadRight;
    GP_ButtonInput dpadUp;
    GP_ButtonInput dpadDown;
    GP_ButtonInput shoulderL1;
    GP_ButtonInput shoulderL2;
    GP_ButtonInput shoulderR1;
    GP_ButtonInput shoulderR2;

    GP_ButtonInput miscHome;
    GP_ButtonInput miscStart;
    GP_ButtonInput miscSelect;
    GP_ButtonInput miscSystem;
    GP_ButtonInput miscBack;
    GP_ButtonInput miscCapture;
    GP_ButtonInput buttonTR;
    GP_ButtonInput buttonTL;

    GP_AnalogInput breakForce;
    GP_AnalogInput throttleForce;
    GP_AnalogInput stickLX;
    GP_AnalogInput stickLY;
    GP_AnalogInput stickRX;
    GP_AnalogInput stickRY;
};

bool GP_update();

void GP_button(GP_ButtonInput *value, GP_BUTTON type, unsigned char mask, unsigned char source);

void GP_analog(int offset, GP_AnalogInput *target, GP_BUTTON type);

void GP_processButtons();

void GP_enablePairing();

void GP_disablePairing();

void GP_clear();

GamepadStatus *GP_getState();

#endif
