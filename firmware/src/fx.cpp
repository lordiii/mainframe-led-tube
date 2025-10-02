#include "fx.h"
#include "led.h"
#include "fs.h"
#include "elf.h"

#define NOEXEC        SCB_MPU_RASR_XN
#define READWRITE    SCB_MPU_RASR_AP(3)
#define MEM_NOCACHE    SCB_MPU_RASR_TEX(1)
#define SIZE_512K    (SCB_MPU_RASR_SIZE(18) | SCB_MPU_RASR_ENABLE)
#define REGION(n)    (SCB_MPU_RBAR_REGION(n) | SCB_MPU_RBAR_VALID)

#include <Arduino.h>

#define EXECUTABLE_SIZE ((1024 * 64) / sizeof(uint8_t)) // 64 KiB memory for external executable

__attribute__((section(".fx_jmp_table"))) __attribute__((aligned(4))) s_jmp_tbl fx_jmp_table;
__attribute__((section(".fx_binary"))) __attribute__((aligned(4))) char fx_binary[EXECUTABLE_SIZE];

const char *effectFolder = (const char *) F("/tube/effects/");

EffectList effectList;
FXState state;

FXState *FX_getState() {
    return &state;
}

bool FX_setEffect(const char *effectName) {
    LED_animationStop();

    state.fn_render = nullptr;
    state.fn_resetData = nullptr;
    state.fn_registerKeybindings = nullptr;

    int size = FS_loadFile(effectFolder, effectName, fx_binary, EXECUTABLE_SIZE);
    if (size <= 0) {
        return false;
    }

    Elf32_Addr *fnInit = FX_readElf((elf32_hdr *) (fx_binary));

    if (fnInit == nullptr) {
        Serial.println("ELF failure");
        return false;
    }

    Serial.println((int) *fnInit, 16);
    ((FX_init_elf) (*fnInit))();

    if (state.fn_resetData == nullptr || state.fn_render == nullptr) {
        Serial.println("no render/reset registered");
        return false;
    }

    state.fn_resetData();

    if (state.fn_registerKeybindings != nullptr) {
        state.fn_registerKeybindings();
    }

    LED_animationStart();
    return true;
}

Elf32_Addr *FX_readElf(elf32_hdr *header) {
    if (header->e_ident[EI_MAG0] != ELFMAG0
        || header->e_ident[EI_MAG1] != ELFMAG1
        || header->e_ident[EI_MAG2] != ELFMAG2
        || header->e_ident[EI_MAG3] != ELFMAG3) {
        Serial.println("INVALID MAGIC NUMBER");
        return nullptr;
    }

    if (header->e_ident[EI_CLASS] != ELFCLASS32) {
        Serial.println("ELF NOT 32-BIT");
        return nullptr;
    }

    if (header->e_ident[EI_DATA] != ELFDATA2LSB) {
        Serial.println("ELF NOT LSB");
        return nullptr;
    }

    if (header->e_ident[EI_DATA] != ELFDATA2LSB) {
        Serial.println("ELF NOT LSB");
        return nullptr;
    }

    if (header->e_ident[EI_OSABI] != ELFOSABI_NONE) {
        Serial.println("ELF WRONG OSABI");
        return nullptr;
    }

    if (header->e_type != ET_EXEC) {
        Serial.println("Unsupported ELF type");
        return nullptr;
    }

    return &header->e_entry;
}

void FX_register(FX_render fxRender, FX_resetData fxResetData, FX_registerKeybindings fxRegisterKeybindings) {
    if (fxRender == nullptr || fxResetData == nullptr) {
        return;
    }

    state.fn_render = fxRender;
    state.fn_resetData = fxResetData;
    state.fn_registerKeybindings = fxRegisterKeybindings;
}

EffectList *FX_getEffects() {
    FS_assertDirectory(effectFolder);
    DirectoryListing *entries = FS_listFiles(effectFolder);
    effectList.count = 0;

    for (int i = 0; i < entries->cnt && i < MAX_EFFECTS; i++) {
        if (strstr(entries->entries[i].fileName, PSTR(".elf")) != nullptr) {
            effectList.count++;
            strncpy(effectList.names[i], entries->entries[i].fileName, EFFECT_NAME_LENGTH);
        }
    }

    return &effectList;
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