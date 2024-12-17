#ifndef TETRIS_H
#define TETRIS_H

#include "_effects.h"
#include "enum.h"
#include "globals.h"
#include "led.h"

#define TETRIS_MAX_SIZE 4

typedef struct TShape {
    unsigned char array[TETRIS_MAX_SIZE][TETRIS_MAX_SIZE];
    int ring;
    int pixel;
    bool placed;
    LED_RGB *color;
} TShape;

enum TState {
    RUNNING,
    ENDING,
    WAITING,
    RINGS
};

class FX_Tetris : FX {
public:
    FX_Tetris() : FX("tetris") {
    }

    bool render(unsigned long delta) override;

    void resetData() override;

    bool onButton(GP_BUTTON button) override;

    bool onAnalogButton(GP_BUTTON button, int value) override;

private:
    TShape current_shape = {};
    int score = 0;
    TState state = {};
    int lastEndAnimationRing = 0;
    unsigned int lastInput = 0;
    unsigned short lastRotation = 0;
    bool ringStatus[LED_TOTAL_RINGS] = {};

    bool renderShape(unsigned char current_shape[TETRIS_MAX_SIZE][TETRIS_MAX_SIZE], int currentRing, int currentPixel,
                     LED_RGB *color, bool testRun);

    void setTetrisShape(TShape *shape);

    bool eliminateRings();

    void rotateFrame(bool clockwise, TShape *shape = nullptr);

    void rotateShape(TShape *shape, bool clockwise);
};


#endif
