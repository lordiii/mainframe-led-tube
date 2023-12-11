#include <effects.h>
#include <globals.h>
#include <effects_lib.h>
#include <OctoWS2811.h>
#include <main.h>

#include "games/games.h"

// Setup LEDs
const byte pinList[LED_STRIP_AMOUNT] = LED_PINS;
DMAMEM int displayMemory[LED_BUFFER_SIZE] = {0};
int drawingMemory[LED_BUFFER_SIZE] = {0};

const int ZeroBuf[LED_BUFFER_SIZE] = {0};

OctoWS2811 leds = OctoWS2811(LED_PER_STRIP, displayMemory, drawingMemory, LED_CONFIGURATION, LED_STRIP_AMOUNT, pinList);
EffectState *state = new EffectState;

const int effectCount = 10;
Effect effects[effectCount] = {
        {"off",            &effectOff},
        {"test-led",       &effectTestLEDs},
        {"strobe",         &effectStrobe},
        {"rainbow-strobe", &effectRainbowStrobe},
        {"police",         &effectPolice},
        {"solid-white",    &effectSolidWhite},
        {"beam",           &effectBeam},
        {"gol",            &effectGOL,    &initializeGOLData},
        {"tetris",         &effectTetris, &initializeTetris, &onTetrisButtonPress, &onTetrisAnalogButton}
};

void initOctoWS2811() {
    leds.begin();
    leds.show();

    state->lastFrameChange = 0;
    state->brightness = 0.50f;
}

void setBrightness(float value) {
    state->brightness = value;

    for (int i = 0; i < leds.numPixels(); i++) {
        leds.setPixel(i, applyBrightness(leds.getPixel(i)));
    }
}

void setCurrentEffect(Effect *effect) {
    taskRenderLeds->end();

    // Wait for current frame
    while (leds.busy()) {}

    // Reset State
    state->lastFrameChange = 0;
    state->current = effect;
    memset(state->data, 0, sizeof(EffectData));

    // Clear display
    fillLEDs(0x000000);

    // Reset Data
    if (state->current != nullptr) {
        state->current->resetData();
    }

    taskRenderLeds->begin(renderFrame, LED_FRAMES_PER_SECOND);
}

void renderFrame() {
    unsigned long delta = millis() - state->lastFrameChange;

    if ((!state->halt && state->slowRate == 0) || state->singleStep ||
        (state->slowRate != 0 && delta > state->slowRate && !state->halt)) {
        bool updated = false;

        if (state->current != nullptr) {
            updated = state->current->callback(delta);
        }

        if (updated) {
            state->lastFrameChange = millis();
        }

        state->singleStep = false;
    }

    if (!leds.busy()) {
        leds.show();
    }
}

//
//
// LED Test Effects
//
//
bool effectTestLEDs(unsigned long delta) {
    EffectTestAll *data = &state->data->testAll;

    if (delta > 250) {
        switch (data->lastColor) {
            case RED:
                data->lastColor = GREEN;
                break;
            case GREEN:
                data->lastColor = BLUE;
                break;
            case BLUE:
            default:
                data->lastColor = RED;
                break;
        }

        fillLEDs(applyBrightness(data->lastColor));

        return true;
    }

    return false;
}

//
//
// Strobe Effects
//
//
bool effectStrobe(unsigned long delta) {
    EffectStrobe *data = &state->data->strobe;

    if (delta > 5 && !data->toggle) {
        fillLEDs(0xFFFFFF);
        data->toggle = true;

        return true;
    }

    if (delta > 50 && data->toggle) {
        fillLEDs(0x000000);
        data->toggle = false;

        return true;
    }

    return false;
}

bool effectRainbowStrobe(unsigned long delta) {
    EffectRainbowStrobe *data = &state->data->rainbowStrobe;

    if (delta > 5 && !data->toggle) {
        data->toggle = true;

        switch (data->lastColor) {
            case RED:
                data->lastColor = GREEN;
                break;
            case GREEN:
                data->lastColor = BLUE;
                break;
            case BLUE:
            default:
                data->lastColor = RED;
                break;
        }

        fillLEDs(data->lastColor);

        return true;
    }

    if (delta > 25 && data->toggle) {
        fillLEDs(0x000000);
        data->toggle = false;

        return true;
    }

    return false;
}

