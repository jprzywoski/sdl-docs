/***************************************************************************
 *                        main.c  -  fire demo (sdl)
 *                           -------------------
 *  begin                : Son Apr  2 22:58:23 MEST 2000
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <time.h>
#include <SDL/SDL.h>

#include "fire.h"

#define PACKAGE "0"
#define VERSION "0"

int main(int argc, char *argv[])
{
    int screen_width = 320;
    int screen_height = 200;
    SDL_VideoInfo vinfo;
    Uint32 vmodeflags = 0;
    SDL_Surface *screen;
    SDL_Surface *buffer;
    int i;

    // check command line args
    if (argc > 1)
    {
        for (i = 1; i <= argc; i++)
        {
            // help
            if (!strcmp("-h", argv[i - 1]) || !strcmp("-help", argv[i - 1]) || !strcmp("--help", argv[i - 1]) ||
                    !strcmp("-?", argv[i - 1]) || !strcmp("help", argv[i - 1]))
            {
                fprintf(stdout, "%s %s\n", PACKAGE, VERSION);
                fprintf(stdout, "Stephan Uhlmann <su@codewizards.org>\n\n");
                fprintf(stdout, " -h : help (this)\n");
                fprintf(stdout, " -v : version\n");
                fprintf(stdout, " -width <value> : width of the screen in pixels\n");
                fprintf(stdout, " -height <value> : height of the screen in pixels\n");
                fprintf(stdout, " -fullscreen : run in fullscreen video mode if possible\n");
                exit(0);
            }
            // version
            if (!strcmp("-v", argv[i - 1]))
            {
                fprintf(stdout, "%s\n", VERSION);
                exit(0);
            }
            // set width
            if (!strcmp("-width", argv[i - 1]))
            {
                screen_width = atoi(argv[i]);
                if (screen_width < 2)
                {
                    screen_width = 320;
                }
                i++;
            }
            // set height
            if (!strcmp("-height", argv[i - 1]))
            {
                screen_height = atoi(argv[i]);
                if (screen_height < 2)
                {
                    screen_height = 200;
                }
                i++;
            }
            // fullscreen
            if (!strcmp("-fullscreen", argv[i - 1]))
            {
                vmodeflags |= SDL_FULLSCREEN;
                i++;
            }
        }
    }
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        ComplainAndExit();
    }
    atexit(SDL_Quit);

    // autodetect fastest surface
    vinfo = *SDL_GetVideoInfo();

    fprintf(stdout, PACKAGE " "VERSION " by Stephan Uhlmann <su@codewizards.org> \n\n");
    fprintf(stdout, "hw_available: %2lu\n", (unsigned long int)vinfo.hw_available);
    fprintf(stdout, "wm_available: %2lu\n", (unsigned long int)vinfo.wm_available);
    fprintf(stdout, "blit_hw     : %2lu\n", (unsigned long int)vinfo.blit_hw);
    fprintf(stdout, "blit_sw     : %2lu\n", (unsigned long int)vinfo.blit_sw);
    fprintf(stdout, "BitsPerPixel: %2lu\n", (unsigned long int)vinfo.vfmt->BitsPerPixel);
    if (vinfo.hw_available == 0)
    {
        vmodeflags |= SDL_SWSURFACE;
    }
    else if (vinfo.blit_sw == 0)
    {
        vmodeflags |= SDL_HWSURFACE;
    }
    else
    {
        vmodeflags |= SDL_SWSURFACE;    // if hw avail but sw blit ok then sw
    }
    screen = SDL_SetVideoMode(screen_width, screen_height, vinfo.vfmt->BitsPerPixel, vmodeflags);
    if (screen == NULL)
    {
        ComplainAndExit();
    }
    SDL_WM_SetCaption(PACKAGE " "VERSION, NULL);

    // now generate surfce we want
    buffer = SDL_CreateRGBSurface(vmodeflags, screen_width, screen_height, 8, 0, 0, 0, 0);
    if (buffer == NULL)
    {
        ComplainAndExit();
    }
    // generate color palette
    fire_colormap(buffer);

    // start frame counting
    sdldemo_startfps();

    // go!
    fire_run(screen, buffer, screen_width, screen_height);

    // stop frame counting
    sdldemo_stopfps();
    fprintf(stdout, "fps         : %2lu\n", (unsigned long)sdldemo_getfps());

    // free memory
    SDL_FreeSurface(buffer);
    SDL_FreeSurface(screen);

    exit(0);
}
