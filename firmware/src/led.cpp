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
bool shouldRenderLeds = false;

LED_RGB buf;

bool LED_renderRequested() {
    return shouldRenderLeds;
}

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

    LED_animationStart();
}

void LED_animationStop() {
    renderTask.end();
}

void LED_requestFrame() {
    LED_animationStop();
    shouldRenderLeds = true;
}

void LED_animationStart() {
    renderTask.begin(LED_requestFrame, LED_FRAMES_PER_SECOND);
    while (leds.busy()) {}
}

LED_Ring *LED_getRing(int ring) {
    if (ring >= LED_TOTAL_RINGS) {
        ring = LED_TOTAL_RINGS - 1;
    } else if (ring < 0) {
        ring = 0;
    }

    return &LED_RINGS[ring];
}

LED_Pixel *LED_getPixel(LED_Ring *ring, int pixel) {
    pixel = (pixel >= 0) ? pixel : (LED_PER_RING + (pixel % -LED_PER_RING));
    pixel = pixel % LED_PER_RING;

    return &ring->start[pixel];
}

void LED_render() {
    EffectState *state = FX_getState();
    if (state->halt) {
        return;
    }

    unsigned long delta = millis() - state->lastFrameChange;

    if (state->current == nullptr) {
        if (delta > 100) {
            LED_clear();
            leds.show();
        }
        return;
    }


    if (state->slowRate == 0 || (state->slowRate != 0 && delta > state->slowRate)) {
        bool updated = false;

        if (state->current != nullptr) {
            updated = state->current->render(delta);
        }

        if (updated) {
            state->lastFrameChange = millis();
        }
    }

    leds.show();
    shouldRenderLeds = false;
    LED_animationStart();
}

void LED_fillRing(LED_RGB src, LED_Ring *ring) {
    LED_fillSection(src, ring->start, ring->start->previous);
}

void LED_fillSection(LED_RGB color, LED_Pixel *start, LED_Pixel *end) {
    while (start < &LEDS[LED_TOTAL_AMOUNT] && start >= &LEDS[0] && start->color <= end->color) {
        memcpy(start->color, &color, sizeof(LED_RGB));
        start++;
    }
}

void LED_rotateAll(bool clockwise) {
    for (int i = 0; i < LED_TOTAL_RINGS; i++) {
        LED_rotateRing(LED_getRing(i), clockwise);
    }
}

void LED_rotateRing(LED_Ring *ring, bool clockwise) {
    if (ring == nullptr) {
        return;
    }

    int size = (LED_PER_RING - 1) * sizeof(LED_RGB);

    if (clockwise) {
        LED_RGB tmp = *ring->start->color;
        memmove(ring->start->color, ring->start->next->color, size);
        *ring->start->previous->color = tmp;
    } else {
        LED_RGB tmp = *ring->start->previous->color;
        memmove(ring->start->next->color, ring->start->color, size);
        *ring->start->color = tmp;
    }
}

void LED_clear() {
    memset(drawingMemory, 0, sizeof(drawingMemory));
}

void LED_move(LED_Pixel *src_s, LED_Pixel *src_e, LED_Pixel *dst) {
    unsigned int size = ((int) src_e->color) - ((int) src_s->color);
    if (size <= 0 || size > (LED_TOTAL_AMOUNT * sizeof(LED_RGB))) {
        return;
    }

    memmove(dst->color, src_s->color, size);
}