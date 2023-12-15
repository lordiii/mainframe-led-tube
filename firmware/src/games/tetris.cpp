#include <effects.h>
#include <effects_lib.h>
#include <globals.h>
#include "tetris.h"

#define TETRONIMO_COUNT 7
const uint8_t TETRONIMOS[TETRONIMO_COUNT][TETRIS_MAX_SIZE][TETRIS_MAX_SIZE] = {
        {{1, 1, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}};
const int TETRONIMO_COLORS[TETRONIMO_COUNT] = {0xEF476F, 0xFFFD166, 0x06D6A0, 0x118AB2, 0xFB5607, 0x8338EC, 0xFFCAD4};
uint8_t rotateBuffer[TETRIS_MAX_SIZE][TETRIS_MAX_SIZE] = {};

void initializeTetris() {
    EffectTetris *data = &state->data->tetris;

    data->shape.placed = true;
    data->shape.pixel = random(0, LED_PER_RING);

    data->score = 0;
    data->lastEndAnimationRing = 0;

    randomSeed(micros());

    for (int i = 0; i < LED_PER_RING; i++) {
        setPixelColor(0, i, applyBrightness(0xFF0000));
        setPixelColor(LED_TOTAL_RINGS - 1, i, applyBrightness(0xFF0000));
    }

    setTetrisShape(&data->shape);
}

bool renderShape(uint8_t shape[TETRIS_MAX_SIZE][TETRIS_MAX_SIZE], int currentRing, int currentPixel, int color,
                 bool testRun) {

    for (int ring = 0; ring < TETRIS_MAX_SIZE; ring++) {
        for (int pixel = 0; pixel < TETRIS_MAX_SIZE; pixel++) {

            if (shape[ring][pixel] == 1) {
                if (getPixelColor(currentRing + ring, currentPixel + pixel) == 0 || color == 0) {
                    if (!testRun) {
                        setPixelColor(currentRing + ring, currentPixel + pixel, color);
                    }
                } else {
                    return false;
                }
            }
        }
    }

    return true;
}

void setTetrisShape(Shape *shape) {
    uint8_t shapeID = random(0, TETRONIMO_COUNT);
    memcpy(shape->array, TETRONIMOS[shapeID], sizeof(TETRONIMOS[shapeID]));
    shape->ring = LED_TOTAL_RINGS - (TETRIS_MAX_SIZE + 1);
    shape->placed = false;
    shape->color = TETRONIMO_COLORS[shapeID];

    while (renderShape(shape->array, shape->ring, shape->pixel, shape->color, true)) {
        shape->ring++;
    }

    shape->ring--;
}

bool eliminateRings() {
    EffectTetris *data = &state->data->tetris;

    int total = 0;

    for (int i = 1; i < (LED_TOTAL_RINGS - 1); i++) {
        data->ringStatus[i] = true;

        for (int j = 0; j < LED_PER_RING; j++) {
            if (getPixelColor(i, j) == 0) {
                data->ringStatus[i] = false;
                break;
            }
        }

        if (data->ringStatus[i]) {
            total++;
        }
    }

    if (total > 0) {
        data->score += ((total * 10) * total);
        data->state = RINGS;
        return true;
    }

    return false;
}

void rotateShape(Shape *shape, bool clockwise) {
    // Rotate
    if (clockwise) {
        for (int i = 0; i < TETRIS_MAX_SIZE; i++) {
            for (int j = 0; j < TETRIS_MAX_SIZE; j++) {
                rotateBuffer[i][j] = shape->array[TETRIS_MAX_SIZE - j - 1][i];
            }
        }
    } else {
        for (int i = 0; i < TETRIS_MAX_SIZE; i++) {
            for (int j = 0; j < TETRIS_MAX_SIZE; j++) {
                rotateBuffer[i][j] = shape->array[j][TETRIS_MAX_SIZE - i - 1];
            }
        }
    }

    // Orient to bottom left
    bool exit = false;
    while (!exit) {
        for (int i = 0; i < TETRIS_MAX_SIZE; i++) {
            if (rotateBuffer[0][i] == 1) {
                exit = true;
                break;
            }
        }

        if (!exit) {
            for (int i = 0; i < (TETRIS_MAX_SIZE - 1); i++) {
                memcpy(rotateBuffer[i], rotateBuffer[i + 1], TETRIS_MAX_SIZE);
            }

            memset(rotateBuffer[TETRIS_MAX_SIZE - 1], 0, TETRIS_MAX_SIZE);
        }
    }

    exit = false;
    while (!exit) {
        for (int i = 0; i < TETRIS_MAX_SIZE; i++) {
            if (rotateBuffer[i][0] == 1) {
                exit = true;
                break;
            }
        }

        if (!exit) {
            for (int i = 0; i < TETRIS_MAX_SIZE; i++) {
                memcpy(rotateBuffer[i], rotateBuffer[i] + 1, TETRIS_MAX_SIZE - 1);
                rotateBuffer[i][TETRIS_MAX_SIZE - 1] = 0;
            }
        }
    }

    if (renderShape(rotateBuffer, shape->ring, shape->pixel, shape->color, true)) {
        memcpy(shape->array, rotateBuffer, sizeof(rotateBuffer));
    }
}

void rotateFrame(bool clockwise, Shape *shape) {
    if (shape != nullptr) {
        shape->pixel += clockwise ? -1 : 1;
    }

    int buffer[2];

    for (int i = 0; i < LED_TOTAL_RINGS; i++) {
        buffer[0] = getPixelColor(i, 0);

        for (int j = (LED_PER_RING - 1); j >= 0; j--) {
            buffer[1] = getPixelColor(i, clockwise ? j : -j);
            setPixelColor(i, clockwise ? j : -j, buffer[0]);

            buffer[0] = buffer[1];
        }
    }
}

void onTetrisButtonPress(Button button) {
    EffectTetris *data = &state->data->tetris;

    if (data->state == RUNNING) {
        stopAnimation();

        renderShape(data->shape.array, data->shape.ring, data->shape.pixel, 0);

        switch (button) {
            case SHOULDER_L1:
                rotateFrame(true);
                if (!renderShape(data->shape.array, data->shape.ring, data->shape.pixel, data->shape.color, true)) {
                    rotateFrame(false);
                }
                break;
            case SHOULDER_R1:
                rotateFrame(false);
                if (!renderShape(data->shape.array, data->shape.ring, data->shape.pixel, data->shape.color, true)) {
                    rotateFrame(true);
                }
                break;
            case BUTTON_X:
                rotateShape(&data->shape, true);
                break;
            case BUTTON_A:
                rotateShape(&data->shape, false);
                break;
            case DPAD_LEFT:
                data->shape.pixel -= 1;

                if (!renderShape(data->shape.array, data->shape.ring, data->shape.pixel,
                                 applyBrightness(data->shape.color), true)) {
                    data->shape.pixel += 1;
                }
                break;
            case DPAD_RIGHT:
                data->shape.pixel += 1;

                if (!renderShape(data->shape.array, data->shape.ring, data->shape.pixel,
                                 applyBrightness(data->shape.color), true)) {
                    data->shape.pixel -= 1;
                }
                break;
            default:
                break;
        }

        renderShape(data->shape.array, data->shape.ring, data->shape.pixel, applyBrightness(data->shape.color));
        if (button != MISC_START) {
            startAnimation();
        }
    }
}

void onTetrisAnalogButton(Button button, int value) {
    EffectTetris *data = &state->data->tetris;

    if (data->state == RUNNING) {
        if (data->lastRotation > ((1020 - value) / 50)) {
            stopAnimation();

            renderShape(data->shape.array, data->shape.ring, data->shape.pixel, 0);

            switch (button) {
                case BREAK:
                    rotateFrame(true, &data->shape);
                    break;
                case THROTTLE:
                    rotateFrame(false, &data->shape);
                    break;
                default:
                    break;
            }

            renderShape(data->shape.array, data->shape.ring, data->shape.pixel, applyBrightness(data->shape.color));

            startAnimation();
            data->lastRotation = 0;
        }

        data->lastRotation++;
    }
}