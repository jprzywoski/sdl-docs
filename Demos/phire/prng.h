// phire prng.h - Pseudo-random number generator
// Copyright (C) 2005, Ertugrul Soeylemez
//
// This program is free software distributed under the terms of the
// GNU General Public License version 2. See the file README.

#ifndef _phire_prng_h
#define _phire_prng_h

#include <SDL/SDL.h>
#include <math.h>

// Type for random numbers, must be 32 bits
typedef Uint32 rnd_t;

// Random increments
#define RNDI1	(0x682AF7A3)
#define RNDI2	(0x9AD17C1B)

// Internal state
extern rnd_t rnd1;
extern rnd_t rnd2;

// Floating point random numbers:
// 0 <= getf() < 1
// 0 <= getf1() <= 1
// -1 <= getf2() <= 1
#define rnd_getf()	(float(rnd_get()) / (float(rnd_t(-1)) + 1))
#define rnd_getf1()	(float(rnd_get()) / float(rnd_t(-1)))
#define rnd_getf2()	(-1 + rnd_getf1() * 2)

// Generate a random number
static inline rnd_t rnd_get()
{
    rnd_t tmp;

    tmp = rnd2 >> 25;
    rnd2 <<= 7;
    rnd2 |= tmp;
    rnd1 += RNDI1;
    rnd2 += RNDI2;
    return rnd1 += rnd2;
}

// Return random point on or in the unit circle
static inline void rnd_circle(float &x, float &y, float f)
{
    do
    {
        x = rnd_getf2();
        y = rnd_getf2();
    }
    while (hypot(x, y) > 1);
    x *= f;
    y *= f;
}

// Functions
void rnd_add(rnd_t val);

void rnd_init();

#endif
