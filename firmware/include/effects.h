#include <OctoWS2811.h>

typedef bool (*EFFECT_CALLBACK)(OctoWS2811, unsigned long);

void initLeds();
void renderFrame();
void setCurrentEffect(EFFECT_CALLBACK callback);
int applyBrightness(int color);
void setBrightness(float value);

bool effectTestLEDs(OctoWS2811 leds, unsigned long delta);
bool effectStrobe(OctoWS2811 leds, unsigned long delta);
bool effectRainbowStrobe(OctoWS2811 leds, unsigned long delta);
bool effectOff(OctoWS2811 leds, unsigned long delta);
bool effectPolice(OctoWS2811 leds, unsigned long delta);
bool effectSolidWhite(OctoWS2811 leds, unsigned long delta);