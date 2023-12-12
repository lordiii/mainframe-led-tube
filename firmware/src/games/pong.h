#ifndef FIRMWARE_PONG_H
#define FIRMWARE_PONG_H

#include <effects.h>
#include <globals.h>

struct PongPaddle {
    int ring;
    int pixel;
};

struct PongBall {
    int ring;
    int pixel;
};

struct EffectPong {
    PongPaddle paddle;
    PongBall ball;
};

void initializePong();

void rotatePongFrame(bool clockwise);

void renderPongPaddle(PongPaddle *paddle, int color);

void renderPongBall(PongBall *ball, int color);

#endif //FIRMWARE_PONG_H
