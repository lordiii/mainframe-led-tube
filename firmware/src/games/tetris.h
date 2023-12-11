#ifndef FIRMWARE_TETRIS_H
#define FIRMWARE_TETRIS_H

#define TETRIS_MAX_SIZE 4

#include <effects.h>

struct Shape {
    uint8_t array[TETRIS_MAX_SIZE][TETRIS_MAX_SIZE];
    int ring;
    int pixel;
    bool placed = false;
    int color;
};

enum TetrisState {
    RUNNING,
    ENDING,
    WAITING,
    RINGS
};

struct EffectTetris {
    Shape *shape;
    int score;
    TetrisState state;
    int lastEndAnimationRing;
    uint32_t lastInput;
    uint16_t lastRotation;
    bool ringStatus[LED_TOTAL_RINGS];
};

bool effectTetris(unsigned long delta);

void initializeTetris();

bool renderShape(uint8_t shape[TETRIS_MAX_SIZE][TETRIS_MAX_SIZE], int currentRing, int currentPixel, int color,
                 bool testRun = false);

void addTetrisShape();

bool eliminateRings();

void rotateFrame(bool clockwise);

void rotateShape(Shape *shape, bool clockwise);

void onTetrisButtonPress(Button button);

void onTetrisAnalogButton(Button button, uint16_t value);

#endif //FIRMWARE_TETRIS_H
