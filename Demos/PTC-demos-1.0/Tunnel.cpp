/*
 * Tunnel example
 * Copyright (c) Glenn Fiedler (ptc@gaffer.org)
 * Originally coded by Thomas Rizos (rizos@swipnet.se)
 * This source is licensed under the GNU GPL
 */
/* include files */
#include <math.h>
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

#include "ptc.h"

inline int GetTicks(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

#endif /* SDL */

/* useful pi constant */
const float t_pi = 3.1415926f;

/* tunnel class */
class Tunnel
{
public:
    Tunnel()
    {
        /* allocate tables */
        tunnel = new int32[64000];
        texture = new char8[65536 * 2];

        /* setup tables */
        setup_tunnel();
        setup_texture();
    }
    ~Tunnel()
    {
        /* free tables */
        delete tunnel;
        delete texture;
    }
    void setup_tunnel()
    {
        /* tunnel index */
        int index = 0;

        /* generate tunnel table */
        for (int y = 100; y > -100; y--)
        {
            for (int x = -160; x < 160; x++)
            {
                /* calculate angle from center */
                double angle = atan2(y, x) * 256 / t_pi / 2;

                /* calculate radius from center */
                double radius = sqrt(x * x + y * y);

                /* texture coordinates */
                double u = angle;
                double v = 6000 / radius;

                /* calculate texture index for (u,v) */
                tunnel[index++] = ((int32)v & 0xFF) * 256 + ((int32)u & 0xFF);
            }
        }
    }
    void setup_texture()
    {
        /* array index */
        int index = 0;

        /* generate blue plasma texture */
        float angle2 = t_pi * 2 / 256 * 230;

        for (int y = 0; y < 256 * 2; y++)
        {
            float angle1 = t_pi * 2 / 256 * 100;

            for (int x = 0; x < 256; x++)
            {
                texture[index++] = (char8)((float)(sin(angle1) * 80 + sin(angle2) * 40) + 128);
                angle1 += t_pi * 2 / 256 * 3;
            }
            angle2 += t_pi * 2 / 256 * 2;
        }
    }
    void draw(int32 *buffer, float t)
    {
        /* tunnel control functions */
        int x = (int)(sin(t) * 99.9);
        int y = (int)(t * 200);

        /* calculate tunnel scroll offset */
        int scroll = ((y & 0xFF) << 8) + (x & 0xFF);

        /* loop through each pixel */
        for (int i = 0; i < 64000; i++)
        {
            /* lookup tunnel texture */
            buffer[i] = texture[tunnel[i] + scroll];
        }
    }
private:
    /* tunnel data */
    int32 *tunnel;
    char8 *texture;
};

main(int argc, char *argv[])
{
    int then, now, frames;

    try
    {
        /* create format */
        Format format(32, 0x00FF0000, 0x0000FF00, 0x000000FF);

        /* create console */
        Console console;

        console.open("tunnel", 320, 200, format);

        /* create surface */
        Surface surface(320, 200, format);

        /* create tunnel */
        Tunnel tunnel;

        /* time data */
        float time = 0;
        float delta = 0.03f;

        /* loop until a key is pressed */
        then = GetTicks();
        frames = 0;
        while (!console.key())
        {
            /* lock surface */
            int32 *buffer = (int32 *)surface.lock();

            /* draw tunnel */
            tunnel.draw(buffer, time);

            /* unlock surface */
            surface.unlock();

            /* copy to console */
            surface.copy(console);

            /* update console */
            console.update();

            /* update time */
            time += delta;
            ++frames;
        }
        /* exit */
        now = GetTicks();
        if ((now - then) > 0)
        {
            printf("%d frames per second\n", (frames * 1000) / (now - then));
        }
        return 0;
    }
    catch (Error error)
    {
        /* report error */
        error.report();
    }
}
