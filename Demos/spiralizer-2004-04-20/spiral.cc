/*
 * spiral.cc
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

#include "spiral.h"

#include <cmath>

void draw_inc_line(const Coord &, const Coord &, Screen &);
void draw_dec_line(const Coord &, const Coord &, Screen &);

Spiral::Spiral(SpiralType t)
{
    switch (t)
    {
    case S_INC:
        line_func = draw_inc_line;
        break;

    case S_DEC:
        line_func = draw_dec_line;
    }
}

void Spiral::Update(Screen &scr)
{
    /* Rotate spiral */
    ang += rotation;

    /* Reshape spiral */
    ang_inc = sin(ang / 24) * (M_PI * 2 / 2.5);

    Move(scr);
    Draw(scr);
}

void Spiral::Move(Screen &scr)
{
    int r = (int)(rad + 0.5);

    pos += mv;
    if (pos.x < r)
    {
        mv.x *= -1;
        pos.x = r;
    }
    else if (pos.x + r > scr.Width())
    {
        mv.x *= -1;
        pos.x = scr.Width() - r;
    }
    if (pos.y < r)
    {
        mv.y *= -1;
        pos.y = r;
    }
    else if (pos.y + r > scr.Height())
    {
        mv.y *= -1;
        pos.y = scr.Height() - r;
    }
}

void Spiral::Draw(Screen &scr)
{
    Coord prev, next;
    double a, r;

    prev = pos;
    a = ang;
    for (r = 0; r < rad; r += rad_inc)
    {
        /* 0.5 is added to get a rounded value *
         * when the number is forced to an int */
        next.x = (int)(pos.x + sin(a) * r + 0.5);
        next.y = (int)(pos.y + cos(a) * r + 0.5);

        line_func(prev, next, scr);

        prev = next;
        a += ang_inc;
    }
}

/* draw incremental line */
void draw_inc_line(const Coord &a, const Coord &b, Screen &scr)
{
    Uint8 *buffer = (Uint8 *)scr.Pixels();
    Sint32 max_x, min_x, max_y, min_y;
    Uint32 dy, dx, s;
    Sint32 step;
    register Uint8 *pxl;
    register Sint32 x, y;

    min_x = (a.x < b.x ? a.x : b.x);
    max_x = (a.x < b.x ? b.x : a.x);
    min_y = (a.y < b.y ? a.y : b.y);
    max_y = (a.y < b.y ? b.y : a.y);

    dy = max_y - min_y;
    dx = max_x - min_x;
    s = 0;
    if (dx > dy)
    {
        y = (int)((a.x < b.x ? a.y : b.y) + 0.5);
        step = (y < max_y ? 1 : -1) * scr.Width();
        pxl = buffer + y * scr.Width() + min_x;
        for (x = min_x; x <= max_x; ++x)
        {
            s += dy;
            if (s > dx)
            {
                s -= dx;

                /* move on the y axis */
                pxl += step;
            }
            /* move one right on the x axis */
            pxl += 1;

            /* this is the exception */
            ++*pxl;
        }
    }
    else
    {
        x = (int)((a.y < b.y ? a.x : b.x) + 0.5);
        step = (x < max_x ? 1 : -1);
        pxl = buffer + min_y * scr.Width() + x;
        for (y = min_y; y <= max_y; ++y)
        {
            s += dx;
            if (s >= dy)
            {
                s -= dy;

                /* move on the x axis */
                pxl += step;
            }
            /* move one down on the y axis */
            pxl += scr.Width();

            /* this is the exception */
            ++*pxl;
        }
    }
}

/* draw decremental line */
void draw_dec_line(const Coord &a, const Coord &b, Screen &scr)
{
    Uint8 *buffer = (Uint8 *)scr.Pixels();
    Sint32 max_x, min_x, max_y, min_y;
    Uint32 dy, dx, s;
    Sint32 step;
    register Uint8 *pxl;
    register Sint32 x, y;

    min_x = (a.x < b.x ? a.x : b.x);
    max_x = (a.x < b.x ? b.x : a.x);
    min_y = (a.y < b.y ? a.y : b.y);
    max_y = (a.y < b.y ? b.y : a.y);

    dy = max_y - min_y;
    dx = max_x - min_x;
    s = 0;
    if (dx > dy)
    {
        y = (int)((a.x < b.x ? a.y : b.y) + 0.5);
        step = (y < max_y ? 1 : -1) * scr.Width();
        pxl = buffer + y * scr.Width() + min_x;
        for (x = min_x; x <= max_x; ++x)
        {
            s += dy;
            if (s > dx)
            {
                s -= dx;

                /* move on the y axis */
                pxl += step;
            }
            /* move one right on the x axis */
            pxl += 1;

            /* this is the exception */
            --*pxl;
        }
    }
    else
    {
        x = (int)((a.y < b.y ? a.x : b.x) + 0.5);
        step = (x < max_x ? 1 : -1);
        pxl = buffer + min_y * scr.Width() + x;
        for (y = min_y; y <= max_y; ++y)
        {
            s += dx;
            if (s >= dy)
            {
                s -= dy;

                /* move on the x axis */
                pxl += step;
            }
            /* move one down on the y axis */
            pxl += scr.Width();

            /* this is the exception */
            --*pxl;
        }
    }
}
