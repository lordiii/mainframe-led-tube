#include <effects.h>
#include <effects_lib.h>
#include <main.h>
#include "tetris.h"

#define TETRONIMO_COUNT 5
const uint8_t TETRONIMOS[TETRONIMO_COUNT][TETRIS_MAX_SIZE][TETRIS_MAX_SIZE] = {
    {{1, 1, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}};
const String TETRONIMO_NAMES[TETRONIMO_COUNT] = {
    "L",
    "CUBE",
    "LINE",
    "T",
    "S",
};

void initializeTetris()
{
    EffectTetris *data = &state->data->tetris;

    if(data->shape == nullptr) {
        data->shape = new Shape;
        data->shape->placed = true;
    }

    data->score = 0;
    data->lastEndAnimationRing = 0;

    addTetrisShape();

    randomSeed(micros());
}

bool renderShape(uint8_t shape[TETRIS_MAX_SIZE][TETRIS_MAX_SIZE], int currentRing, int currentPixel, int color, bool testRun)
{
    for (int ring = 0; ring < TETRIS_MAX_SIZE; ring++)
    {
        for (int pixel = 0; pixel < TETRIS_MAX_SIZE; pixel++)
        {
            if (shape[ring][pixel] == 1)
            {
                if(getPixelColor(currentRing + ring, currentPixel + pixel) == 0 || color == 0)
                {
                    if(!testRun) {
                        setPixelColor(currentRing + ring, currentPixel + pixel, color);
                    }
                } else {
                    return false;
                }
            }
        }
    }

    return true;
}

bool detectDownCollision(Shape *shape)
{
    for (int y = 0; y < TETRIS_MAX_SIZE; y++)
    {
        for (int x = 0; x < TETRIS_MAX_SIZE; x++)
        {
            if (shape->array[y][x] == 1)
            {
                if (getPixelColor(shape->ring - 1 + y, shape->pixel + x) > 0)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool detectSideCollision(Shape *shape, int sideforce)
{
    for (int x = 0; x < TETRIS_MAX_SIZE; x++)
    {
        for (int y = 0; y < TETRIS_MAX_SIZE; y++)
        {
            if (shape->array[y][x] == 1)
            {
                if (getPixelColor(shape->ring + y, shape->pixel + x + sideforce) > 0)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void addTetrisShape()
{
    EffectTetris *data = &state->data->tetris;

    uint8_t shapeID;
    do
    {
        shapeID = random(0, TETRONIMO_COUNT);
        memcpy(data->shape->array, TETRONIMOS[shapeID], sizeof(TETRONIMOS[shapeID]));
    } while (TETRONIMO_NAMES[shapeID].equals(data->shape->name));

    data->shape->name = TETRONIMO_NAMES[shapeID];

    data->shape->ring = LED_TOTAL_RINGS - (TETRIS_MAX_SIZE + 1);
    data->shape->pixel = random(0, LED_PER_RING);
    data->shape->placed = false;
    data->shape->color = randomColor();
}

bool processMovement(bool forceMovement)
{
    EffectTetris *data = &state->data->tetris;

    bool checkDown = controller->dpadDown || forceMovement;
    int checkSide = (controller->dpadLeft || controller->dpadRight) ? (controller->dpadLeft ? -1 : 1) : 0;

    bool collidesDown = false;
    if (checkDown) {
        collidesDown = detectDownCollision(data->shape);
    }

    bool collidesSide = false;
    if (checkSide != 0) {
        collidesSide = detectSideCollision(
                data->shape,
                controller->dpadLeft ? -1 : 1
        );
    }

    if(collidesDown)
    {
        data->shape->placed = true;
        data->shape->pixel += checkSide && !collidesSide ? checkSide : 0;
    } else {
        data->shape->ring -= checkDown ? 1 : 0;
        data->shape->pixel += !collidesSide && checkSide ? checkSide : 0;
    }
    
    for (int i = 0; i < TETRIS_MAX_SIZE; i++)
    {
        for (int j = 0; j < TETRIS_MAX_SIZE; j++)
        {
            if (data->shape->array[i][j] == 1 && (data->shape->ring + i) == 0)
            {
                data->shape->placed = true;
                break;
            }
        }
    }

    return checkSide != 0;
}

void eliminateRings()
{
    int total = 0;
    int i = 0;
    
    while(i < LED_TOTAL_RINGS) {
        bool match = true;

        for(int pixel = 0; pixel < LED_PER_RING; pixel++) {
            if(getPixelColor(i, pixel) == 0)
            {
                match = false;
                break;
            }
        }

        if(match)
        {
            total++;

            for(int ring = i; ring < (LED_TOTAL_RINGS - 1); ring++) {
                for(int pixel = 0; pixel < LED_PER_RING; pixel++) {
                    setPixelColor(ring, pixel, getPixelColor(ring + 1, pixel));
                 }
            }

            for(int pixel = 0; pixel < LED_PER_RING; pixel++) {
                setPixelColor((LED_TOTAL_RINGS - 1), pixel, 0);
            }
        } else {
            i++;
        }
    }

    state->data->tetris.score += ((total * 10) * total);
}

uint8_t rotateBuffer[TETRIS_MAX_SIZE][TETRIS_MAX_SIZE] = {};
void rotateShape(Shape *shape)
{
    for (int i = 0; i < TETRIS_MAX_SIZE; i++)
    {
        for (int j = 0; j < TETRIS_MAX_SIZE; j++)
        {
            rotateBuffer[i][j] = shape->array[TETRIS_MAX_SIZE - j - 1][i];
        }
    }

    bool exit = false;
    while(!exit)
    {
        for (int i = 0; i < TETRIS_MAX_SIZE; i++)
        {
            if(rotateBuffer[0][i] == 1)
            {
                exit = true;
                break;
            }
        }

        if(!exit)
        {
            for(int i = 0; i < (TETRIS_MAX_SIZE - 1); i++)
            {
                memcpy(rotateBuffer[i], rotateBuffer[i + 1], TETRIS_MAX_SIZE);
            }

            memset(rotateBuffer[TETRIS_MAX_SIZE - 1], 0, TETRIS_MAX_SIZE);
        }
    }

    exit = false;
    while(!exit)
    {
        for (int i = 0; i < TETRIS_MAX_SIZE; i++)
        {
            if(rotateBuffer[i][0] == 1)
            {
                exit = true;
                break;
            }
        }

        if(!exit)
        {
            for(int i = 0; i < TETRIS_MAX_SIZE; i++)
            {
                memcpy(rotateBuffer[i], rotateBuffer[i] + 1, TETRIS_MAX_SIZE - 1);
                rotateBuffer[i][TETRIS_MAX_SIZE - 1] = 0;
            }
        }
    }


    if(renderShape(rotateBuffer, shape->ring, shape->pixel, shape->color, true))
    {
        memcpy(shape->array, rotateBuffer, sizeof(rotateBuffer));
    }
}

void rotateFrame(bool clockwise)
{
    EffectTetris *data = &state->data->tetris;

    data->shape->pixel += clockwise ? -1 : 1;

    int buffer[2];

    for(int i = 0; i < LED_TOTAL_RINGS; i++)
    {
        buffer[0] = getPixelColor(i, 0);

        for (int j = (LED_PER_RING - 1); j >= 0; j--)
        {
            buffer[1] = getPixelColor(i, clockwise ? j : -j);
            setPixelColor(i, clockwise ? j : -j, buffer[0]);

            buffer[0] = buffer[1];
        }
    }
}