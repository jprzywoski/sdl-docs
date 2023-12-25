/*
 * screen.h
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

#ifndef SCREEN_H
#define SCREEN_H

#include "options.h"

#include <SDL/SDL.h>
#include <iostream>

class Screen
{
public:
    Screen(const Options &);

    void Flip()
    {
        SDL_Flip(surf);
    }
    void UpdateRects(SDL_Rect *rects, int n)
    {
        SDL_UpdateRects(surf, n, rects);
    }

    void ToggleFS()
    {
        SDL_WM_ToggleFullScreen(surf);
    }

    void Lock()
    {
        SDL_LockSurface(surf);
    }
    void Unlock()
    {
        SDL_UnlockSurface(surf);
    }

    void Fill(SDL_Rect *r, Uint32 c)
    {
        SDL_FillRect(surf, r, c);
    }

    int Width() const
    {
        return surf->w;
    }
    int Height() const
    {
        return surf->h;
    }

    void *Pixels() const
    {
        return surf->pixels;
    }
    SDL_Surface *Surface() const
    {
        return surf;
    }

    int SetPalette(int);

private:
    int ClearPalettes();
    int LoadPalettes(const char *);

    SDL_Surface *surf;
    SDL_Color **pals;
    int npals;
};

#endif /* SCREEN_H */
