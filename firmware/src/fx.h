#ifndef FX_H
#define FX_H

#include "elf.h"
#include "led.h"
#include "gamepad.h"
#include "fs.h"

#define MAX_EFFECTS 32
#define EFFECT_NAME_LENGTH 64

typedef void(*FX_init_elf)();

typedef bool(*FX_render)(unsigned long delta);

typedef void(*FX_resetData)();

typedef void(*FX_registerKeybindings)();

struct FXState {
    FX_render fn_render;
    FX_resetData fn_resetData;
    FX_registerKeybindings fn_registerKeybindings;

    unsigned int lastFrameChange;
    unsigned int slowRate;
    bool halt;
    bool singleStep;
};

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

struct EffectList {
    char names[MAX_EFFECTS][EFFECT_NAME_LENGTH];
    int count;
};

Elf32_Addr *FX_readElf(elf32_hdr *header);

bool FX_setEffect(const char *effectName);

EffectList *FX_getEffects();

FXState *FX_getState();

void FX_init();

#endif
