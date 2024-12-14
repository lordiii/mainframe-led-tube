#ifndef TETRIS_H
#define TETRIS_H

#include "_effects.h"
#include "enum.h"

#define TETRIS_MAX_SIZE 4

struct Shape {
    unsigned char array[TETRIS_MAX_SIZE][TETRIS_MAX_SIZE];
    int ring;
    int pixel;
    bool placed;
    int color;
};

enum TetrisState {
    RUNNING,
    ENDING,
    WAITING,
    RINGS
};

struct EffectTetris {
    Shape shape;
    int score;
    TetrisState state;
    int lastEndAnimationRing;
    unsigned int lastInput;
    unsigned short lastRotation;
    bool ringStatus[LED_TOTAL_RINGS];
};

bool renderTetrisFrame(unsigned long delta);

void initializeTetris();

bool renderShape(unsigned char shape[TETRIS_MAX_SIZE][TETRIS_MAX_SIZE], int currentRing, int currentPixel, int color,
                 bool testRun = false);

void setTetrisShape(Shape *shape);

bool eliminateRings();

void rotateFrame(bool clockwise, Shape *shape = nullptr);

void rotateShape(Shape *shape, bool clockwise);

void onTetrisButtonPress(GP_BUTTON button);

void onTetrisAnalogButton(GP_BUTTON button, int value);

#endif
