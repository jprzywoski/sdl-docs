/***************************************************************************
 *                                  fire.c
 *                           -------------------
 *  begin                : Thu Apr 13 2000
 *  copyright            : (C) 2000 by Stephan Uhlmann
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

#include "fire.h"

// flame palette contains (modified) source code from
// The Rasterman's xflame (version ported to SDL by Sam Lantinga)

void fire_colormap(SDL_Surface *buffer)
{
    SDL_Color cmap[300];
    int i, r, g, b;

    r = g = b = 0;
    for (i = 0; (r < 255) || (g < 255) || (b < 255); i++)
    {
        r = (i - 10) * 3;
        g = (i - 50) * 3;
        b = (i - 140) * 3;
        if (r < 0)
        {
            r = 0;
        }
        if (r > 255)
        {
            r = 255;
        }
        if (g < 0)
        {
            g = 0;
        }
        if (g > 255)
        {
            g = 255;
        }
        if (b < 0)
        {
            b = 0;
        }
        if (b > 255)
        {
            b = 255;
        }
        cmap[i].r = r;
        cmap[i].g = g;
        cmap[i].b = b;
    }
    SDL_SetColors(buffer, cmap, 0, i);
}

void fire_run(SDL_Surface *screen, SDL_Surface *buffer, int width, int height)
{
    Uint8 *offset, *minoffset, *maxoffset;
    int degree;
    int i, x, y;
    int xdim, ydim;
    int xwave, ywave;
    SDL_Event event;
    Uint8 cnew;

    minoffset = (Uint8 *)buffer->pixels + width;
    maxoffset = (Uint8 *)buffer->pixels + width * height - width;

    degree = 90;
    xwave = 3;
    ywave = 1;

    srand(time(NULL));

    SDL_PollEvent(&event);
    while ((event.type != SDL_QUIT) && (event.type != SDL_KEYDOWN))
    {
        degree++;
        if (degree >= 180)
        {
            degree = 0;
            xwave = (int)((float)rand() / RAND_MAX * 5 + 1);
            ywave = (int)((float)rand() / RAND_MAX * 5 + 1);
        }
        xdim = (int)(sin(degree * 3.141 / 180) * width * 0.45);
        ydim = (int)(sin(degree * 3.141 / 180) * height * 0.40);
        // thats the funny whirling stuff
        for (i = 0; i < 360; i += 10)
        {
            x = (Uint32)(sin((degree + i) * xwave * 3.141 / 180) * xdim) + width / 2;
            y = (Uint32)(cos((degree + i) * ywave * 3.141 / 180) * ydim) + height / 2;

            setPixel8(buffer, x - 1, y - 1, 220);
            setPixel8(buffer, x, y - 1, 220);
            setPixel8(buffer, x + 1, y - 1, 220);
            setPixel8(buffer, x - 1, y, 220);
            setPixel8(buffer, x, y, 220);
            setPixel8(buffer, x + 1, y, 220);
            setPixel8(buffer, x - 1, y + 1, 220);
            setPixel8(buffer, x, y + 1, 220);
            setPixel8(buffer, x + 1, y + 1, 220);
        }
        // some noise in the last row
        for (offset = maxoffset - width; offset < maxoffset; offset++)
        {
            *(offset) = (Uint8)(rand() * 128) + 96;
        }
        // fire!
        for (offset = minoffset; offset < maxoffset; offset++)
        {
            cnew = (*(offset - 1) + *(offset) + *(offset + 1) + *(offset - width)) >> 2;
            if (cnew > 0)
            {
                *(offset - width) = --cnew;    // nice effect: s/width/height/
            }
        }
        SDL_BlitSurface(buffer, NULL, screen, NULL);
        SDL_UpdateRect(screen, 0, 0, width - 1, height - 2);

        sdldemo_tickfps();

        SDL_PollEvent(&event);
    }
}
