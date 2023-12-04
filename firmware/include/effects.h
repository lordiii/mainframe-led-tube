#include <OctoWS2811.h>

typedef bool (*EffectCallback)(unsigned long);

struct Effect
{
    String name;
    EffectCallback callback;
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

extern Effect **effects;
extern int effectCount;

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
bool effectBeam(unsigned long delta);