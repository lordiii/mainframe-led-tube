//
// Created by gregor on 12/7/23.
//

#ifndef FIRMWARE_TETRIS_H
#define FIRMWARE_TETRIS_H

#include "effects.h"

struct CollisionBox
{
    int left;
    int right;
    int bottom;
    int up;
};

struct Shape
{
    uint8_t array[3][4];
    int ring;
    int pixel;
    CollisionBox colission;
    bool placed = false;
    int currentColor;
};

struct EffectTetris
{
    Shape *currentShape;
};

bool effectTetris(unsigned long delta);
void initializeTetris();

void renderShape(Shape *shape, int color);
void addTetrisShape();
void processMovement(bool forceMovement);

#endif //FIRMWARE_TETRIS_H
