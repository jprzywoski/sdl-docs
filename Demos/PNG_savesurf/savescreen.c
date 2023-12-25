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
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    int width = 1024;
    int height = 768;
    SDL_Surface *input_surf, *output_surf;
    Uint32 rmask, gmask, bmask, amask;
    char *input, *output, *str_ptr;
    int namelen;

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
    /* Parsing shell parameters */
    if (argc == 3)
    {
        input = argv[1];
        output = argv[2];
    }
    else if (argc == 2)
    {
        input = argv[1];
        str_ptr = strstr(argv[1], ".");
        if (str_ptr == NULL)
        {
            namelen = strlen(argv[1]);
        }
        else
        {
            namelen = str_ptr - argv[1];
        }
        output = (char *)malloc(namelen + 5);
        strncpy(output, argv[1], namelen);
        strcat(output, ".png");
    }
    else
    {
        printf("The correct syntax is: %s input [output]\n", argv[0]);
        exit(-1);
    }
    /* Initializing video subsystem */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL_Init error: %s\n", SDL_GetError());
        exit(-1);
    }
    /* Calling SDL_Quit at exit */
    atexit(SDL_Quit);

    /* setting screen mode, get a screen from SDL */
    SDL_Surface *screen = SDL_SetVideoMode(width, height, 0, 0);

    SDL_WM_SetCaption("SDL_savescreen by Encelo", NULL);

    printf("input file: %s\n", input);
    printf("output file: %s\n", output);

    /* Opening input and output files */
    input_surf = IMG_Load(input);
    if (input_surf == NULL)
    {
        printf("IMG_Load error: %s\n", IMG_GetError());
        exit(-1);
    }
    /* Blitting the surface on the screen*/
    SDL_BlitSurface(input_surf, NULL, screen, NULL);
    SDL_Flip(screen);
    SDL_Delay(500);

    /* Creating the output surface to save */
    output_surf = SDL_CreateRGBSurface(screen->flags, screen->w, screen->h, screen->format->BitsPerPixel, rmask, gmask,
                                       bmask, screen->format->BitsPerPixel <= 24 ? 0 : amask);
    SDL_BlitSurface(screen, NULL, output_surf, NULL);
    if (png_save_surface(output, output_surf) < 0)
    {
        exit(-1);
    }
    /* Quitting */
    if (argc == 2)
    {
        free(output);
    }
    SDL_FreeSurface(output_surf);
    SDL_FreeSurface(input_surf);
    SDL_Quit();

    return 0;
}
