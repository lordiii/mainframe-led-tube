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
    for (int x = 0; x < 4; x++)
    {
        if (shape->array[shape->colission.bottom][x] == 1 && getPixelColor(shape->ring - 1, shape->pixel + x) > 0) {
            return true;
        }
    }

    return false;
}

bool detectSideCollision(Shape *shape, int sideforce)
{
    int *index = sideforce > 0 ? &shape->colission.right : &shape->colission.left;

    for (int x = 0; x < 3; x++)
    {
        if(shape->array[*index][x] == 1 && getPixelColor(shape->ring + x, shape->pixel + sideforce) > 0) {
            return sideforce;
        }
    }

    return 0;
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

    int *bottom = &data->currentShape->colission.bottom;
    for (*bottom = 0; *bottom < 3; *bottom += 1) {
        for (int i = 0; i < 4; i++) {
            if (data->currentShape->array[*bottom][i] == 1) {
                break;
            }
        }
    }

    int *left = &data->currentShape->colission.left;
    for (*left = 0; *left < 4; *left += 1) {
        for (int i = 0; i < 3; i++) {
            if (data->currentShape->array[*left][i] == 1) {
                break;
            }
        }
    }

    int *right = &data->currentShape->colission.right;
    for (*right = 0; *right < 4; *right += 1) {
        for (int i = 0; i < 3; i++) {
            if (data->currentShape->array[*right][i] == 1) {
                break;
            }
        }
    }
}

void processMovement(bool forceMovement)
{
    EffectTetris *data = &state->data->tetris;

    int downForce = data->movement == NONE || data->movement == DOWN || forceMovement;
    int sideForce = data->movement == LEFT || data->movement == RIGHT ? data->movement : 0;

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

