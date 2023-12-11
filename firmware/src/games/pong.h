#ifndef FIRMWARE_PONG_H
#define FIRMWARE_PONG_H

#include <effects.h>
#include <globals.h>

struct PongPaddle {
    int ring = LED_TOTAL_RINGS / 2;
    int pixel = 0;
};

struct PongBall {
    int ring = 0;
    int pixel = 0;
};

struct EffectPong {
    PongPaddle *paddle;
    PongBall *ball;
};

void rotatePongFrame(bool clockwise);

void renderPongPaddle(PongPaddle *paddle, int color);

void renderPongBall(PongBall *ball, int color);

#endif //FIRMWARE_PONG_H
