/*
 * "Lightning"
 *
 *   (C) Brian Dolan <bdolan@bu.edu>, 2003
 *
 * This software is released under the terms of the GPL.
 *
 * Contact author for permission if you want to use this
 * software, or work derived from it, under other terms.
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>

/* DEFINES ////////////////////////////////////////////////////////////////// */
#define PI		3.1415
#define SCREEN_W	320
#define SCREEN_H	240

/* the maximum lightning bolt length (and brightness) */
#define MAX_LENGTH	255

/* how often the lightning happens (50 means once out of every 50 frames) */
#define FREQ		50

/* the default brightness */
#define DEFAULT_GAMMA	0.5

/*
 * GLOBALS //////////////////////////////////////////////////////////////////
 * the screen brightness
 */
float l_gamma = DEFAULT_GAMMA;

/* audio stuff */
int audio_rate = 44100;
Uint16 audio_format = AUDIO_S16;
int audio_channels = 2;
int audio_buffers = 4096;
Mix_Chunk *thunder[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

/*
 * GETPIXEL /////////////////////////////////////////////////////////////////
 * Return the pixel value at (x, y)
 * NOTE: The surface must be locked before calling this!
 */
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

/*
 * PUTPIXEL /////////////////////////////////////////////////////////////////
 * Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!
 */
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

/*
 * FADE /////////////////////////////////////////////////////////////////////
 * alters the rgb of a certain pixel by a certain percent
 */
void fade(SDL_Surface *screen, int x, int y, float r_pcnt, float g_pcnt, float b_pcnt)
{
    Uint8 r, g, b;

    SDL_GetRGB(getpixel(screen, x, y), screen->format, &r, &g, &b);
    r += r_pcnt * (float)(255 - r);
    g += g_pcnt * (float)(255 - g);
    b += b_pcnt * (float)(255 - b);
    putpixel(screen, x, y, SDL_MapRGB(screen->format, r, g, b));
}

/*
 * LIGHTNING ///////////////////////////////////////////////////////////////
 * draws the lightning, can be recursive (for forks)
 */
void lightning(SDL_Surface *screen, int x_arg, int y_arg, int length)
{
    float x = x_arg, y = y_arg;
    float angle = PI / 2; /* start pointing downward */
    float percent;
    int i;

    for (i = length; i > 0; i--)
    {
        /* should we fork the bolt here? */
        if (rand() % 100 == 1)
        {
            lightning(screen, x, y, i / 2);
        }
        percent = (float)(i) / MAX_LENGTH;

        /* plot the center white dot */
        fade(screen, x, y, percent, percent, percent);

        /* plot the surrounding blue dots */
        fade(screen, x - 1, y, .3 * percent, .3 * percent, .9 * percent);
        fade(screen, x, y + 1, .3 * percent, .3 * percent, .9 * percent);
        fade(screen, x + 1, y, .3 * percent, .3 * percent, .9 * percent);
        fade(screen, x, y - 1, .3 * percent, .3 * percent, .9 * percent);
        /* bend the angle every few cycles */
        if (rand() % 20 < 10)
        {
            angle += rand() % 3 - 1;
        }
        /* make sure to stay between 0 and 2*PI */
        if (angle > 2 * PI)
        {
            angle -= 2 * PI;
        }
        else if (angle < 0)
        {
            angle += 2 * PI;
        }
        /* now point it more downward */
        if (angle < PI / 2 || angle > 3 * PI / 2)
        {
            angle += rand() % 2;
        }
        else
        {
            angle -= rand() % 2;
        }
        /* make sure to stay between 0 and 2*PI */
        if (angle > 2 * PI)
        {
            angle -= 2 * PI;
        }
        else if (angle < 0)
        {
            angle += 2 * PI;
        }
        /* find the new coordinates */
        x += 1 * cos(angle);
        y += 1 * sin(angle);
        /* do clipping */
        if (x < 1 || x > screen->w - 1)
        {
            break;
        }
    }
}

/* MAIN ////////////////////////////////////////////////////////////////// */
int main(int argc, char *argv[])
{
    SDL_Surface *screen;
    SDL_Surface *temp_image;
    SDL_Surface *back;
    SDL_Event event;
    int flags = SDL_DOUBLEBUF | SDL_SWSURFACE;
    SDL_Rect r;
    int i;
    int audio = 1;

    atexit(SDL_Quit);
    for (i = 1; i < argc; ++i)
    {
        if (strncmp(argv[i], "-na", 3) == 0)
        {
            audio = 0;
        }
        else if (strncmp(argv[i], "-nd", 3) == 0)
        {
            flags &= ~SDL_DOUBLEBUF;
        }
        else if (strncmp(argv[i], "-h", 2) == 0)
        {
            flags |= SDL_HWSURFACE;
            flags &= ~SDL_SWSURFACE;
        }
        else if (strncmp(argv[i], "-f", 2) == 0)
        {
            flags |= SDL_FULLSCREEN;
        }
    }
    if (audio)
    {
        SDL_Init(SDL_INIT_AUDIO);
        if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers))
        {
            printf("Unable to open audio!\n");
            exit(1);
        }
        thunder[0] = Mix_LoadWAV("wavs/1.wav");
        thunder[1] = Mix_LoadWAV("wavs/2.wav");
        thunder[2] = Mix_LoadWAV("wavs/3.wav");
        thunder[3] = Mix_LoadWAV("wavs/4.wav");
        thunder[4] = Mix_LoadWAV("wavs/5.wav");
        thunder[5] = Mix_LoadWAV("wavs/6.wav");
        thunder[6] = Mix_LoadWAV("wavs/7.wav");
        thunder[7] = Mix_LoadWAV("wavs/8.wav");
    }
    SDL_Init(SDL_INIT_VIDEO);

    screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, 0, flags);
    if (!screen)
    {
        fprintf(stderr, "Failed to open screen!\n");
        exit(-1);
    }
    SDL_WM_SetCaption("Lightning", "Lightning");
    if (flags & SDL_FULLSCREEN)
    {
        SDL_ShowCursor(0);
    }
    temp_image = IMG_Load("lightning.png");
    if (!temp_image)
    {
        fprintf(stderr, "Could not load background!\n");
        exit(-1);
    }
    back = SDL_DisplayFormat(temp_image);
    SDL_FreeSurface(temp_image);

    /* start off dark */
    l_gamma = DEFAULT_GAMMA;
    while (1)
    {
        if (SDL_PollEvent(&event) > 0)
        {
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                break;
            }
            if (event.type & (SDL_KEYUP | SDL_KEYDOWN))
            {
                Uint8 *keys = SDL_GetKeyState(&i);

                if (keys[SDLK_ESCAPE])
                {
                    break;
                }
            }
        }
        /* draw the background */
        r.x = 0;
        r.y = 0;
        SDL_BlitSurface(back, NULL, screen, &r);

        /* darken the screen until we're at the default gamma */
        l_gamma = DEFAULT_GAMMA + 0.8 * (l_gamma - DEFAULT_GAMMA);
        SDL_SetGamma(l_gamma, l_gamma, l_gamma);
        /* don't always draw lightning */
        if (rand() % FREQ == 1)
        {
            /* there's lighting - light it up */
            l_gamma = 1;
            SDL_SetGamma(l_gamma, l_gamma, l_gamma);
            /* lock the screen */
            if (SDL_MUSTLOCK(screen))
            {
                if (SDL_LockSurface(screen) < 0)
                {
                    fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
                    return;
                }
            }
            /* BAM SKANKS!!! /////////////////////////////// */
            lightning(screen, rand() % screen->w, screen->h / 8, MAX_LENGTH - rand() % 50);
            if (SDL_MUSTLOCK(screen))
            {
                SDL_UnlockSurface(screen);
            }
            /* play some thunder */
            if (audio)
            {
                Mix_PlayChannel(-1, thunder[rand() % 8], 0);
            }
        }
        SDL_Flip(screen);
        SDL_Delay(100);
    }
    SDL_FreeSurface(back);
    if (audio)
    {
        Mix_CloseAudio();
    }
    exit(0);
}
