/*
 * screen.cc
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

#include "screen.h"
#include "getrand.h"

#include <iostream>
#include <cstdlib>

using namespace std;

Screen::Screen(const Options &o): pals(NULL), npals(0)
{
    surf = SDL_SetVideoMode(o.reso_x, o.reso_y, 8, SDL_HWPALETTE);
    if (!surf)
    {
        cerr << "Surface init failure\n";
        throw;
    }
    SDL_ShowCursor(SDL_DISABLE);
    SDL_WM_SetCaption("Spiralizer", NULL);
    if (HAS_FLAG(o.flags, F_FSMODE))
    {
        ToggleFS();
    }
    if (o.pal_img)
    {
        if (LoadPalettes(o.pal_img) < 0)
        {
            cerr << "Screen: Palette loading failure. Exiting\n";
            exit(1);
        }
        if (o.pal_num == -1)
        {
            SetPalette((int)getrand(npals));
        }
        else
        {
            SetPalette(o.pal_num);
        }
    }
}

int Screen::SetPalette(int n)
{
    if (n < 0)
    {
        cerr << "Negative palette number requested\n";
        return -1;
    }
    if (n >= npals)
    {
        cerr << "Too high palette number requested\n";
        return -1;
    }
    SDL_SetPalette(surf, SDL_LOGPAL | SDL_PHYSPAL, pals[n], 0, 256);

    return 0;
}

int Screen::ClearPalettes()
{
    if (!pals)
    {
        return -1;
    }
    for (int i = 0; i < npals; ++i)
    {
        delete pals[i];
    }
    delete pals;

    pals = NULL;
    npals = 0;

    return 0;
}

int Screen::LoadPalettes(const char *p)
{
    SDL_Surface *p_img;
    int width, x, y;
    SDL_Color *pal, c;
    Uint8 *row, *pxl;
    const int bpp = 3;

    p_img = SDL_LoadBMP(p);
    if (!p_img)
    {
        cerr << "Failed opening palette image: " << p << endl;
        return -1;
    }
    if (pals)
    {
        ClearPalettes();
    }
    pals = new SDL_Color *[p_img->h];
    if (!pals)
    {
        cerr << "Out of memory\n";
        return -1;
    }
    npals = p_img->h;
    width = (p_img->w < 256 ? p_img->w : 256);
    for (y = 0; y < p_img->h; ++y)
    {
        row = (Uint8 *)p_img->pixels + y * p_img->w * bpp;
        pal = new SDL_Color[256];
        if (!pal)
        {
            cerr << "Out of memory\n";
            return -1;
        }
        for (x = 0; x < width; ++x)
        {
            pxl = row + x * bpp;

            SDL_GetRGB(*(Uint32 *)pxl, p_img->format, &c.r, &c.g, &c.b);

            pal[x] = c;
        }
        pals[y] = pal;
    }
    SDL_FreeSurface(p_img);

    return 0;
}
