#ifndef EFFECTS_LIB_H
#define EFFECTS_LIB_H

// Color Constants
#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF

// Misc
int applyBrightness(int color);
int calculatePixelId(int ring, int pixel);

// Segments
void setRingColor(int ring, int color);
void fillLEDs(int color);
void clearLEDs();
void moveArea(int srcRing, int srcPixel, int dstRing, int dstPixel, int amount);

// Single Pixel
int getPixelColor(int ring, int pixel);
void setPixelColor(int ring, int pixel, int r, int g, int b);
void setPixelColor(int ring, int pixel, int color);

// Fade
void fadePixelToBlack(int ring, int pixel, unsigned char scale);
void fadeRingToBlack(int ring, unsigned char scale);
void fadeAllToBlack(unsigned char scale);

#endif
