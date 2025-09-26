#ifndef FX_H
#define FX_H

#include "led.h"


void FX2_init();

struct s_jmp_tbl {
	unsigned int version;
	void (*LED_fillRing)(LED_RGB, LED_Ring*);
    void (*LED_fillSection)(LED_RGB color, LED_Pixel *start, LED_Pixel *end);
    void (*LED_clear)();
    void (*LED_move)(LED_Pixel *src_s, LED_Pixel *src_e, LED_Pixel *dst);
} __attribute__((aligned(4)));

#endif
