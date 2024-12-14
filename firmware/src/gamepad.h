#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "enum.h"

struct ControllerStatus {
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
};

void GP_update();

void GP_button(bool *value, GP_BUTTON type, unsigned char mask, unsigned char source);

void GP_analog(int offset, int *value, GP_BUTTON type);

#endif
