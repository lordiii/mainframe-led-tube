#include <OctoWS2811.h>

typedef bool (*EffectCallback)(unsigned long);

struct Effect
{
    String name = "";
    EffectCallback callback = nullptr;
    void (*resetData)() = []() {};
};

struct EffectStrobe
{
    bool toggle;
};

struct EffectRainbowStrobe
{
    bool toggle;
    int lastColor;
};

struct EffectTestAll
{
    int lastColor;
};

struct EffectPolice
{
    bool toggle;
    int lastColor;
    int blinkTimes;
};

union EffectData
{
    EffectTestAll testAll;
    EffectStrobe strobe;
    EffectRainbowStrobe rainbowStrobe;
    EffectPolice police;
};

struct EffectState
{
    Effect* current = nullptr;
    EffectData* data = new EffectData;
    unsigned int lastFrameChange = 0;
    float brightness = 0.75f;
};

extern OctoWS2811 leds;
extern EffectState *state;

extern Effect effects[];
extern const int effectCount;

void initOctoWS2811();
void renderFrame();
void setCurrentEffect(Effect *effect);
void setBrightness(float value);

bool effectTestLEDs(unsigned long delta);
bool effectOff(unsigned long delta);
bool effectStrobe(unsigned long delta);
bool effectRainbowStrobe(unsigned long delta);
bool effectPolice(unsigned long delta);
bool effectSolidWhite(unsigned long delta);
bool effectBeam(unsigned long delta);