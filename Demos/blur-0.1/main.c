/***************************************************************************
 *                                 main.c
 *                           -------------------
 *  begin                : Mon Feb 19 2001
 *  copyright            : (C) 2001 by Stephan Uhlmann
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
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_thread.h>

#include "sdldemotools.h"
#include "blur.h"

#define PACKAGE "0"
#define VERSION "0"

int main(int argc, char *argv[])
{
    SDL_VideoInfo vinfo;
    int screen_width = 640;
    int screen_height = 400;
    Uint32 vmodeflags = 0;
    SDL_Surface *screen;
    int i;
    char *musicfile = NULL;

    /*
     * vmodeflags=SDL_FULLSCREEN;
     * check command line args
     */
    if (argc > 1)
    {
        for (i = 1; i <= argc; i++)
        {
            /* help */
            if (!strcmp("-h", argv[i - 1]) || !strcmp("-help", argv[i - 1]) || !strcmp("--help", argv[i - 1]) ||
                    !strcmp("-?", argv[i - 1]) || !strcmp("help", argv[i - 1]))
            {
                fprintf(stdout, "%s %s\n", PACKAGE, VERSION);
                fprintf(stdout, "Stephan Uhlmann <su@codewizards.org>\n\n");
                fprintf(stdout, " -h : help (this)\n");
                fprintf(stdout, " -v : version\n");
                fprintf(stdout, " -window : run in window (not fullscreen)\n");
                fprintf(stdout,
                        " -music <file> : use specified music file (MOD, XM, IT, S3M) instead of the included one\n");
                exit(0);
            }
            /* version */
            if (!strcmp("-v", argv[i - 1]))
            {
                fprintf(stdout, "%s\n", VERSION);
                exit(0);
            }
            /* window */
            if (!strcmp("-window", argv[i - 1]))
            {
                vmodeflags ^= SDL_FULLSCREEN;
            }
            /* window */
            if (!strcmp("-music", argv[i - 1]))
            {
                musicfile = argv[i];
                i++;
            }
        }
    }
    /* init video */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
    {
        ComplainAndExit();
    }
    atexit(SDL_Quit);

    /* autodetect fastest surface */
    vinfo = *SDL_GetVideoInfo();

    fprintf(stdout, PACKAGE " "VERSION " by Stephan Uhlmann <su@codewizards.org> \n\n");
    fprintf(stdout, "SDL version %d.%d.%d\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);
    fprintf(stdout, "hw_available: %2lu\n", (unsigned long int)vinfo.hw_available);
    fprintf(stdout, "wm_available: %2lu\n", (unsigned long int)vinfo.wm_available);
    fprintf(stdout, "blit_hw     : %2lu\n", (unsigned long int)vinfo.blit_hw);
    fprintf(stdout, "blit_sw     : %2lu\n", (unsigned long int)vinfo.blit_sw);
    fprintf(stdout, "BitsPerPixel: %2lu\n", (unsigned long int)vinfo.vfmt->BitsPerPixel);
    fprintf(stdout, "\n");
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
        vmodeflags |= SDL_SWSURFACE;    /* if hw avail but sw blit ok then sw */
    }
    screen = SDL_SetVideoMode(screen_width, screen_height, vinfo.vfmt->BitsPerPixel, vmodeflags);
    if (screen == NULL)
    {
        ComplainAndExit();
    }
    /* now generate surface we want */
    vmodeflags |= SDL_SRCALPHA;
    blur_buffer = SDL_CreateRGBSurface(vmodeflags, screen_width, screen_height, 32, 0, 0, 0, 0);
    if (blur_buffer == NULL)
    {
        ComplainAndExit();
    }
    SDL_SetAlpha(blur_buffer, SDL_SRCALPHA | SDL_RLEACCEL, 128);

    /* another one */
    blur_top = SDL_CreateRGBSurface(vmodeflags, screen_width, 70, 32, 0, 0, 0, 0);
    if (blur_top == NULL)
    {
        ComplainAndExit();
    }
    SDL_SetAlpha(blur_top, SDL_SRCALPHA | SDL_RLEACCEL, 128);
    /* init audio: 22kHz, 16bit (signed values), stereo */
    if (Mix_OpenAudio(22050, AUDIO_S16, 2, 512) < 0)
    {
        ComplainAndExit();
    }
    blur_music = NULL;
    if (musicfile == NULL)
    {
        blur_music = Mix_LoadMUS("FlkFace.s3m");
    }
    else
    {
        blur_music = Mix_LoadMUS(musicfile);
    }
    if (blur_music == NULL)
    {
        fprintf(stderr, "Couldn't load music: %s\n", Mix_GetError());
    }
    /* callback function for mixer */
    Mix_SetPostMix(blur_mix_callback, 0);

    blur_back = SDL_DisplayFormat(IMG_Load("back.jpg"));
    blur_font = SDL_DisplayFormatAlpha(IMG_Load("broken5.png"));

    /* if we run in window set a caption */
    SDL_WM_SetCaption(PACKAGE " "VERSION, NULL);

    /* hide ugly mouse cursor */
    SDL_ShowCursor(0);

    /* init fft with 256 points */
    InitializeFFT(256);

    /* start frame counting */
    sdldemo_startfps();

    /* here we go */
    blur_run(screen);

    /* stop frame counting */
    sdldemo_stopfps();
    fprintf(stdout, "fps:%6.2f\n", sdldemo_getfps());

    EndFFT();

    /* free memory */
    SDL_FreeSurface(screen);
    SDL_FreeSurface(blur_buffer);
    /* Mix_CloseAudio(); */
    SDL_ShowCursor(1);
    exit(0);
}
