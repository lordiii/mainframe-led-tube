#include "_effects.h"
#include "globals.h"
#include "tetris.h"
#include "led.h"
#include "gamepad.h"

#include <Entropy.h>
#include <Arduino.h>

#define TETRONIMO_COUNT 7
const uint8_t TETRONIMOS[TETRONIMO_COUNT][TETRIS_MAX_SIZE][TETRIS_MAX_SIZE] = {
        {{1, 1, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {{1, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}
};

LED_RGB *TETRONIMO_COLORS[TETRONIMO_COUNT] = {
        &Color_Coral, &Color_Green, &Color_Blue, &Color_Yellow, &Color_Violet, &Color_Crimson, &Color_Snow,
};
unsigned char rotateBuffer[TETRIS_MAX_SIZE][TETRIS_MAX_SIZE] = {};

bool FX_Tetris::render(unsigned long delta) {
    GP_Status *gamepad = GP_getState();

    const bool forceMovement = delta > (unsigned long) max(500 - ((this->score / 100) * 25), 50);

    switch (this->state) {
        case RUNNING: {
            if (this->current_shape.placed) {
                this->setTetrisShape(&this->current_shape);

                if (!this->eliminateRings()) {
                    if (!renderShape(this->current_shape.array, this->current_shape.ring, this->current_shape.pixel,
                                     this->current_shape.color, false)) {
                        this->state = ENDING;
                    }
                }
            } else {
                renderShape(this->current_shape.array, this->current_shape.ring, this->current_shape.pixel,
                            &Color_Black, false);

                if (forceMovement || ((millis() - this->lastInput) > 100 && gamepad->dpadDown)) {
                    this->current_shape.ring--;
                    if (!renderShape(this->current_shape.array, this->current_shape.ring, this->current_shape.pixel,
                                     this->current_shape.color, true)) {
                        this->current_shape.ring++;
                        this->current_shape.placed = true;
                    }
                }

                if (!renderShape(this->current_shape.array, this->current_shape.ring, this->current_shape.pixel,
                                 this->current_shape.color, false)) {
                    this->state = ENDING;
                }
            }

            return forceMovement;
        }
        case ENDING: {
            if (delta > 62) {
                LED_fillRing(Color_Red, LED_getRing(LED_TOTAL_RINGS - this->lastEndAnimationRing));
                LED_fillRing(Color_Red, LED_getRing(this->lastEndAnimationRing));

                this->lastEndAnimationRing++;

                if (this->lastEndAnimationRing >= LED_TOTAL_RINGS) {
                    this->state = WAITING;
                }
                return true;
            }
            break;
        }
        case WAITING: {
            LED_Pixel *pxl = LED_getPixel(LED_getRing(1), 0);
            bool startNew = pxl->color->R == 0 && pxl->color->G == 0 && pxl->color->B == 0;

            if (startNew && delta > 1000) {
                LED_clear();
                this->resetData();
                this->state = RUNNING;
                return true;
            } else if (delta > 10) {
                LED_clear(); // TODO: Clear via fade animation
                return true;
            }

            break;
        }
        case RINGS: {
            if (delta > 10) {
                int done = 1;
                for (int i = 1; i < (LED_TOTAL_RINGS - 1); i++) {
                    if (this->ringStatus[i]) {
                        LED_Ring *ring = LED_getRing(i);
                        LED_fillRing(Color_Black, ring); // TODO: Clear via fade animation

                        LED_move(ring->next->start, LED_getRing(LED_TOTAL_RINGS - 1)->start, ring->start);

                        // Move ring status
                        memmove(this->ringStatus + i, this->ringStatus + i + 1, (LED_TOTAL_RINGS - i) * sizeof(bool));

                        this->ringStatus[LED_TOTAL_RINGS - 1] = false;
                    } else {
                        done++;
                    }
                }

                if (done >= (LED_TOTAL_RINGS - 1)) {
                    this->state = RUNNING;

                    renderShape(this->current_shape.array, this->current_shape.ring, this->current_shape.pixel,
                                this->current_shape.color, false);
                }

                return true;
            }
            break;
        }
    }

    return false;
}

void FX_Tetris::resetData() {
    this->current_shape.placed = true;
    this->current_shape.pixel = 0;

    this->score = 0;
    this->lastEndAnimationRing = 0;

    this->state = RUNNING;

    LED_Ring *first = LED_getRing(0);
    LED_fillRing(Color_Red, first);
    LED_fillRing(Color_Red, LED_getRing(LED_TOTAL_RINGS));

    this->setTetrisShape(&this->current_shape);
}

bool FX_Tetris::renderShape(unsigned char shape[TETRIS_MAX_SIZE][TETRIS_MAX_SIZE], int currentRing, int currentPixel,
                            LED_RGB *color, bool testRun) {

    for (int ring = 0; ring < TETRIS_MAX_SIZE; ring++) {
        for (int pixel = 0; pixel < TETRIS_MAX_SIZE; pixel++) {
            if (shape[ring][pixel] == 1) {
                LED_Pixel *pxl = LED_getPixel(LED_getRing(currentRing + ring), currentPixel + pixel);

                if ((pxl->color->R == 0 && pxl->color->G == 0 && pxl->color->B == 0) ||
                    (color->R == 0 && color->G == 0 && color->B == 0)) {
                    if (!testRun) {
                        *pxl->color = *color;
                    }
                } else {
                    return false;
                }
            }
        }
    }

    return true;
}

void FX_Tetris::setTetrisShape(TShape *shape) {
    unsigned char shapeID = Entropy.random(0, TETRONIMO_COUNT);
    if (shapeID == shape->id) {
        return setTetrisShape(shape);
    }

    memcpy(shape->array, TETRONIMOS[shapeID], sizeof(TETRONIMOS[shapeID]));
    shape->ring = LED_TOTAL_RINGS - (TETRIS_MAX_SIZE + 1);
    shape->placed = false;
    shape->color = TETRONIMO_COLORS[shapeID];
    shape->id = shapeID;

    while (renderShape(shape->array, shape->ring, shape->pixel, shape->color, true)) {
        shape->ring++;
    }

    shape->ring--;
}

bool FX_Tetris::eliminateRings() {
    int total = 0;

    for (int i = 1; i < (LED_TOTAL_RINGS - 1); i++) {
        this->ringStatus[i] = true;

        LED_Ring *ring = LED_getRing(i);

        for (int j = 0; j < LED_PER_RING; j++) {
            LED_Pixel *pxl = LED_getPixel(ring, j);

            if (pxl->color->R == 0 && pxl->color->G == 0 && pxl->color->B == 0) {
                this->ringStatus[i] = false;
                break;
            }
        }

        if (this->ringStatus[i]) {
            total++;
        }
    }

    if (total > 0) {
        this->score += ((total * 10) * total);
        this->state = RINGS;
        return true;
    }

    return false;
}

void FX_Tetris::rotateShape(TShape *shape, bool clockwise) {
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
        for (auto &i: rotateBuffer) {
            if (i[0] == 1) {
                exit = true;
                break;
            }
        }

        if (!exit) {
            for (auto &i: rotateBuffer) {
                memcpy(i, i + 1, TETRIS_MAX_SIZE - 1);
                i[TETRIS_MAX_SIZE - 1] = 0;
            }
        }
    }

    memcpy(shape->array, rotateBuffer, sizeof(rotateBuffer));
    if (!renderShape(rotateBuffer, shape->ring, shape->pixel, shape->color, true)) {
        rotateShape(shape, clockwise);
    }
}

void moveShape(FX_Tetris *tetris, bool left) {
    tetris->current_shape.pixel += left ? -1 : 1;

    if (!tetris->renderShape(tetris->current_shape.array, tetris->current_shape.ring,
                             tetris->current_shape.pixel, tetris->current_shape.color, true)) {
        tetris->current_shape.pixel += left ? 1 : -1;
    }
}

void onRotateStationary(GP_BUTTON btn, GP_Status *gp, FX *effect) {
    auto *tetris = (FX_Tetris *) effect;

    EffectState *state = FX_getState();
    if (state->halt || tetris->state != RUNNING) {
        return;
    }

    int value = btn == BREAK ? gp->breakForce : gp->throttleForce;

    if (tetris->lastRotation > ((1100 - value) / 50)) {
        LED_animationStop();

        tetris->renderShape(tetris->current_shape.array, tetris->current_shape.ring,
                            tetris->current_shape.pixel, &Color_Black, false);

        switch (btn) {
            case BREAK:
                LED_rotateAll(true);
                if (!tetris->renderShape(tetris->current_shape.array, tetris->current_shape.ring,
                                         tetris->current_shape.pixel, tetris->current_shape.color, true)) {
                    tetris->current_shape.pixel--;
                }
                break;
            case THROTTLE:
                LED_rotateAll(false);
                if (!tetris->renderShape(tetris->current_shape.array, tetris->current_shape.ring,
                                         tetris->current_shape.pixel, tetris->current_shape.color, true)) {
                    tetris->current_shape.pixel++;
                }
                break;
            default:
                return;
        }

        tetris->renderShape(tetris->current_shape.array, tetris->current_shape.ring, tetris->current_shape.pixel,
                            tetris->current_shape.color, false);

        tetris->lastRotation = 0;

        LED_animationStart();
    }

    tetris->lastRotation++;
}

void onRotateShape(GP_BUTTON btn, GP_Status *gp, FX *effect) {
    auto *tetris = (FX_Tetris *) effect;
    EffectState *state = FX_getState();
    if (state->halt || tetris->state != RUNNING) {
        return;
    }

    LED_animationStop();

    tetris->renderShape(tetris->current_shape.array, tetris->current_shape.ring, tetris->current_shape.pixel,
                        &Color_Black, false);

    switch (btn) {
        case BUTTON_X:
            tetris->rotateShape(&tetris->current_shape, true);
            break;
        case BUTTON_A:
            tetris->rotateShape(&tetris->current_shape, false);
            break;
        default:
            break;
    }

    tetris->renderShape(tetris->current_shape.array, tetris->current_shape.ring, tetris->current_shape.pixel,
                        tetris->current_shape.color, false);

    LED_animationStart();
}

void onShapeMove(GP_BUTTON btn, GP_Status *gp, FX *effect) {
    auto *tetris = (FX_Tetris *) effect;
    EffectState *state = FX_getState();
    if (state->halt || tetris->state != RUNNING) {
        return;
    }

    LED_animationStop();

    tetris->renderShape(tetris->current_shape.array, tetris->current_shape.ring, tetris->current_shape.pixel,
                        &Color_Black, false);

    switch (btn) {
        case DPAD_LEFT:
            moveShape(tetris, true);
            break;
        case DPAD_RIGHT:
            moveShape(tetris, false);
            break;
        default:
            break;
    }

    tetris->renderShape(tetris->current_shape.array, tetris->current_shape.ring, tetris->current_shape.pixel,
                        tetris->current_shape.color, false);

    LED_animationStart();
}

bool FX_Tetris::registerKeybindings() {
    GP_registerKeybind(BREAK, onRotateStationary);
    GP_registerKeybind(THROTTLE, onRotateStationary);

    GP_registerKeybind(BUTTON_X, onRotateShape);
    GP_registerKeybind(BUTTON_A, onRotateShape);

    GP_registerKeybind(DPAD_LEFT, onShapeMove);
    GP_registerKeybind(DPAD_RIGHT, onShapeMove);

    return true;
}