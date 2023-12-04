#include <OctoWS2811.h>

typedef bool (*EffectCallback)(unsigned long);

struct Effect
{
    String name;
    EffectCallback callback;

    Effect* previous;
    Effect* next;
};

struct EffectList
{
    Effect *first;
    Effect *last;
    unsigned short length;
};

struct EffectState
{
    Effect* current;
    void* effectData;
    unsigned int lastFrameChange;
    float brightness;
};

extern OctoWS2811 leds;
extern EffectState *state;
extern EffectList *effects;

void initOctoWS2811();
void registerEffect(String name, EffectCallback callback);
void renderFrame();
void setCurrentEffect(Effect *effect);
void setBrightness(float value);

bool effectTestLEDs(unsigned long delta);
bool effectStrobe(unsigned long delta);
bool effectRainbowStrobe(unsigned long delta);
bool effectPolice(unsigned long delta);
bool effectSolidWhite(unsigned long delta);
bool effectSegmentTest(OctoWS2811 leds, unsigned long delta);