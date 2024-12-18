#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "enum.h"

typedef struct GP_Locks {
    bool buttonY;
    bool buttonB;
    bool buttonA;
    bool buttonX;
    bool dpadLeft;
    bool dpadRight;
    bool dpadUp;
    bool dpadDown;
    bool shoulderL1;
    bool shoulderL2;
    bool shoulderR1;
    bool shoulderR2;

    bool miscHome;
    bool miscStart;
    bool miscSelect;
    bool miscSystem;
    bool miscBack;
    bool miscCapture;
    bool buttonTR;
    bool buttonTL;

    bool breakForce;
    bool throttleForce;
    bool stickLX;
    bool stickLY;
    bool stickRX;
    bool stickRY;
} GP_Locks;

typedef struct GP_Status {
    bool buttonY;
    bool buttonB;
    bool buttonA;
    bool buttonX;
    bool dpadLeft;
    bool dpadRight;
    bool dpadUp;
    bool dpadDown;
    bool shoulderL1;
    bool shoulderL2;
    bool shoulderR1;
    bool shoulderR2;

    bool miscHome;
    bool miscStart;
    bool miscSelect;
    bool miscSystem;
    bool miscBack;
    bool miscCapture;
    bool buttonTR;
    bool buttonTL;

    int breakForce;
    int throttleForce;
    int stickLX;
    int stickLY;
    int stickRX;
    int stickRY;
} GP_Status;

bool GP_update();

void GP_processButtons();

void GP_enablePairing();

void GP_disablePairing();

void GP_clear();

GP_Status *GP_getState();

#endif
