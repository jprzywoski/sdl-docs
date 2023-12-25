/***************************************************************************
 *                             sdldemotools.c
 *                           -------------------
 *  begin                : Thu Apr 13 2000
 *  copyright            : (C) 2000,2001 by Stephan Uhlmann
 *  email                : su@codewizards.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sdldemotools.h"

void ComplainAndExit(void)
{
    fprintf(stderr, "Problem: %s\n", SDL_GetError());
    exit(1);
}

void setPixel8(SDL_Surface *screen, int x, int y, Uint8 c)
{
    *((Uint8 *)(screen->pixels + y * screen->pitch + x)) = c;
}

Uint8 getPixel8(SDL_Surface *screen, int x, int y)
{
    return *((Uint8 *)(screen->pixels + y * screen->pitch + x));
}

void setPixel32(SDL_Surface *screen, int x, int y, SDL_Color c)
{
    *((Uint8 *)(screen->pixels + y * screen->pitch + x * 4 + 2)) = c.r;
    *((Uint8 *)(screen->pixels + y * screen->pitch + x * 4 + 1)) = c.g;
    *((Uint8 *)(screen->pixels + y * screen->pitch + x * 4)) = c.b;
}

SDL_Color getPixel32(SDL_Surface *screen, int x, int y)
{
    SDL_Color c;

    c.r = *((Uint8 *)(screen->pixels + y * screen->pitch + x * 4 + 2));
    c.g = *((Uint8 *)(screen->pixels + y * screen->pitch + x * 4 + 1));
    c.b = *((Uint8 *)(screen->pixels + y * screen->pitch + x * 4));
    return c;
}

/* line algorithm GPL by Julien Carme <julien.carme@acm.org> */

#define SWAP(x, y) \
    x = x + y; \
    y = x - y; \
    x = x - y;

void drawLine8(SDL_Surface *screen, int x1, int y1, int x2, int y2, Uint8 c)
{
    int dx, dy, cxy, dxy;

    /* calculate the distances */
    dx = abs(x1 - x2);
    dy = abs(y1 - y2);

    cxy = 0;
    if (dy > dx)
    {
        /* Follow Y axis */
        if (y1 > y2)
        {
            SWAP(y1, y2);
            SWAP(x1, x2);
        }
        if (x1 > x2)
        {
            dxy = -1;
        }
        else
        {
            dxy = 1;
        }
        for (y1 = y1; y1 < y2; y1++)
        {
            cxy += dx;
            if (cxy >= dy)
            {
                x1 += dxy;
                cxy -= dy;
            }
            setPixel8(screen, x1, y1, c);
        }
    }
    else
    {
        /* Follow X axis */
        if (x1 > x2)
        {
            SWAP(x1, x2);
            SWAP(y1, y2);
        }
        if (y1 > y2)
        {
            dxy = -1;
        }
        else
        {
            dxy = 1;
        }
        for (x1 = x1; x1 < x2; x1++)
        {
            cxy += dy;
            if (cxy >= dx)
            {
                y1 += dxy;
                cxy -= dx;
            }
            setPixel8(screen, x1, y1, c);
        }
    }
}

void drawLine32(SDL_Surface *screen, int x1, int y1, int x2, int y2, SDL_Color c)
{
    int dx, dy, cxy, dxy;

    /* calculate the distances */
    dx = abs(x1 - x2);
    dy = abs(y1 - y2);

    cxy = 0;
    if (dy > dx)
    {
        /* Follow Y axis */
        if (y1 > y2)
        {
            SWAP(y1, y2);
            SWAP(x1, x2);
        }
        if (x1 > x2)
        {
            dxy = -1;
        }
        else
        {
            dxy = 1;
        }
        for (y1 = y1; y1 < y2; y1++)
        {
            cxy += dx;
            if (cxy >= dy)
            {
                x1 += dxy;
                cxy -= dy;
            }
            setPixel32(screen, x1, y1, c);
        }
    }
    else
    {
        /* Follow X axis */
        if (x1 > x2)
        {
            SWAP(x1, x2);
            SWAP(y1, y2);
        }
        if (y1 > y2)
        {
            dxy = -1;
        }
        else
        {
            dxy = 1;
        }
        for (x1 = x1; x1 < x2; x1++)
        {
            cxy += dy;
            if (cxy >= dx)
            {
                y1 += dxy;
                cxy -= dx;
            }
            setPixel32(screen, x1, y1, c);
        }
    }
}

void drawLineAdd32(SDL_Surface *screen, int x1, int y1, int x2, int y2, SDL_Color c)
{
    int dx, dy, cxy, dxy;
    SDL_Color gc;

    /* calculate the distances */
    dx = abs(x1 - x2);
    dy = abs(y1 - y2);

    cxy = 0;
    if (dy > dx)
    {
        /* Follow Y axis */
        if (y1 > y2)
        {
            SWAP(y1, y2);
            SWAP(x1, x2);
        }
        if (x1 > x2)
        {
            dxy = -1;
        }
        else
        {
            dxy = 1;
        }
        for (y1 = y1; y1 < y2; y1++)
        {
            cxy += dx;
            if (cxy >= dy)
            {
                x1 += dxy;
                cxy -= dy;
            }
            gc = getPixel32(screen, x1, y1);
            gc.r += c.r;
            gc.g += c.g;
            gc.b += c.b;
            setPixel32(screen, x1, y1, gc);
        }
    }
    else
    {
        /* Follow X axis */
        if (x1 > x2)
        {
            SWAP(x1, x2);
            SWAP(y1, y2);
        }
        if (y1 > y2)
        {
            dxy = -1;
        }
        else
        {
            dxy = 1;
        }
        for (x1 = x1; x1 < x2; x1++)
        {
            cxy += dy;
            if (cxy >= dx)
            {
                y1 += dxy;
                cxy -= dx;
            }
            gc = getPixel32(screen, x1, y1);
            gc.r += c.r;
            gc.g += c.g;
            gc.b += c.b;
            setPixel32(screen, x1, y1, gc);
        }
    }
}

void sdldemo_startfps()
{
    sdldemo_fpsframes = 0;
    sdldemo_fpsticks = 1;
}

void sdldemo_tickfps()
{
    sdldemo_fpsticks = SDL_GetTicks();
    sdldemo_fpsframes++;
}

void sdldemo_stopfps()
{
    sdldemo_fpsticks = SDL_GetTicks();
}

float sdldemo_getfps()
{
    return (float)(sdldemo_fpsframes * 1000) / sdldemo_fpsticks;
}
