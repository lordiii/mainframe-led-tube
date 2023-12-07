#include <effects.h>
#include <effects_lib.h>
#include "tetris.h"

const uint8_t TETRONIMOS[5][3][4] = {
        {{1, 1, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}},
        {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
        {{0,0,0,0}, {1, 1, 1, 1}, {0, 0, 0, 0}},
        {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}},
        {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}
};

void initializeTetris()
{
    EffectTetris *data = &state->data->tetris;

    if(data->currentShape == nullptr) {
        data->currentShape = new Shape;
        data->currentShape->placed = true;
    }

    randomSeed(micros());
}

void renderShape(Shape *shape, int color)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (shape->array[i][j] == 1)
            {
                setPixelColor(shape->ring + i, shape->pixel + j, color);
            }
        }
    }
}

bool detectDownCollision(Shape *shape)
{
    for(int y = 0; y < 3; y++) {
        for (int x = 0; x < 4; x++)
        {
            if (shape->array[y][x] == 1) {
                if(getPixelColor(shape->ring - 1 + y, shape->pixel + x) > 0)
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
    for (int x = 0; x < 4; x++)
    {
        for(int y = 0; y < 3; y++)
        {
            if (shape->array[y][x] == 1) {
                if(getPixelColor(shape->ring + y, shape->pixel + x + sideforce) > 0)
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

    uint8_t shapeID = random(0, 5);
    memcpy(data->currentShape->array, TETRONIMOS[shapeID], sizeof(TETRONIMOS[shapeID]));

    data->currentShape->ring = LED_TOTAL_RINGS - 4;
    data->currentShape->pixel = random(0, LED_PER_RING);
    data->currentShape->placed = false;
    data->currentShape->currentColor = randomColor();
}

void processMovement(bool forceMovement)
{
    EffectTetris *data = &state->data->tetris;

    int downForce = state->movement == NONE || state->movement == DOWN || forceMovement;
    int sideForce = state->movement == LEFT || state->movement == RIGHT ? state->movement : 0;

    bool collidesDown;
    if (downForce > 0) {
        collidesDown = detectDownCollision(data->currentShape);
    } else {
        collidesDown = false;
    }

    int collidesSide;
    if (sideForce > 0) {
        collidesSide = detectSideCollision(
                data->currentShape,
                sideForce
        );
    } else {
        collidesSide = 0;
    }

    if(data->currentShape->ring <= 0 || collidesDown || collidesSide != 0)
    {
        data->currentShape->ring -= collidesDown;
        data->currentShape->pixel += collidesSide;
        data->currentShape->placed = true;
    } else {
        data->currentShape->ring -= downForce;
        data->currentShape->pixel += sideForce;
    }
}

void eliminateRings()
{
    int total = 0;

    for(int i = 0; i < LED_TOTAL_RINGS; i++) {
        bool match = true;

        for(int pixel = 0; pixel < LED_PER_RING; pixel++) {
            if(getPixelColor(0, pixel) > 0)
            {
                match = false;
                break;
            }
        }

        if(match)
        {
            total++;

            for(int ring = 0; ring < (LED_TOTAL_RINGS - 1); ring++) {
                for(int pixel = 0; pixel < LED_PER_RING; pixel++) {
                    setPixelColor(ring, pixel, getPixelColor(ring + 1, pixel));
                 }
            }

            for(int pixel = 0; pixel < LED_PER_RING; pixel++) {
                setPixelColor(LED_TOTAL_RINGS, pixel, 0);
            }
        } else {
            break;
        }
    }

    state->data->tetris.score += ((total * 10) * total);
}