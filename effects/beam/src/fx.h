#ifndef FX_H
#define FX_H

#define LED_TOTAL_RINGS 60
#define LED_PER_RING 32

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


struct LED_RGB {
    unsigned char G;
    unsigned char R;
    unsigned char B;
};

struct LED_Ring;
struct LED_Pixel;

struct LED_Pixel {
    int i;

    LED_RGB *color;

    LED_Ring *ring;
    LED_Pixel *previous;
    LED_Pixel *next;
};

struct LED_Ring {
    int i;
    LED_Pixel *start;
    LED_Ring *previous;
    LED_Ring *next;
};

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

extern "C" void init();

typedef bool(*FX_render)(unsigned long delta);

typedef void(*FX_resetData)();

typedef void(*FX_registerKeybindings)();

typedef void (*KeybindFn)(GP_BUTTON btn, GP_Status *);

struct s_jmp_tbl {
    unsigned int version;

    void (*FX_register)(FX_render, FX_resetData, FX_registerKeybindings);

    void (*LED_fillRing)(LED_RGB, LED_Ring *);
    void (*LED_fillSection)(LED_RGB color, LED_Pixel *start, LED_Pixel *end);
    void (*LED_clear)();
    void (*LED_move)(LED_Pixel *src_s, LED_Pixel *src_e, LED_Pixel *dst);
    LED_Ring *(*LED_getRing)(int);
    LED_Pixel *(*LED_getPixel)(LED_Ring *, int);
    void (*LED_rotateAll)(bool);
    void (*LED_rotateRing)(LED_Ring *, bool);

    void (*GP_registerKeybind)(GP_BUTTON, KeybindFn);
};

extern s_jmp_tbl SYSTEM;

#endif
