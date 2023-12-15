#include <effects.h>
#include <effects_lib.h>
#include <globals.h>
#include <Arduino.h>

//
// Misc
//
int applyBrightness(int color) {
    uint8_t r = color >> 16;
    uint8_t g = color >> 8;
    uint8_t b = color;

    r *= state->brightness;
    g *= state->brightness;
    b *= state->brightness;

    return (r << 16) | (g << 8) | b;
}


int calculatePixelId(int ring, int pixel) {
    ring = abs(ring);
    pixel = (pixel > 0) ? pixel : LED_PER_RING + pixel;

    pixel = pixel % LED_PER_RING;
    ring = ring % LED_TOTAL_RINGS;

    return (ring * LED_PER_RING) + pixel;
}

//
// Segments
//
void setRingColor(int ring, int color) {
    int start = calculatePixelId(ring, 0);
    for (int i = start; i < (start + LED_PER_RING); i++) {
        leds.setPixel(i, color);
    }
}

void fillLEDs(int color) {
    for (int i = 0; i < LED_TOTAL_AMOUNT; i++) {
        leds.setPixel(i, color);
    }
}

void moveArea(int srcRing, int srcPixel, int dstRing, int dstPixel, int amount) {
    int src = calculatePixelId(srcRing, srcPixel) * LED_BYTES_PER_LED;
    int dst = calculatePixelId(dstRing, dstPixel) * LED_BYTES_PER_LED;

    memmove(((uint8_t *) drawingMemory) + dst, ((uint8_t *) drawingMemory) + src, amount * LED_BYTES_PER_LED);
}

//
// Single Pixels
//
int getPixelColor(int ring, int pixel) {
    return leds.getPixel(calculatePixelId(ring, pixel));
}

void setPixelColor(int ring, int pixel, int r, int g, int b) {
    leds.setPixel(calculatePixelId(ring, pixel), r, g, b);
}

void setPixelColor(int ring, int pixel, int color) {
    leds.setPixel(calculatePixelId(ring, pixel), color);
}

//
// Fade
//
void fadePixelToBlack(int ring, int pixel, uint8_t scale) {
    auto *pixels = (uint8_t *) drawingMemory;

    pixels += calculatePixelId(ring, pixel) * LED_BYTES_PER_LED;

    for (int i = 0; i < (LED_BYTES_PER_LED); i++) {
        *pixels = (((uint16_t) *pixels) * scale) >> 8;
        pixels++;
    }
}

void fadeRingToBlack(int ring, uint8_t scale) {
    auto *pixels = (uint8_t *) drawingMemory;

    pixels += calculatePixelId(ring, 0) * LED_BYTES_PER_LED;

    for (size_t i = 0; i < (LED_PER_RING * LED_BYTES_PER_LED); i++) {
        *pixels = (((uint16_t) *pixels) * scale) >> 8;
        pixels++;
    }
}

void fadeAllToBlack(uint8_t scale) {
    auto *pixels = (uint8_t *) drawingMemory;

    for (size_t i = 0; i < (LED_TOTAL_AMOUNT * LED_BYTES_PER_LED); i++) {
        *pixels = ((uint8_t) (((uint16_t) *pixels) * scale) >> 8);
        pixels++;
    }
}