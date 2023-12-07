#include <effects.h>
#include <effects_lib.h>
#include "gol.h"

void addCopperhead(int centerRing, int centerPixel)
{
    EffectGOL *data = &state->data->gol;
    
    data->state[calculatePixelId(centerRing + 1, centerPixel)] = true;
    data->state[calculatePixelId(centerRing, centerPixel)] = true;
    data->state[calculatePixelId(centerRing - 1, centerPixel)] = true;
    data->state[calculatePixelId(centerRing - 2, centerPixel)] = true;

    data->state[calculatePixelId(centerRing + 1, centerPixel - 2)] = true;
    data->state[calculatePixelId(centerRing - 2, centerPixel - 2)] = true;

    data->state[calculatePixelId(centerRing + 2, centerPixel - 3)] = true;
    data->state[calculatePixelId(centerRing, centerPixel - 3)] = true;
    data->state[calculatePixelId(centerRing - 1, centerPixel - 3)] = true;
    data->state[calculatePixelId(centerRing - 3, centerPixel - 3)] = true;

    data->state[calculatePixelId(centerRing + 2, centerPixel - 4)] = true;
    data->state[calculatePixelId(centerRing - 3, centerPixel - 4)] = true;

    data->state[calculatePixelId(centerRing + 2, centerPixel - 6)] = true;
    data->state[calculatePixelId(centerRing - 3, centerPixel - 6)] = true;

    data->state[calculatePixelId(centerRing + 2, centerPixel - 7)] = true;
    data->state[calculatePixelId(centerRing + 1, centerPixel - 7)] = true;
    data->state[calculatePixelId(centerRing - 2, centerPixel - 7)] = true;
    data->state[calculatePixelId(centerRing - 3, centerPixel - 7)] = true;

    data->state[calculatePixelId(centerRing + 2, centerPixel - 8)] = true;
    data->state[calculatePixelId(centerRing + 1, centerPixel - 8)] = true;
    data->state[calculatePixelId(centerRing, centerPixel - 8)] = true;
    data->state[calculatePixelId(centerRing - 1, centerPixel - 8)] = true;
    data->state[calculatePixelId(centerRing - 2, centerPixel - 8)] = true;
    data->state[calculatePixelId(centerRing - 3, centerPixel - 8)] = true;

    data->state[calculatePixelId(centerRing + 1, centerPixel - 9)] = true;
    data->state[calculatePixelId(centerRing - 2, centerPixel - 9)] = true;

    data->state[calculatePixelId(centerRing, centerPixel - 10)] = true;
    data->state[calculatePixelId(centerRing - 1, centerPixel - 10)] = true;

    data->state[calculatePixelId(centerRing, centerPixel - 11)] = true;
    data->state[calculatePixelId(centerRing - 1, centerPixel - 11)] = true;
}

void initializeGOLData()
{
    EffectGOL *data = &state->data->gol;

    addCopperhead(30, 15);
    addCopperhead(10, 0);
    addCopperhead(20, 10);
    addCopperhead(50, 22);

    for (int i = 0; i < LED_TOTAL_RINGS; i++)
    {
        for (int j = 0; j < LED_PER_RING; j++)
        {
            setPixelColor(i, j, data->state[calculatePixelId(i, j)] ? (random(100, 255)) : 0x000000);
        }
    }
}

bool calculateGOLCell(int ring, int pixel)
{
    int cnt = 0;
    bool populated = getPixelColor(ring, pixel) > 0;

    // Top Left
    if (getPixelColor(ring + 1, pixel - 1) > 0)
    {
        cnt++;
    }
    // Top Center
    if (getPixelColor(ring + 1, pixel) > 0)
    {
        cnt++;
    }
    // Top Right
    if (getPixelColor(ring + 1, pixel + 1) > 0)
    {
        cnt++;
    }
    // Center Left
    if (getPixelColor(ring, pixel - 1) > 0)
    {
        cnt++;
    }
    // Center Right
    if (getPixelColor(ring, pixel + 1) > 0)
    {
        cnt++;
    }
    // Bottom Left
    if (getPixelColor(ring - 1, pixel - 1) > 0)
    {
        cnt++;
    }
    // Bottom Center
    if (getPixelColor(ring - 1, pixel) > 0)
    {
        cnt++;
    }
    // Bottom Right
    if (getPixelColor(ring - 1, pixel + 1) > 0)
    {
        cnt++;
    }

    return ((cnt == 2 && populated) || cnt == 3);
}

