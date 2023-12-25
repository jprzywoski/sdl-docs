/* SaveSurf: an example on how to save a SDLSurface in PNG
 * Copyright (C) 2006 Angelo "Encelo" Theodorou
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
 * NOTE:
 *
 * This program is part of "Mars, Land of No Mercy" SDL examples,
 * you can find other examples on http://marsnomercy.org
 */

#include "pngfuncs.h"
#include <GL/gl.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    int width = 1024;
    int height = 768;
    SDL_Surface *output_surf;
    Uint32 rmask, gmask, bmask, amask;
    int row, stride;
    GLubyte *swapline;
    GLubyte *pixels;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
    /* Initializing video subsystem */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL_Init error: %s\n", SDL_GetError());
        exit(-1);
    }
    /* Calling SDL_Quit at exit */
    atexit(SDL_Quit);

    /* Setting screen mode, get a screen from SDL */
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Surface *screen = SDL_SetVideoMode(width, height, 24, SDL_OPENGL);

    SDL_WM_SetCaption("GL_savescreen by Encelo", NULL);

    printf("output file: output.png\n");

    /* Blitting something on the screen */
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);
    glEnd();
    SDL_GL_SwapBuffers();
    SDL_Delay(200);

    /* Creating the output surface to save:
     * 1) allocating memory for pixels data
     * 2) reading from the front buffer
     * 3) swapping data with memcpy
     * 4) creating surface from flipped pixels data
     * */
    stride = screen->w * 4;             /* length of a line in bytes */
    pixels = (GLubyte *)malloc(stride * screen->h);
    swapline = (GLubyte *)malloc(stride);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, screen->w, screen->h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    /* vertical flip */
    for (row = 0; row < screen->h / 2; row++)
    {
        memcpy(swapline, pixels + row * stride, stride);
        memcpy(pixels + row * stride, pixels + (screen->h - row - 1) * stride, stride);
        memcpy(pixels + (screen->h - row - 1) * stride, swapline, stride);
    }
    output_surf = SDL_CreateRGBSurfaceFrom(pixels, screen->w, screen->h, 32, screen->pitch, rmask, gmask, bmask, amask);
    if (png_save_surface("output.png", output_surf) < 0)
    {
        exit(-1);
    }
    /* Quitting */
    SDL_FreeSurface(output_surf);
    SDL_Quit();

    return 0;
}
