/*graphics.cpp
 * Graphics code.
 *
 * Copyright (C) 2001  James Blewitt
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

SDL_Surface *LoadBMP(char *file)
{
    SDL_Surface *image;

    /* Load the BMP file into a surface */
    image = SDL_LoadBMP(file);
    if (image == NULL)
    {
        fprintf(stderr, "Couldn't load %s: %s\n", file, SDL_GetError());
        return NULL;
    }
    return image;
}

void Draw(unsigned int Xpos, unsigned int Ypos, unsigned int Width, unsigned int Height, SDL_Surface *image,
          SDL_Surface *screen)
{
    SDL_Rect dest;

    /* Blit onto the screen surface.
     * The surfaces should not be locked at this point.
     */
    dest.x = Xpos;
    dest.y = Ypos;
    dest.w = Width;
    dest.h = Height;
    SDL_BlitSurface(image, NULL, screen, &dest);

    /* Update the changed portion of the screen */
    SDL_UpdateRects(screen, 1, &dest);
}

void Erase(unsigned int Xpos, unsigned int Ypos, unsigned int Width, unsigned int Height, SDL_Surface *screen,
           SDL_Surface *background)
{
// Draws the area of the background from under the sprite to the screen.
    SDL_Rect src;

    src.x = Xpos;
    src.y = Ypos;
    src.w = Width;
    src.h = Height;

    SDL_BlitSurface(background, &src, screen, &src);
}

void Update(unsigned int Xpos, unsigned int Ypos, unsigned int Width, unsigned int Height, SDL_Surface *screen)
{
// For updating specific areas of the screen.
    SDL_Rect rect;

    rect.x = Xpos;
    rect.y = Ypos;
    rect.w = Width;
    rect.h = Height;
// Don't try to update stuff that is off the screen.
    if (rect.x + rect.w > screen->w)
    {
        rect.w = screen->w - rect.x;
    }
    if (rect.y + rect.h > screen->h)
    {
        rect.h = screen->h - rect.y;
    }
    SDL_UpdateRects(screen, 1, &rect);
}
