/*
 * spiral.h
 *
 * Copyright (C) 2004 Henrik Ala-Uotila
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef SPIRAL_H
#define SPIRAL_H

#include "screen.h"
#include "vector.h"
#include "coord.h"

enum SpiralType
{
    S_INC, S_DEC
};

class Spiral
{
public:
    Spiral(SpiralType);
    Spiral(const Spiral &s): pos(s.pos), mv(s.mv), rad(s.rad), rad_inc(s.rad_inc), ang(s.ang), ang_inc(s.ang_inc),
        rotation(s.rotation) {}

    void Update(Screen &);

    Spiral operator =(const Spiral &s)
    {
        pos = s.pos;
        mv = s.mv;
        rad = s.rad;
        rad_inc = s.rad_inc;
        ang = s.ang;
        ang_inc = s.ang_inc;
        rotation = s.rotation;

        return *this;
    }

    Coord pos;
    Vector mv;
    double rad;
    double rad_inc;
    double ang;
    double ang_inc;
    double rotation;

private:
    void Move(Screen &);
    void Draw(Screen &);

    void (*line_func)(const Coord &, const Coord &, Screen &);
};

#endif /* SPIRAL_H */
