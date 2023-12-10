#ifndef FIRMWARE_TETRIS_H
#define FIRMWARE_TETRIS_H

#define TETRIS_MAX_SIZE 4

#include "effects.h"

struct Shape
{
    uint8_t array[TETRIS_MAX_SIZE][TETRIS_MAX_SIZE];
    int ring;
    int pixel;
    bool placed = false;
    int color;
    String name;
};

enum TetrisState
{
    RUNNING,
    ENDING,
    WAITING
};

struct EffectTetris
{
    Shape *shape;
    int score;
    TetrisState state;
    int lastEndAnimationRing;
    uint32_t lastInput;
    uint32_t lastRotation;
};

bool effectTetris(unsigned long delta);
void initializeTetris();

bool renderShape(uint8_t shape[TETRIS_MAX_SIZE][TETRIS_MAX_SIZE], int currentRing, int currentPixel, int color, bool testRun = false);
void addTetrisShape();
bool processMovement(bool forceMovement);
void eliminateRings();
void rotateFrame(bool clockwise);
void rotateShape(Shape *shape);

#endif //FIRMWARE_TETRIS_H
