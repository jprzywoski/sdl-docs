// phire fire.h - Fire class
// Copyright (C) 2005, Ertugrul Soeylemez
//
// This program is free software distributed under the terms of the
// GNU General Public License version 2. See the file README.

#ifndef _phire_fire_h
#define _phire_fire_h

#include "node.h"

class fire: public node
{
protected:
    float x, y;
    float ax, ay;
    float time;
    float miny;
    int phase;

public:
    fire(node *p);

    void draw();
    void run();
};

#endif
