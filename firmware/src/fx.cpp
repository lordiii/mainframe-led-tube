#include "fx.h"
#include "led.h"

#include <Arduino.h>

#define EXECUTABLE_SIZE ((1024 * 64) / sizeof(uint8_t)) // 64 KiB memory for external executable

__attribute__((section(".fx_jmp_table"))) __attribute__((aligned(4))) s_jmp_tbl fx_jmp_table;
__attribute__((section(".fx_binary"))) __attribute__((aligned(4))) uint8_t fx_binary[EXECUTABLE_SIZE];

FXState state;

FXState *FX_getState() {
    return &state;
}

void FX_register(FX_render fxRender, FX_resetData fxResetData, FX_registerKeybindings fxRegisterKeybindings) {
    state.fn_render = nullptr;
    state.fn_resetData = nullptr;
    state.fn_registerKeybindings = nullptr;

    if (fxRender == nullptr || fxResetData == nullptr) {
        return;
    }

    state.fn_render = fxRender;
    state.fn_resetData = fxResetData;
    state.fn_registerKeybindings = fxRegisterKeybindings;
}

void FX_init() {
    fx_jmp_table.version = 1;

    fx_jmp_table.FX_register = &FX_register;

    fx_jmp_table.LED_fillRing = &LED_fillRing;
    fx_jmp_table.LED_fillSection = &LED_fillSection;
    fx_jmp_table.LED_clear = &LED_clear;
    fx_jmp_table.LED_move = &LED_move;
    fx_jmp_table.LED_getRing = &LED_getRing;
    fx_jmp_table.LED_getPixel = &LED_getPixel;
    fx_jmp_table.LED_rotateAll = &LED_rotateAll;
    fx_jmp_table.LED_rotateRing = &LED_rotateRing;

    memset(fx_binary, 0, EXECUTABLE_SIZE * sizeof(uint8_t));
}