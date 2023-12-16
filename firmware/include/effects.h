#ifndef FIRMWARE_EFFECTS_H
#define FIRMWARE_EFFECTS_H

#include <OctoWS2811.h>
#include <globals.h>

#define LED_STRIP_AMOUNT (LED_TOTAL_RINGS / LED_RINGS_PER_SEGMENT)
#define LED_PER_STRIP (LED_PER_RING * LED_RINGS_PER_SEGMENT)
#define LED_TOTAL_AMOUNT (LED_PER_RING * LED_TOTAL_RINGS)
#define LED_BYTES_PER_LED 3
#define LED_BUFFER_SIZE ((LED_PER_RING * LED_TOTAL_RINGS * LED_BYTES_PER_LED) / 4)

typedef bool (*EffectCallback)(unsigned long);

enum Button
{
    BUTTON_Y,
    BUTTON_B,
    BUTTON_A,
    BUTTON_X,
    BUTTON_TL,
    BUTTON_TR,
    DPAD_LEFT,
    DPAD_RIGHT,
    DPAD_UP,
    DPAD_DOWN,
    SHOULDER_L1,
    SHOULDER_L2,
    SHOULDER_R1,
    SHOULDER_R2,
    MISC_HOME,
    MISC_START,
    MISC_SELECT,
    MISC_SYSTEM,
    MISC_BACK,
    MISC_CAPTURE,
    THROTTLE,
    BREAK,
    STICK_L_X,
    STICK_L_Y,
    STICK_R_X,
    STICK_R_Y
};

struct Effect
{
    const char *name = "";
    EffectCallback callback = nullptr;
    void (*resetData)() = []() {};
    void (*onButtonPress)(Button button) = [](Button button){};
    void (*onAnalogButton)(Button button, int value) = [](Button button, int value){};
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
extern int displayMemory[LED_BUFFER_SIZE];
extern int drawingMemory[LED_BUFFER_SIZE];
extern const int ZeroBuf[LED_BUFFER_SIZE];
extern IntervalTimer *taskRenderLeds;

void initOctoWS2811();
void renderFrame();
void setCurrentEffect(Effect *effect);
void setBrightness(float value);
void startAnimation();
void stopAnimation();

bool effectTestLEDs(unsigned long delta);
bool effectOff(unsigned long delta);
bool effectStrobe(unsigned long delta);
bool effectRainbowStrobe(unsigned long delta);
bool effectPolice(unsigned long delta);
bool effectSolidWhite(unsigned long delta);
bool effectBeam(unsigned long delta);

#include "games/games.h"

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
    EffectPong pong;
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