/* SDL_Demo: a little demo based on SDL direct pixel blitting
 * Copyright (C) 2004 Angelo "Encelo" Theodorou
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
 */

#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "libsdl_draw.h"

extern SDL_Surface *screen;

/* Return the pixel value at (x, y)
 * NOTE: The surface must be locked before calling this! */
Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
        {
            return p[0] << 16 | p[1] << 8 | p[2];
        }
        else
        {
            return p[0] | p[1] << 8 | p[2] << 16;
        }
    case 4:
        return *(Uint32 *)p;

    default:
        return 0;                       /* shouldn't happen, but avoids warnings */
    }
}

/* Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this! */
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
        {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        }
        else
        {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

/* Copies two polygons */
void copy(float src[][2], float dest[][2], int points)
{
    int i;

    for (i = 0; i < points; i++)
    {
        dest[i][X] = src[i][X];
        dest[i][Y] = src[i][Y];
    }
}

/* Finds the polygon square clipping */
void find_clip(int x, int y, SDL_Rect *rect, float vertex[][2], int points)
{
    int i;
    float min_x, min_y;
    float max_x, max_y;

    min_x = max_x = vertex[0][X];
    min_y = max_y = vertex[0][Y];
    for (i = 1; i < points; i++)
    {
        if (vertex[i][X] < min_x)
        {
            min_x = vertex[i][X];
        }
        if (vertex[i][X] > max_x)
        {
            max_x = vertex[i][X];
        }
        if (vertex[i][Y] < min_y)
        {
            min_y = vertex[i][Y];
        }
        if (vertex[i][Y] > max_y)
        {
            max_y = vertex[i][Y];
        }
    }
    rect->x = x + (int)(min_x);
    rect->y = y + (int)(min_y);
    rect->w = (int)(max_x - min_x);
    rect->h = (int)(max_y - min_y);
}

/* Extends the clip area */
void extend_clip(SDL_Rect *rect, int dim)
{
    rect->x -= dim;
    rect->y -= dim;
    rect->w += dim;
    rect->h += dim;
}

/* Returns 1 if the vertices array is the center point (0,0), 0 otherwise */
int is_point(float vertex[][2], int points)
{
    int i;

    for (i = 0; i < points; i++)
    {
        if ((vertex[i][X] > 1) || (vertex[i][Y] > 1))
        {
            return 0;
        }
    }
    return 1;
}

/* Scales a polygon */
void scale(float value, float vertex[][2], int points)
{
    int i;

    for (i = 0; i < points; i++)
    {
        vertex[i][X] = value * vertex[i][X];
        vertex[i][Y] = value * vertex[i][Y];
    }
}

/* Rotates a polygon */
void rotate(int degrees, float vertex[][2], int points)
{
    int i;
    double rad = M_PI / 180 * degrees;
    float temp_x;

    for (i = 0; i < points; i++)
    {
        temp_x = vertex[i][X] * cos(rad) - vertex[i][Y] * sin(rad);
        vertex[i][Y] = vertex[i][X] * sin(rad) + vertex[i][Y] * cos(rad);
        vertex[i][X] = temp_x;
    }
}

/* Draws polygon vertices */
void draw(int x, int y, float vertex[][2], int points, Uint32 color)
{
    /* locking the screen if necessary */
    if (SDL_MUSTLOCK(screen))
    {
        SDL_LockSurface(screen);
    }

    int i, px, py;

    for (i = 0; i < points; i++)
    {
        px = x + vertex[i][X];
        py = y + vertex[i][Y];
        putpixel(screen, px, py, color);
        SDL_UpdateRect(screen, px, py, 1, 1);
    }
    /* unlocking the screen if necessary */
    if (SDL_MUSTLOCK(screen))
    {
        SDL_UnlockSurface(screen);
    }
}

/* Draws a line */
void draw_line(int p0[2], int p1[2], Uint32 color)
{
    int i;
    int comp;
    int d_a, d_b;
    int a, b;
    int inc;

    /* locking the screen if necessary */
    if (SDL_MUSTLOCK(screen))
    {
        SDL_LockSurface(screen);
    }
    /* drawing a single point */
    if (p0[X] == p1[X] && p0[Y] == p1[Y])
    {
        putpixel(screen, p0[X], p0[Y], color);
    }
    /* Horizontal drawing */
    else if (abs(p1[X] - p0[X]) >= abs(p1[Y] - p0[Y]))
    {
        d_a = p1[X] - p0[X];
        d_b = p1[Y] - p0[Y];
        a = p0[X];
        b = p1[X];
        /* left to right */
        if (p0[X] < p1[X])
        {
            inc = 1;
        }
        /* right to left */
        else
        {
            inc = -1;
        }
        /* drawing the extremes too */
        b += inc;
        for (i = a; i != b; i = i + inc)
        {
            /* calculating the y */
            comp = (d_b * (i - a)) / (d_a) + p0[Y];
            /* drawing the pixel to the actual coordinates */
            putpixel(screen, i, comp, color);
        }
    }
    /* Vertical drawing */
    else
    {
        d_a = p1[Y] - p0[Y];
        d_b = p1[X] - p0[X];
        a = p0[Y];
        b = p1[Y];
        /* bottom to top */
        if (p0[Y] < p1[Y])
        {
            inc = 1;
        }
        /* top to bottom */
        else
        {
            inc = -1;
        }
        /* drawing the extremes too */
        b += inc;
        for (i = a; i != b; i = i + inc)
        {
            /* calculating the x */
            comp = (d_b * (i - a)) / (d_a) + p0[X];
            /* drawing the pixel to the actual coordinates */
            putpixel(screen, comp, i, color);
        }
    }
    /* unlocking the screen if necessary */
    if (SDL_MUSTLOCK(screen))
    {
        SDL_UnlockSurface(screen);
    }
}

/* Draws a polygon with edges */
void draw_poly(int x, int y, float vertex[][2], int points, Uint32 color)
{
    int i, p0[2], p1[2];

    for (i = 0; i < points - 1; i++)
    {
        p0[X] = x + vertex[i][X];
        p0[Y] = y + vertex[i][Y];
        p1[X] = x + vertex[i + 1][X];
        p1[Y] = y + vertex[i + 1][Y];
        draw_line(p0, p1, color);
    }
    p0[X] = x + vertex[i][X];
    p0[Y] = y + vertex[i][Y];
    p1[X] = x + vertex[0][X];
    p1[Y] = y + vertex[0][Y];
    draw_line(p0, p1, color);
}

/* Draws a filled polygon scaling it to its center
 * Very lazy, slow and dumb solution */
void draw_fill_scaling(int x, int y, float vertex[][2], int points, Uint32 color)
{
    int p0[2], p1[2];
    float poly[points][2];

    copy(vertex, poly, points);
    while (1)
    {
        scale(0.99, poly, points);
        draw_poly(x, y, poly, points, color);
        if (is_point(poly, points))
        {
            return;
        }
    }
}

/* Second attempt to fill a polygon :-) */
void draw_fill(int x, int y, float vertex[][2], int points, Uint32 color)
{
    int i, j, fill = 0;
    SDL_Rect clip;

    /* locking the screen if necessary */
    if (SDL_MUSTLOCK(screen))
    {
        SDL_LockSurface(screen);
    }
    draw_poly(x, y, vertex, points, color);
    find_clip(x, y, &clip, vertex, points);
    for (i = 1; i < clip.h - 1; i++)
    {
        for (j = 0; j < clip.w; j++)
        {
            if ((getpixel(screen, clip.x + j, clip.y + i) == color) && (getpixel(screen, clip.x + j - 1, clip.y + i) !=
                    color))
            {
                fill = 1;
            }
            else if ((getpixel(screen, clip.x + j, clip.y + i) == color) && (getpixel(screen, clip.x + j + 1, clip.y +
                     i) != color))
            {
                fill = 2;
            }
            if (fill == 1)
            {
                putpixel(screen, clip.x + j, clip.y + i, color);
            }
            else if (fill == 2)
            {
                fill = 0;
                break;
            }
        }
    }
    /* unlocking the screen if necessary */
    if (SDL_MUSTLOCK(screen))
    {
        SDL_UnlockSurface(screen);
    }
}

/* Draws text with libSDL_ttf */
void draw_text(TTF_Font *font, char *text, SDL_Color color, int x, int y, SDL_Rect *dest)
{
    SDL_Surface *text_surf;

    text_surf = TTF_RenderText_Solid(font, text, color);
    if (x == TEXT_CENTER)
    {
        dest->x = (screen->w - text_surf->w) / 2;
    }
    else
    {
        dest->x = x;
    }
    if (y == TEXT_CENTER)
    {
        dest->y = (screen->h - text_surf->h) / 2;
    }
    else
    {
        dest->y = y;
    }
    dest->w = text_surf->w;
    dest->h = text_surf->h;
    SDL_BlitSurface(text_surf, NULL, screen, dest);
    SDL_UpdateRects(screen, 1, dest);
    SDL_FreeSurface(text_surf);
}

/* Erases a text inside a dest rectangle */
void erase_text(SDL_Rect *dest)
{
    Uint32 black;

    black = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);

    SDL_FillRect(screen, dest, black);
    SDL_UpdateRects(screen, 1, dest);
}

/* Implements a simple fade out
 * It's easy to change this routine to a fade in or to support animations */
void fade_out(int time, int steps)
{
    Uint32 black;

    black = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);

    const SDL_VideoInfo *video_info;
    int bpp;
    SDL_Surface *surf, temp;
    int opacity;

    video_info = SDL_GetVideoInfo();
    bpp = video_info->vfmt->BitsPerPixel;
    surf = SDL_CreateRGBSurface(0, screen->w, screen->h, bpp, 0, 0, 0, 0);

    SDL_BlitSurface(screen, NULL, &temp, NULL);
    for (opacity = 0; opacity < 255; opacity += 255 / steps)
    {
        SDL_SetAlpha(surf, SDL_SRCALPHA | SDL_RLEACCEL, opacity);
        SDL_BlitSurface(surf, NULL, screen, NULL);
        SDL_UpdateRect(screen, 0, 0, screen->w, screen->h);
        SDL_Flip(screen);
        SDL_Delay(time / steps);
        SDL_BlitSurface(&temp, NULL, screen, NULL);
    }
    SDL_FreeSurface(surf);
}
