#ifndef FIRMWARE_PONG_H
#define FIRMWARE_PONG_H

#include <effects.h>
#include <globals.h>

struct PongPaddle {
    int ring;
    int pixel;
    int color;
    int width;
};

struct PongBall {
    int ring;
    int pixel;
    int color;
    int size;
};

struct EffectPong {
    PongPaddle paddleP1;
    PongPaddle paddleP2;
    PongBall ball;
    int lastRotation;
    int lastP1Move;
    int lastP2Move;
    int directionX;
    int directionY;
    int moveX;
    int moveY;
};

void initializePong();

bool renderPongPaddle(int ring, int pixel, int width, int color, bool testRun = false);

bool renderPongBall(int ring, int pixel, int size, int color, bool testRun = false);

void onPongButtonPress(Button button);

void onPongAnalogButton(Button button, int value);

#endif //FIRMWARE_PONG_H