bool effectPolice(unsigned long delta) {
    EffectPolice *data = &state->data->police;

    if (delta > 25 && data->blinkTimes >= 2) {
        if (data->lastColor == BLUE) {
            data->lastColor = RED;
        } else {
            data->lastColor = BLUE;
        }

        fillLEDs(0x000000);

        data->blinkTimes = 0;
        return true;
    }

    if (delta > 50 && data->blinkTimes < 2) {
        if (data->toggle) {
            data->toggle = false;
            data->blinkTimes++;
            fillLEDs(0x000000);
        } else {
            data->toggle = true;
            fillLEDs(data->lastColor);
        }

        return true;
    }

    return false;
}

//
//
// Miscellanous
//
//
bool effectSolidWhite(unsigned long delta) {
    fillLEDs(0xFFFFFF);
    return true;
}

bool effectOff(unsigned long delta) {
    fillLEDs(0);
    return true;
}

bool effectBeam(unsigned long delta) {
    EffectBeam *data = &state->data->beam;

    if (delta > 30) {
        fadeAllToBlack(100);

        setRingColor(data->lastRing, 0xFF00FF);
        data->lastRing++;

        return true;
    }

    return false;
}

//
//
// Game of life
//
//
bool effectGOL(unsigned long delta) {
    EffectGOL *data = &state->data->gol;

    if (delta > 10) {
        for (int i = 0; i < LED_TOTAL_RINGS; i++) {
            for (int j = 0; j < LED_PER_RING; j++) {
                data->state[calculatePixelId(i, j)] = calculateGOLCell(i, j);
            }
        }

        for (int i = 0; i < LED_TOTAL_RINGS; i++) {
            for (int j = 0; j < LED_PER_RING; j++) {
                setPixelColor(i, j, data->state[calculatePixelId(i, j)] ? 0xFFFFFF : 0x000000);
            }
        }

        return true;
    }

    return false;
}

//
//
// Tetris
//
//
bool effectTetris(unsigned long delta) {
    EffectTetris *data = &state->data->tetris;
    const bool forceMovement = delta > (unsigned long) max(500 - ((data->score / 100) * 25), 100);

    switch (data->state) {
        case RUNNING: {
            if (data->shape->placed) {
                addTetrisShape();
                if (!eliminateRings()) {
                    if (!renderShape(data->shape->array, data->shape->ring, data->shape->pixel,
                                     applyBrightness(data->shape->color))) {
                        data->state = ENDING;
                    }
                }
            } else {
                renderShape(data->shape->array, data->shape->ring, data->shape->pixel, 0);

                if (forceMovement || ((millis() - data->lastInput) > 100 && controller->dpadDown)) {
                    data->shape->ring--;
                    if (!renderShape(data->shape->array, data->shape->ring, data->shape->pixel, data->shape->color,
                                     true)) {
                        data->shape->ring++;
                        data->shape->placed = true;
                    }
                }

                if (!renderShape(data->shape->array, data->shape->ring, data->shape->pixel,
                                 applyBrightness(data->shape->color))) {
                    data->state = ENDING;
                }
            }

            return forceMovement;
        }
        case ENDING: {
            if (delta > 62) {
                setRingColor(LED_TOTAL_RINGS - data->lastEndAnimationRing, applyBrightness(0xFF0000));
                setRingColor(data->lastEndAnimationRing, applyBrightness(0xFF0000));
                data->lastEndAnimationRing++;

                if (data->lastEndAnimationRing >= LED_TOTAL_RINGS) {
                    data->state = WAITING;
                }
                return true;
            }
            break;
        }
        case WAITING: {
            bool startNew = getPixelColor(0, 0) == 0;

            if (startNew && delta > 1000) {
                initializeTetris();
                data->state = RUNNING;
                return true;
            } else if (delta > 10 && !startNew) {
                fadeAllToBlack(255);
                return true;
            }

            break;
        }
        case RINGS: {
            if (delta > 10) {
                int done = 1;
                for (int i = 1; i < (LED_TOTAL_RINGS - 1); i++) {
                    if (data->ringStatus[i]) {
                        fadeRingToBlack(i, 225);

                        int src = calculatePixelId(i, 0);
                        if (memcmp(drawingMemory + src, ZeroBuf, LED_PER_RING) == 0) {
                            moveArea(i + 1, 0, i, 0, (LED_TOTAL_RINGS - 2 - i) * LED_PER_RING);
                            memmove(data->ringStatus + i, data->ringStatus + i + 1, LED_TOTAL_RINGS - i);
                            data->ringStatus[LED_TOTAL_RINGS - 1] = false;
                        }
                    } else {
                        done++;
                    }
                }

                if (done >= (LED_TOTAL_RINGS - 1)) {
                    data->state = RUNNING;

                    renderShape(data->shape->array, data->shape->ring, data->shape->pixel,
                                applyBrightness(data->shape->color));
                }

                return true;
            }
            break;
        }
    }

    return false;
}