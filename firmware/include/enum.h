#ifndef ENUM_H
#define ENUM_H

// Enum ordering has to match GP_Status order
enum GP_BUTTON {

    // Boolean values:

    BUTTON_Y,
    BUTTON_B,
    BUTTON_A,
    BUTTON_X,
    DPAD_LEFT,
    DPAD_RIGHT,
    DPAD_UP,
    DPAD_DOWN,
    SHOULDER_L1,
    SHOULDER_L2,
    SHOULDER_R1,
    SHOULDER_R2,

    MISC_HOME,
    MISC_START,
    MISC_SELECT,
    MISC_SYSTEM,
    MISC_BACK,
    MISC_CAPTURE,
    BUTTON_TR,
    BUTTON_TL,

    // Analog values:

    BREAK,
    THROTTLE,
    STICK_L_X,
    STICK_L_Y,
    STICK_R_X,
    STICK_R_Y,
};

enum TUBE_SECTION {
    TOP_SECTION,
    CENTER_SECTION,
    BOTTOM_SECTION
};

#endif
