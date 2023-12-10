#ifndef FIRMWARE_EFFECTS_LIB_H
#define FIRMWARE_EFFECTS_LIB_H

#include <Arduino.h>

// Color Constants
#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF

int applyBrightness(int color);
int fadePixelByAmount(int color, float amount);
void fillLEDs(int color);
int calculatePixelId(int ring, int pixel);
void setPixelColor(int ring, int pixel, int color);
void fadePixelToBlack(int ring, int pixel, uint8_t scale);
void fadeRingToBlack(int ring, uint8_t scale);
void fadeAllToBlack(uint8_t scale);
void setRingColor(int ring, int color);
int getPixelColor(int ring, int pixel);
void setPixelColor(int ring, int pixel, int r, int g, int b);
int randomColor();
uint8_t qsub8(uint8_t i, uint8_t j);
uint8_t qadd8(uint8_t i, uint8_t j);
int HeatColor(uint8_t temperature);
uint8_t scale8_video(uint8_t i, uint8_t scale);

#endif