#include <effects.h>
#include <effects_lib.h>
#include "pong.h"

void rotatePongFrame(bool clockwise) {
    EffectPong *data = &state->data->pong;

    data->paddle->pixel += clockwise ? -1 : 1;
    data->ball->pixel += clockwise ? -1 : 1;
}

void renderPongPaddle(PongPaddle *paddle, int color) {
    setPixelColor(paddle->ring + 3, paddle->pixel, applyBrightness(color));
    setPixelColor(paddle->ring + 2, paddle->pixel, applyBrightness(color));
    setPixelColor(paddle->ring + 1, paddle->pixel, applyBrightness(color));
    setPixelColor(paddle->ring + 0, paddle->pixel, applyBrightness(color));
    setPixelColor(paddle->ring - 1, paddle->pixel, applyBrightness(color));
    setPixelColor(paddle->ring - 2, paddle->pixel, applyBrightness(color));
}

void renderPongBall(PongBall *ball, int color) {
    setPixelColor(ball->ring + 1, ball->pixel + 1, applyBrightness(color));
    setPixelColor(ball->ring + 1, ball->pixel, applyBrightness(color));
    setPixelColor(ball->ring + 0, ball->pixel + 1, applyBrightness(color));
    setPixelColor(ball->ring + 0, ball->pixel, applyBrightness(color));
}