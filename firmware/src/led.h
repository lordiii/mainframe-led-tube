#ifndef LED_H
#define LED_H

#include "colors.h"

struct LED_Ring;
struct LED_Pixel;

typedef struct LED_Pixel {
    int i;

    LED_RGB *color;

    LED_Ring *ring;
    LED_Pixel *previous;
    LED_Pixel *next;
} LED_Pixel;

typedef struct LED_Ring {
    int i;
    LED_Pixel *start;
    LED_Ring *previous;
    LED_Ring *next;
} LED_Ring;


void LED_init();

LED_Ring *LED_getRing(int ring);

LED_Pixel *LED_getPixel(LED_Ring *ring, int pixel);

void LED_render();

void LED_animationStop();

void LED_animationStart();

void LED_setColor(LED_RGB *src, LED_Pixel *dst);

void LED_fillRing(LED_RGB *src, LED_Ring *ring);

void LED_fillSection(LED_RGB *src, LED_Pixel *start, LED_Pixel *end);

void LED_clear();

void LED_move(LED_Pixel *src_s, LED_Pixel *src_e, LED_Pixel *dst);

bool LED_renderRequested();

void LED_rotateAll(bool clockwise);

void LED_rotateRing(LED_Ring *ring, bool clockwise);

#endif
