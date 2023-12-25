// phire fire.cpp - Fire class
// Copyright (C) 2005, Ertugrul Soeylemez
//
// This program is free software distributed under the terms of the
// GNU General Public License version 2. See the file README.

#include "app.h"
#include "fire.h"
#include "prng.h"

// Radius of a particle
#define PRADIUS (5)

// Draw pixel with additive rendering
static inline void putp(int x, int y, int col)
{
    if (x < 1)
    {
        return;
    }
    if (y < 1)
    {
        return;
    }
    if (x >= mx)
    {
        return;
    }
    if (y >= my)
    {
        return;
    }
    x += y * mx;
    col += px[x];
    px[x] = min(255, col);
}

// Draw fire
void fire::draw()
{
    float rad;
    float tmp;
    int tx, ty;

    rad = time * PRADIUS;
    for (tx = 0; tx < rad; tx++)
    {
        for (ty = 0; ty < rad; ty++)
        {
            tmp = 1 - hypot(tx, ty) / rad;
            if (tmp < 0)
            {
                continue;
            }
            putp(int(x + tx), int(y + ty), int(time * tmp * 96));
            if (ty)
            {
                putp(int(x + tx), int(y - ty), int(time * tmp * 96));
            }
            if (tx)
            {
                putp(int(x - tx), int(y + ty), int(time * tmp * 96));
            }
            if (tx && ty)
            {
                putp(int(x - tx), int(y - ty), int(time * tmp * 96));
            }
        }
    }
}

// Update fire
void fire::run()
{
    x += ax * tdiff;
    y += ay * tdiff;
    switch (phase)
    {
    case 0:
        ay -= 5 * tdiff;
        if (ay < -0.0625)
        {
            phase++;
        }
        break;

    case 1:
        ay -= 5 * tdiff;
        if (x > mx / 2 + 5)
        {
            ax -= 20 * tdiff;
        }
        if (x < mx / 2 - 5)
        {
            ax += 20 * tdiff;
        }
        if (y < miny)
        {
            phase++;
        }
        time -= 0.025 * tdiff;
        break;

    default:
        ay += 15 * tdiff;
        time -= 0.05 * tdiff;
    }
    if (time < 0)
    {
        time = 0;
    }
    if (time < 0.5)
    {
        die();
    }
}

// fire constructor
fire::fire(node *p): node(p)
{
    x = (mx - 1) / 2;
    y = my - (my / 4);
    rnd_circle(ax, ay, 25);
    ay += 5;
    time = 1 - rnd_getf1() * 0.5;
    phase = 0;
    miny = my / 4 - rnd_getf2() * my / 8;
}
