// This file auto-generated by h3mtools' passabilitytool by John AAkerblom
static const int _1x1[] = { 487, 488, 495, 248, 502, 520, 521, 522, 523, 524, 525, 526, 527, 528, 529, 531, 532, 533, 534, 535, 537, 538, 325, 291, 292, 293, 540, 541, 542 };
static const int _1x2[] = { -1 };
static const int _1x3[] = { -1 };
static const int _1x4[] = { -1 };
static const int _1x5[] = { -1 };
static const int _1x6[] = { -1 };

static const int _2x1[] = { 486, 492, 493, 496, 498, 500, 501, 509, 510, 511, 512, 513, 514, 515, 516, 517, 518, 519, 530, 536, 539 };
static const int _2x2[] = { 494, 497 };
static const int _2x3[] = { 490 };
static const int _2x4[] = { -1 };
static const int _2x5[] = { -1 };
static const int _2x6[] = { -1 };

static const int _3x1[] = { 491 };
static const int _3x2[] = { 489, 505, 506, 507, 508 };
static const int _3x3[] = { -1 };
static const int _3x4[] = { -1 };
static const int _3x5[] = { -1 };
static const int _3x6[] = { -1 };

static const int _4x1[] = { -1 };
static const int _4x2[] = { -1 };
static const int _4x3[] = { 499 };
static const int _4x4[] = { -1 };
static const int _4x5[] = { -1 };
static const int _4x6[] = { -1 };

static const int _5x1[] = { -1 };
static const int _5x2[] = { -1 };
static const int _5x3[] = { 503, 504 };
static const int _5x4[] = { -1 };
static const int _5x5[] = { -1 };
static const int _5x6[] = { -1 };

static const int _6x1[] = { -1 };
static const int _6x2[] = { -1 };
static const int _6x3[] = { -1 };
static const int _6x4[] = { -1 };
static const int _6x5[] = { -1 };
static const int _6x6[] = { -1 };

static const int _7x1[] = { -1 };
static const int _7x2[] = { -1 };
static const int _7x3[] = { -1 };
static const int _7x4[] = { -1 };
static const int _7x5[] = { -1 };
static const int _7x6[] = { -1 };

static const int _8x1[] = { -1 };
static const int _8x2[] = { -1 };
static const int _8x3[] = { -1 };
static const int _8x4[] = { -1 };
static const int _8x5[] = { -1 };
static const int _8x6[] = { -1 };

static const int table_sizes[6][8] = {
    {   29,   21,    1,   -1,   -1,   -1,   -1,   -1 },
    {   -1,    2,    5,   -1,   -1,   -1,   -1,   -1 },
    {   -1,    1,   -1,    1,    2,   -1,   -1,   -1 },
    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1 },
    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1 },
    {   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1 }
};

static const int *table[6][8] = {
    { _1x1, _2x1, _3x1, _4x1, _5x1, _6x1, _7x1, _8x1 },
    { _1x2, _2x2, _3x2, _4x2, _5x2, _6x2, _7x2, _8x2 },
    { _1x3, _2x3, _3x3, _4x3, _5x3, _6x3, _7x3, _8x3 },
    { _1x4, _2x4, _3x4, _4x4, _5x4, _6x4, _7x4, _8x4 },
    { _1x5, _2x5, _3x5, _4x5, _5x5, _6x5, _7x5, _8x5 },
    { _1x6, _2x6, _3x6, _4x6, _5x6, _6x6, _7x6, _8x6 },
};

#include <stdlib.h>

int rand_impassable_ROUGH(int x_dim, int y_dim)
{
    int size = table_sizes[y_dim - 1][x_dim - 1];
    return (-1 == size)? -1 : table[y_dim - 1][x_dim - 1][(rand()+1) % size];
}