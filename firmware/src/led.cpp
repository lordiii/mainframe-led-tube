#include "led.h"
#include "effects/_effects.h"
#include "globals.h"

#include <OctoWS2811.h>
#include <Arduino.h>

#define LED_STRIP_AMOUNT (LED_TOTAL_RINGS / LED_RINGS_PER_SEGMENT)
#define BYTES_PER_LED 3
#define LED_BUFFER_SIZE ((LED_PER_RING * LED_TOTAL_RINGS * BYTES_PER_LED) / 4) // 3 Bytes per LED fitted into (4 byte) integers

const unsigned char pinList[LED_STRIP_AMOUNT] = LED_PINS;
DMAMEM int displayMemory[LED_BUFFER_SIZE] = {};
int drawingMemory[LED_BUFFER_SIZE] = {};

OctoWS2811 leds = OctoWS2811(LED_PER_RING * LED_RINGS_PER_SEGMENT, displayMemory, drawingMemory,
                             WS2811_GRB | WS2811_800kHz, LED_STRIP_AMOUNT, pinList);

LED_Ring LED_RINGS[LED_TOTAL_RINGS];
LED_Pixel LEDS[LED_TOTAL_AMOUNT];

// LED Render Task
IntervalTimer renderTask = IntervalTimer();

void LED_init() {
    leds.begin();
    leds.show();

    auto led_ptr = (LED_RGB *) (void *) drawingMemory;
    for (int i = 0; i < LED_TOTAL_AMOUNT; i++) {
        LED_Ring *ring = LED_getRing(i / LED_PER_RING);

        if (i % LED_PER_RING == 0) {
            ring->i = i / LED_PER_RING;

            ring->start = LEDS + i;
            ring->previous = &LED_RINGS[(ring->i - 1) % LED_TOTAL_RINGS];
            ring->next = &LED_RINGS[(ring->i + 1) % LED_TOTAL_RINGS];
        }

        LEDS[i].i = i % LED_PER_RING;

        LEDS[i].color = led_ptr + i;
        LEDS[i].ring = ring;

        LEDS[i].previous = LED_getPixel(ring, i - 1);
        LEDS[i].next = LED_getPixel(ring, i + 1);
    }

    renderTask.begin(LED_render, LED_FRAMES_PER_SECOND);
}

LED_Ring *LED_getRing(int ring) {
    if (ring < 0 || ring >= LED_TOTAL_RINGS) {
        return nullptr;
    }

    return LED_RINGS + ring;
}

LED_Pixel *LED_getPixel(LED_Ring *ring, int pixel) {
    pixel = (pixel >= 0) ? pixel : (LED_PER_RING + (pixel % -LED_PER_RING));
    pixel = pixel % LED_PER_RING;

    return ring->start + pixel;
}

void LED_render() {
    EffectState *state = FX_getState();
    if (state->current == nullptr) {
        LED_clear();
        leds.show();
        return;
    }

    unsigned long delta = millis() - state->lastFrameChange;

    if ((!state->halt && state->slowRate == 0) || state->singleStep ||
        (state->slowRate != 0 && delta > state->slowRate && !state->halt)) {
        bool updated = false;

        if (state->current != nullptr) {
            updated = state->current->render(delta);
        }

        if (updated) {
            state->lastFrameChange = millis();
        }

        state->singleStep = false;
    }

    leds.show();
}

void LED_setColor(LED_RGB *src, LED_Pixel *dst) {
    memcpy(dst->color, src, sizeof(LED_RGB));
}

void LED_fillRing(LED_RGB *src, LED_Ring *ring) {
    if (ring == nullptr) {
        return;
    }

    LED_fillSection(src, ring->start->color, ring->next->start->color);
}

void LED_fillSection(LED_RGB *src, LED_RGB *start, LED_RGB *end) {
    while (start < end) {
        memcpy(start++, src, sizeof(LED_RGB));
    }
}

void LED_clear() {
    memset(drawingMemory, 0, sizeof(drawingMemory));
}

void LED_move(LED_RGB *src_s, LED_RGB *src_e, LED_RGB *dst) {
    const int size = src_e - src_s;
    if (size < 0) {
        return;
    }

    memmove(dst, src_s, size);
}