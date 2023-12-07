#ifndef FIRMWARE_TETRIS_H
#define FIRMWARE_TETRIS_H

#include "effects.h"

struct Shape
{
    uint8_t array[3][4];
    int ring;
    int pixel;
    bool placed = false;
    int currentColor;
};

struct EffectTetris
{
    Shape *currentShape;
    int score;
};

bool effectTetris(unsigned long delta);
void initializeTetris();

void renderShape(Shape *shape, int color);
void addTetrisShape();
void processMovement(bool forceMovement);
void eliminateRings();

#endif //FIRMWARE_TETRIS_H
