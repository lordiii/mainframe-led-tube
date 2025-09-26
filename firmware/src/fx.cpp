#include "fx.h"
#include "led.h"

#include <Arduino.h>

#define EXECUTABLE_SIZE ((1024 * 64) / sizeof(uint8_t)) // 64 KiB memory for external executable

__attribute__((section(".fx_jmp_table"))) s_jmp_tbl fx_jmp_table;
__attribute__((section(".fx_binary"))) uint8_t fx_binary[EXECUTABLE_SIZE];

void FX2_init() {
    fx_jmp_table.version = 1;

    fx_jmp_table.LED_fillRing = &LED_fillRing;
    fx_jmp_table.LED_fillSection = &LED_fillSection;
    fx_jmp_table.LED_clear = &LED_clear;
    fx_jmp_table.LED_move = &LED_move;

    memset(fx_binary, 0, EXECUTABLE_SIZE * sizeof(uint8_t));
}