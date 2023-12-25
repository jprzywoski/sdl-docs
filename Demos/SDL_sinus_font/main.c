/* Sinusfont: a simple SDL_ttf based sinus scroller
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
 *
 * NOTE:
 *
 * This program is part of "Mars, Land of No Mercy" SDL examples,
 * you can find others examples on http://mars.sourceforge.net
 */

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define DIM_W		800
#define DIM_H		600
#define CENTER_X	DIM_W / 2
#define CENTER_Y	DIM_H / 2
#define M_PI		3.14159265358979323846  /* some math.h don't define Pi */

int main(int argc, char **argv)
{
    SDL_Surface *screen;
    Uint32 black;
    TTF_Font *font;
    int height, width;
    SDL_Color text_color;
    char *string, *default_str = "Mars: Land of No Mercy";
    int size;
    char letter[2];
    SDL_Surface **letter_surf;
    SDL_Rect *letter_rect;
    char *font_file;
    int i, dir = 1;
    double angle = 0;
    int xpos, first_x;
    int ypos;

    /* Parsing shell parameters */
    if (argc == 1)
    {
        font_file = "Distres2.ttf";
        size = 35;
        string = default_str;
    }
    else if (argc == 4)
    {
        font_file = argv[1];
        if ((sscanf(argv[2], "%d", &size)) != 1)
        {
            printf("The second argument must be an int\n");
            exit(-1);
        }
        string = argv[3];
    }
    else
    {
        printf("The correct syntax is: %s font dimension string\n", argv[0]);
        exit(-1);
    }
    /* Dynamic allocation of structures based on numeber of letters */
    letter_surf = (SDL_Surface **)malloc(sizeof (SDL_Surface *) * strlen(string));
    letter_rect = (SDL_Rect *)malloc(sizeof (SDL_Rect) * strlen(string));
    /* Initializing video subsystem */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL_Init error: %s\n", SDL_GetError());
        exit(-1);
    }
    /* Calling SDL_Quit at exit */
    atexit(SDL_Quit);
    /* Opening a window to draw inside */
    if ((screen = SDL_SetVideoMode(DIM_W, DIM_H, 0, SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL)
    {
        printf("SDL_SetVideoMode error: %s\n", SDL_GetError());
        exit(-1);
    }
    /* Initializing SDL_ttf */
    if (TTF_Init())
    {
        printf("TTF_Init error: %s", TTF_GetError());
        exit(-1);
    }
    /* Opening the font */
    if ((font = TTF_OpenFont(font_file, size)) == NULL)
    {
        printf("TTF_OpenFont error: %s", TTF_GetError());
        exit(-1);
    }
    /* Mapping background and text color */
    black = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);

    text_color.r = 0xf2;
    text_color.g = 0x5b;
    text_color.b = 0x09;

    /* Getting text surface dimension, it will be useful later */
    TTF_SizeText(font, string, &width, &height);
    /* Creating surfaces for every letter */
    for (i = 0; i < strlen(string); i++)
    {
        letter[0] = string[i];
        letter[1] = '\0';
        letter_surf[i] = TTF_RenderText_Blended(font, letter, text_color);
        letter_rect[i].w = letter_surf[i]->w;
        letter_rect[i].h = letter_surf[i]->h;
    }
    /* Vertical text scrolling */
    ypos = screen->h;
    xpos = CENTER_X - width / 2;
    while (ypos > CENTER_Y)
    {
        for (i = 0; i < strlen(string); i++)
        {
            letter_rect[i].x = xpos;
            letter_rect[i].y = ypos;
            xpos += letter_rect[i].w;
            SDL_BlitSurface(letter_surf[i], NULL, screen, &letter_rect[i]);
        }
        xpos = CENTER_X - (width / 2);
        ypos -= 2;

        SDL_Flip(screen);
        SDL_Delay(20);
        for (i = 0; i < strlen(string); i++)
        {
            SDL_FillRect(screen, &letter_rect[i], black);
        }
    }
    /* Sinus scroller */
    first_x = xpos;
    while (angle <= 360 * 8)
    {
        for (i = 0; i < strlen(string); i++)
        {
            if (i == 0)
            {
                xpos = first_x;
            }
            letter_rect[i].x = xpos;
            xpos += letter_rect[i].w;
            ypos = CENTER_Y + sin(M_PI / 180 * (angle + i * 15)) * height;
            letter_rect[i].y = ypos;
            SDL_BlitSurface(letter_surf[i], NULL, screen, &letter_rect[i]);
        }
        angle += 7;

        SDL_Flip(screen);
        SDL_Delay(20);
        /* Bouncing from one screen edge to the other */
        if (xpos > screen->w)
        {
            dir = -1;
        }
        if (first_x < 0)
        {
            dir = 1;
        }
        first_x += dir * 3;
        for (i = 0; i < strlen(string); i++)
        {
            SDL_FillRect(screen, &letter_rect[i], black);
        }
    }
    /* Freeing the surfaces */
    for (i = 0; i < strlen(string); i++)
    {
        SDL_FreeSurface(letter_surf[i]);
    }
    /* Freeing dynamic allocated structures */
    free(letter_surf);
    free(letter_rect);

    /* Closing font and libraries */
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
