#include <effects.h>
#include <effects_lib.h>
#include <globals.h>
#include "pong.h"

void initializePong() {
    EffectPong *data = &state->data->pong;

    data->ball.ring = LED_TOTAL_RINGS / 2;
    data->ball.pixel = LED_PER_RING / 2;
    data->ball.color = 0xFFFFFF;
    data->ball.size = 1;

    data->paddleP1.width = 7;
    data->paddleP1.pixel = data->ball.pixel;
    data->paddleP1.ring = (LED_TOTAL_RINGS - 3);
    data->paddleP1.color = 0xFFFFFF;

    data->paddleP2.width = 7;
    data->paddleP2.pixel = data->ball.pixel;
    data->paddleP2.ring = 2;
    data->paddleP2.color = 0xFFFFFF;

    data->directionX = 0;
    data->directionY = 1;

    renderPongPaddle(data->paddleP1.ring, data->paddleP1.pixel, data->paddleP1.width, data->paddleP1.color);
    renderPongPaddle(data->paddleP2.ring, data->paddleP2.pixel, data->paddleP2.width, data->paddleP2.color);
}

bool renderPongPaddle(int ring, int pixel, int width, int color, bool testRun) {
    for (int i = -(width / 2); i <= (width / 2); i++) {
        if (getPixelColor(ring, pixel + i) == 0 || color == 0) {
            if (!testRun) {
                setPixelColor(ring, pixel + i, applyBrightness(color));
            }
        } else {
            return false;
        }
    }

    return true;
}

bool renderPongBall(int ring, int pixel, int size, int color, bool testRun) {
    for (int i = -(size / 2); i <= (size / 2); i++) {
        for (int j = -(size / 2); j <= (size / 2); j++) {
            if (getPixelColor(ring + i, pixel + j) == 0 || color == 0) {
                if (!testRun) {
                    setPixelColor(ring + i, pixel + j, color);
                }
            } else {
                return false;
            }
        }
    }

    return true;
}

void onPongButtonPress(Button button) {
    stopAnimation();

    if (button != MISC_START) {
        startAnimation();
    }
}

void onPongRotateFrame(Button button, int value) {
    EffectPong *data = &state->data->pong;

    if (data->lastRotation > ((1020 - value) / 50)) {

        renderPongBall(data->ball.ring, data->ball.pixel, data->ball.size, 0);
        renderPongPaddle(data->paddleP1.ring, data->paddleP1.pixel, data->paddleP1.width, 0);
        renderPongPaddle(data->paddleP2.ring, data->paddleP2.pixel, data->paddleP2.width, 0);

        switch (button) {
            case BREAK:
                data->paddleP1.pixel--;
                data->paddleP2.pixel--;
                data->ball.pixel--;
                break;
            case THROTTLE:
                data->paddleP1.pixel++;
                data->paddleP2.pixel++;
                data->ball.pixel++;
                break;
            default:
                break;
        }

        renderPongBall(data->ball.ring, data->ball.pixel, data->ball.size, data->ball.color);
        renderPongPaddle(data->paddleP1.ring, data->paddleP1.pixel, data->paddleP1.width, data->paddleP1.color);
        renderPongPaddle(data->paddleP2.ring, data->paddleP2.pixel, data->paddleP2.width, data->paddleP2.color);
    }

    data->lastRotation++;
}

void onPongPlayerMove(PongPaddle *player, int value, int *last) {
    if (*last > (509 - abs(value)) / 50) {
        renderPongPaddle(player->ring, player->pixel, player->width, 0);
        *last = 0;

        if (value > 0) {
            player->pixel++;
        } else {
            player->pixel--;
        }

        if (!renderPongPaddle(player->ring, player->pixel, player->width, player->color, true)) {
            if (value > 0) {
                player->pixel--;
            } else {
                player->pixel++;
            }
        }

        renderPongPaddle(player->ring, player->pixel, player->width, player->color);
    }

    *last = *last + 1;
}

void onPongAnalogButton(Button button, int value) {
    EffectPong *data = &state->data->pong;

    stopAnimation();
    switch (button) {
        case BREAK:
        case THROTTLE:
            onPongRotateFrame(button, value);
            break;
        case STICK_L_X:
            onPongPlayerMove(&data->paddleP1, value, &data->lastP1Move);
            break;
        case STICK_R_X:
            onPongPlayerMove(&data->paddleP2, value, &data->lastP2Move);
            break;
        default:
            break;
    }
    startAnimation();
}