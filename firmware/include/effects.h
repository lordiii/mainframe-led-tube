#include <OctoWS2811.h>

typedef bool (*EffectCallback)(unsigned long);

struct EffectState
{
    EffectCallback callback;
    void* effectData;
    unsigned int lastFrameChange;
    float brightness;
};

extern OctoWS2811 leds;
extern EffectState *state;

void initOctoWS2811();
void renderFrame();
void setCurrentEffect(EffectCallback callback);
void setBrightness(float value);

bool effectTestLEDs(unsigned long delta);
bool effectStrobe(unsigned long delta);
bool effectRainbowStrobe(unsigned long delta);
bool effectOff(unsigned long delta);
bool effectPolice(unsigned long delta);
bool effectSolidWhite(unsigned long delta);