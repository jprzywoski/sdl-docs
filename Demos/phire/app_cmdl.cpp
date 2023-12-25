// phire app_cmdl.cpp - Parse command line arguments
// Copyright (C) 2005, Ertugrul Soeylemez
//
// This program is free software distributed under the terms of the
// GNU General Public License version 2. See the file README.

#include <SDL/SDL.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include "app.h"

// Invocation name
char *prog = "phire";

// Print version info
static void putver()
{
    printf("phire v%s\n", VER);
    puts("Copyright (C) 2005, Ertugrul Soeylemez\n");
    puts("This program is free software distributed under the terms");
    puts("of the GNU General Public License version 2. Please see");
    puts("the README file to learn more.");
}

// Print usage help
static void putusage()
{
    putver();
    printf("\n%s [OPTIONS]\n\n", prog);
    puts("Options:");
    puts("-f  Start fullscreen");
    puts("-H  Use hardware surface");
    puts("-h  This help");
    puts("-S  Use software surface");
    puts("-V  Version information");
    puts("-x  Horizontal resolution (def: 160)");
    puts("-y  Vertical resolution (def: 240)");
    puts("\nPlease see the README file for more detailed information or");
    puts("if you want to report a bug.");
}

// Parse command line arguments
int app_cmdl(int argc, char **argv)
{
    int opt;
    int tmp;

    if (argc < 1)
    {
        return 0;
    }
    prog = argv[0];
    while ((opt = getopt(argc, argv, "fHhSVx:y:")) != -1)
    {
        switch (opt)
        {
        case 'f':
            sparam |= SDL_FULLSCREEN;
            break;

        case 'H':
            if (sparam & SDL_SWSURFACE)
            {
                debug("-H and -S are mutually exclusive.");
                return 2;
            }
            sparam |= SDL_HWSURFACE;
            break;

        case 'h':
            putusage();
            return 1;

        case 'S':
            if (sparam & SDL_HWSURFACE)
            {
                debug("-H and -S are mutually exclusive.");
                return 2;
            }
            sparam |= SDL_SWSURFACE;
            break;

        case 'V':
            putver();
            return 1;

        case 'x':
            tmp = atoi(optarg);
            if (tmp < 16)
            {
                debug("-x resolution invalid (must be integer and >16): %s", optarg);
                return 2;
            }
            mx = tmp;
            break;

        case 'y':
            tmp = atoi(optarg);
            if (tmp < 16)
            {
                debug("-y resolution invalid (must be integer and >16): %s", optarg);
                return 2;
            }
            my = tmp;
            break;

        default:
            return 2;
        }
    }
    return 0;
}
