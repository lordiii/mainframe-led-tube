#include <effects.h>
#include <effects_lib.h>
#include <main.h>
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

    if (data->shape == nullptr) {
        data->shape = new Shape;
        data->shape->placed = true;
    }

    data->score = 0;
    data->lastEndAnimationRing = 0;
    data->shape->pixel = random(0, LED_PER_RING);

    randomSeed(micros());

    for (int i = 0; i < LED_PER_RING; i++) {
        setPixelColor(0, i, applyBrightness(0xFF0000));
        setPixelColor(LED_TOTAL_RINGS - 1, i, applyBrightness(0xFF0000));
    }

    addTetrisShape();
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

void addTetrisShape() {
    EffectTetris *data = &state->data->tetris;

    uint8_t shapeID = random(0, TETRONIMO_COUNT);
    memcpy(data->shape->array, TETRONIMOS[shapeID], sizeof(TETRONIMOS[shapeID]));
    data->shape->ring = LED_TOTAL_RINGS - (TETRIS_MAX_SIZE + 1);
    data->shape->placed = false;
    data->shape->color = TETRONIMO_COLORS[shapeID];

    while (renderShape(data->shape->array, data->shape->ring, data->shape->pixel, data->shape->color, true)) {
        data->shape->ring++;
    }

    data->shape->ring--;
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
        state->data->tetris.score += ((total * 10) * total);
        data->state = RINGS;
        return true;
    }

    return false;
}

void rotateShape(Shape *shape) {
    for (int i = 0; i < TETRIS_MAX_SIZE; i++) {
        for (int j = 0; j < TETRIS_MAX_SIZE; j++) {
            rotateBuffer[i][j] = shape->array[TETRIS_MAX_SIZE - j - 1][i];
        }
    }

    bool exit = false;
    while (!exit)
    {
        for (int i = 0; i < TETRIS_MAX_SIZE; i++)
        {
            if (rotateBuffer[0][i] == 1)
            {
                exit = true;
                break;
            }
        }

        if (!exit)
        {
            for (int i = 0; i < (TETRIS_MAX_SIZE - 1); i++)
            {
                memcpy(rotateBuffer[i], rotateBuffer[i + 1], TETRIS_MAX_SIZE);
            }

            memset(rotateBuffer[TETRIS_MAX_SIZE - 1], 0, TETRIS_MAX_SIZE);
        }
    }

    exit = false;
    while (!exit)
    {
        for (int i = 0; i < TETRIS_MAX_SIZE; i++)
        {
            if (rotateBuffer[i][0] == 1)
            {
                exit = true;
                break;
            }
        }

        if (!exit)
        {
            for (int i = 0; i < TETRIS_MAX_SIZE; i++)
            {
                memcpy(rotateBuffer[i], rotateBuffer[i] + 1, TETRIS_MAX_SIZE - 1);
                rotateBuffer[i][TETRIS_MAX_SIZE - 1] = 0;
            }
        }
    }

    if (renderShape(rotateBuffer, shape->ring, shape->pixel, shape->color, true))
    {
        memcpy(shape->array, rotateBuffer, sizeof(rotateBuffer));
    }
}

void rotateFrame(bool clockwise, bool moveShape) {
    EffectTetris *data = &state->data->tetris;

    if (moveShape) {
        data->shape->pixel += clockwise ? -1 : 1;
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
        state->halt = true;
        renderShape(data->shape->array, data->shape->ring, data->shape->pixel, 0);

        switch (button) {
            case SHOULDER_L1:
                rotateFrame(true, false);
                if (!renderShape(data->shape->array, data->shape->ring, data->shape->pixel, data->shape->color, true)) {
                    rotateFrame(false, false);
                }
                break;
            case SHOULDER_R1:
                rotateFrame(false, false);
                if (!renderShape(data->shape->array, data->shape->ring, data->shape->pixel, data->shape->color, true)) {
                    rotateFrame(true, false);
                }
                break;
            case BUTTON_X:
                rotateShape(data->shape);
                break;
            case BUTTON_A:
                rotateShape(data->shape);
                break;
            case DPAD_LEFT:
                data->shape->pixel -= 1;

                if (!renderShape(data->shape->array, data->shape->ring, data->shape->pixel,
                                 applyBrightness(data->shape->color), true)) {
                    data->shape->pixel += 1;
                }
                break;
            case DPAD_RIGHT:
                data->shape->pixel += 1;

                if (!renderShape(data->shape->array, data->shape->ring, data->shape->pixel,
                                 applyBrightness(data->shape->color), true)) {
                    data->shape->pixel -= 1;
                }
                break;
            default:
                break;
        }

        renderShape(data->shape->array, data->shape->ring, data->shape->pixel, applyBrightness(data->shape->color));
        if (button != MISC_START) {
            state->halt = false;
        }
    }
}

void onTetrisAnalogButton(Button button, uint16_t value) {
    EffectTetris *data = &state->data->tetris;

    if (data->state == RUNNING) {
        if (data->lastRotation > ((1020 - value) / 50)) {
            state->halt = true;

            renderShape(data->shape->array, data->shape->ring, data->shape->pixel, 0);

            switch (button) {
                case BREAK:
                    rotateFrame(true, true);
                    break;
                case THROTTLE:
                    rotateFrame(false, true);
                    break;
                default:
                    break;
            }

            renderShape(data->shape->array, data->shape->ring, data->shape->pixel, applyBrightness(data->shape->color));

            state->halt = false;
            data->lastRotation = 0;
        }

        data->lastRotation++;
    }
}