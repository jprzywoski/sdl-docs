// phire prng.cpp - Pseudo-random number generator
// Copyright (C) 2005, Ertugrul Soeylemez
//
// This program is free software distributed under the terms of the
// GNU General Public License version 2. See the file README.

#include <time.h>
#include "prng.h"

// Internal state
rnd_t rnd1 = 0;
rnd_t rnd2 = 0;

// Add entropy
void rnd_add(rnd_t val)
{
    rnd2 += val;
    rnd_get();
}

// Initialize PRNG
void rnd_init()
{
    rnd1 = time(0);
    rnd_get();
}
