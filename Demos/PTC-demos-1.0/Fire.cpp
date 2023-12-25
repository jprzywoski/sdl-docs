/*
 *
 * Fire example: ye olde fire effect, your favourite indexed mode example
 * Copyright (c) Glenn Fiedler (ptc@gaffer.org)
 * Ripped out of PTC 0.72 and adapted for PTC 2.0 by brn (brn@eleet.mcb.at)
 * This source code is licensed under the GNU GPL
 *
 */
#include <stdio.h>
#include <time.h>

/* Both MacOS and BeOS are supported by SDL */
#ifdef __MWERKS__
#define SDL
#endif

#if defined(SDL)

#include "SDLptc.h"

#define GetTicks() SDL_GetTicks()

#else

#include <sys/time.h>
#include <stdlib.h>

#include "ptc.h"

inline int GetTicks(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

#define randomize()	srand(time(NULL))
#define random(max)	(rand() % (max))

#endif /* SDL */

#define RGB32(r, g, b)	(r << 16) | (g << 8) | b

void SetupPalette(int32 palette[])
{
    /* black to red */
    int i = 0;
    int c = 0;

    while (i < 64)
    {
        palette[i] = RGB32(c, 0, 0);
        c += 4;
        i++;
    }
    /* red to yellow */
    c = 0;
    while (i < 128)
    {
        palette[i] = RGB32(255, c, 0);
        c += 4;
        i++;
    }
    /* yellow to white */
    c = 0;
    while (i < 128)
    {
        palette[i] = RGB32(255, 255, c);
        c += 4;
        i++;
    }
    /* white */
    while (i < 256)
    {
        palette[i] = RGB32(255, 255, 255);
        i++;
    }
}

main(int argc, char *argv[])
{
    int flaming;
    int then, now, frames;

    try
    {
        Format format(8);
        Console console;

        console.open("Fire effect demo", 320, 200, format);

        Surface surface(320, 210, format);
        int32 *palette = new int32[256];

        SetupPalette(palette);

        surface.palette(palette);
        console.palette(palette);

        /* setup surface data */
        int pitch = surface.pitch();
        int width = surface.width();    /* note: width must be divisible by 4 */
        int height = surface.height();

        /* flame data */
        int state = 0;
        int intensity = 0;

        /* main loop */
        randomize();
        then = GetTicks();
        frames = 0;
        flaming = 1;
        while (flaming)
        {
            /* exit on keypress */
            if (console.key())
            {
                state = 2;
            }
            /* state machine */
            switch (state)
            {
            case 0:                     /* raise flame */
                intensity += 4587;
                if (intensity > 52429)
                {
                    state = 1;
                }
                break;

            case 1:                     /* constant flame */
                break;

            case 2:                     /* lower flame */
                intensity -= 328;
                if (intensity <= 0)
                {
                    flaming = 0;
                }
                break;
            }

            /* lock surface */
            char8 *buffer = (char8 *)surface.lock();

            /* main flame loop */
            char8 *line = buffer;

            for (int y = 0; y < height - 4; y += 2)
            {
                /* current pixel */
                char8 *pixel = line;

                /* special case: first pixel on line */
                char8 *p = pixel + (pitch << 1);
                int32 top = *p;

                top += *(p + width - 1);
                top += *(p + 1);

                int32 bottom = *(pixel + (pitch << 2));
                int32 c1 = (top + bottom) >> 2;

                if (c1 > 1)
                {
                    c1--;
                }
                int32 c2 = (c1 + bottom) >> 1;

                *pixel = (char8)c1;
                *(pixel + pitch) = (char8)c2;
                pixel++;
                /* main line loop */
                for (int x = 1; x < width - 1; x++)
                {
                    /* sum top pixels */
                    char8 *p = pixel + (pitch << 1);
                    int32 top = *p;

                    top += *(p - 1);
                    top += *(p + 1);

                    /* bottom pixel */
                    int32 bottom = *(pixel + (pitch << 2));

                    /* combine pixels */
                    int32 c1 = (top + bottom) >> 2;

                    if (c1 > 1)
                    {
                        c1--;
                    }
                    /* interpolate */

                    int32 c2 = (c1 + bottom) >> 1;

                    /* store pixels */
                    *pixel = (char8)c1;
                    *(pixel + pitch) = (char8)c2;

                    /* next pixel */
                    pixel++;
                }
                /* special case last pixel on line */
                p = pixel + (pitch << 1);
                top = *p;
                top += *(p - 1);
                top += *(p - width + 1);
                bottom = *(pixel + (pitch << 2));
                c1 = (top + bottom) >> 2;
                if (c1 > 1)
                {
                    c1--;
                }
                c2 = (c1 + bottom) >> 1;
                *pixel = (char8)c1;
                *(pixel + pitch) = (char8)c2;

                /* next line */
                line += pitch * 2;
            }

            /* flame generator bar */
            char8 *generator = buffer + pitch * (height - 4);

            for (int x = 0; x < width; x += 4)
            {
                /* random 4x4 color blocks */
                int32 c = random(intensity >> 8);

                *(generator + 0) = c;
                *(generator + 1) = c;
                *(generator + 2) = c;
                *(generator + 3) = c;
                *(generator + pitch + 0) = c;
                *(generator + pitch + 1) = c;
                *(generator + pitch + 2) = c;
                *(generator + pitch + 3) = c;
                *(generator + pitch * 2 + 0) = c;
                *(generator + pitch * 2 + 1) = c;
                *(generator + pitch * 2 + 2) = c;
                *(generator + pitch * 2 + 3) = c;
                *(generator + pitch * 3 + 0) = c;
                *(generator + pitch * 3 + 1) = c;
                *(generator + pitch * 3 + 2) = c;
                *(generator + pitch * 3 + 3) = c;

                /* next block */
                generator += 4;
            }
            /* unlock surface */
            surface.unlock();

            /* update display */
            surface.copy(console, Area(0, 0, 320, 200), Area(0, 0, 320, 200));

            ++frames;
            console.update();
        }
        /* exit... */
        now = GetTicks();
        if ((now - then) > 0)
        {
            printf("%d frames per second\n", (frames * 1000) / (now - then));
        }
        return 0;
    }
    catch (Error e)
    {
        e.report();
    }

    return 0;
}
