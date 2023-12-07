#ifndef FIRMWARE_EFFECTS_H
#define FIRMWARE_EFFECTS_H

#include <OctoWS2811.h>
#include <globals.h>
#include "games/tetris.h"

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

struct EffectBeam
{
    int lastRing;
};

extern OctoWS2811 leds;

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

// Game of life
struct EffectGOL
{
    bool state[LED_TOTAL_RINGS * LED_PER_RING];
};

void initializeGOLData();
bool calculateGOLCell(int ring, int pixel);
bool effectGOL(unsigned long delta);

// Data Union
union EffectData
{
    EffectTestAll testAll;
    EffectStrobe strobe;
    EffectRainbowStrobe rainbowStrobe;
    EffectPolice police;
    EffectBeam beam;
    EffectGOL gol;
    EffectTetris tetris;
};

struct EffectState
{
    Effect* current = nullptr;
    EffectData* data = new EffectData;
    unsigned int lastFrameChange = 0;
    unsigned int slowRate = 0;
    float brightness = 0.75f;
    bool halt = false;
    bool singleStep = false;
};

extern EffectState *state;

#endif