/* game_lib.c - sprite libs, etc
 *
 * copyright 06/26/2001
 * dave blood - geekd@yahoo.com
 *
 * some chunks of code taken from SDL examples
 * http://www.libsdl.org/
 *
 * other code where credited
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "gamelib.h"
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>

/* This function may run in a separate event thread */
int FilterEvents(const SDL_Event *event)
{
    if (event->type == SDL_MOUSEMOTION)
    {
        return 0;    /* Drop it, we've handled it */
    }
    return 1;
}

SDL_Surface *init_sdl(int width, int height, char *title, const char *win_icon)
{
    SDL_Surface *screen;
    SDL_Surface *icon;
    int audio_rate;
    Uint16 audio_format;
    int audio_channels;

    /* Initialize SDL */
#ifndef NOSOUND
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "SDL error: %s\n", SDL_GetError());
        exit(1);
    }
#endif
#ifdef NOSOUND
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL error: %s\n", SDL_GetError());
        exit(1);
    }
#endif

    atexit(CleanUp);

    icon = IMG_Load(win_icon);
    SDL_WM_SetIcon(icon, NULL);

    screen = SDL_SetVideoMode(width, height,    /* width and height */
                              16,                                     /* color depth */
                              SDL_HWSURFACE | SDL_DOUBLEBUF           /* video flags */
                             );
    if (screen == NULL)
    {
        fprintf(stderr, "Video error: %s\n", SDL_GetError());
        exit(1);
    }
    /* set window title */

    SDL_WM_SetCaption(title, NULL);

    /* set input filter */
    SDL_SetEventFilter(FilterEvents);

    /* initialize Audio */

    /* Initialize variables */
    audio_rate = MIX_DEFAULT_FREQUENCY;
    audio_format = MIX_DEFAULT_FORMAT;
    audio_channels = 2;

    /* Open the audio device */
#ifndef NOSOUND
    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, 512) < 0)
    {
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        exit(2);
    }
    else
    {
        Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
        printf("Opened audio at %d Hz %d bit %s", audio_rate, (audio_format & 0xFF), (audio_channels > 1) ? "stereo" :
               "mono");
    }
    /* done w/ audio */
#endif

    return screen;
}

void new_sprite(Sprite *temp, const char *filename, int x, int y, int transparent, int alpha)
{
    SDL_Surface *new;

    temp->x = x;
    temp->y = y;

    /* load image */

    new = IMG_Load(filename);
    if (new == NULL)
    {
        fprintf(stderr, "Couldn't load %s: %s\n", filename, SDL_GetError());
        exit(1);
    }
    /* set transparent color */
    if (transparent == 1)
    {
        SDL_SetColorKey(new, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
    }
    if (alpha)
    {
        SDL_SetAlpha(new, SDL_SRCALPHA | SDL_RLEACCEL, alpha);
    }
    /* convert the images to match the surface for speedier blitting */

    temp->image = SDL_DisplayFormat(new);

    SDL_FreeSurface(new);

    temp->w = (temp->image)->w;
    temp->h = (temp->image)->h;

    temp->x_vel = 0;
    temp->y_vel = 0;
}

void new_sprite_surface(Sprite *temp, SDL_Surface *screen, int x, int y, int transparent)
{
    SDL_Surface *new;

    temp->x = x;
    temp->y = y;

    /* load image */

    new = screen;
    /* set transparent color */
    if (transparent == 1)
    {
        SDL_SetColorKey(new, SDL_SRCCOLORKEY | SDL_RLEACCEL, 0);
    }
    /* convert the images to match the surface for speedier blitting */

    temp->image = SDL_DisplayFormat(new);

    SDL_FreeSurface(new);

    temp->w = (temp->image)->w;
    temp->h = (temp->image)->h;

    temp->x_vel = 0;
    temp->y_vel = 0;
}

#ifndef NOSOUND
Mix_Chunk *new_audio(char *filename)
{
    Mix_Chunk *wave;

    /* Load the requested wave file */
    wave = Mix_LoadWAV(filename);
    if (wave == NULL)
    {
        fprintf(stderr, "Couldn't load %s: %s\n", filename, SDL_GetError());
        exit(1);
    }
    return wave;
}

#endif

void draw_sprite(SDL_Surface *screen, Sprite obj)
{
    SDL_Rect dest;

    dest.x = obj.x;
    dest.y = obj.y;
    dest.w = obj.w;
    dest.h = obj.h;

    SDL_BlitSurface(obj.image, NULL, screen, &dest);
}

#ifndef NOSOUND
void play_audio(Mix_Chunk *wave, int channel)
{
    Mix_PlayChannel(channel, wave, 0);
}

#endif

/*  Full object-to-object pixel-level collision detector
 *  from: http://gamedev.net/reference/articles/article735.asp
 *  by John Amato - adapted to SDL and this game lib by me (geekd)
 */
short int collision_detect_perfect(Sprite obj1, Sprite obj2)
{
    int left1, left2, over_left;
    int right1, right2, over_right;
    int top1, top2, over_top;
    int bottom1, bottom2, over_bottom;
    int over_width, over_height;
    int i, j;
    int pixel1x, pixel2x;
    int pixel1y, pixel2y;

    left1 = obj1.x;
    left2 = obj2.x;
    right1 = obj1.x + obj1.w;
    right2 = obj2.x + obj2.w;
    top1 = obj1.y;
    top2 = obj2.y;
    bottom1 = obj1.y + obj1.h;
    bottom2 = obj2.y + obj2.h;
    /* Trivial rejections: */
    if (bottom1 < top2)
    {
        return 0;
    }
    if (top1 > bottom2)
    {
        return 0;
    }
    if (right1 < left2)
    {
        return 0;
    }
    if (left1 > right2)
    {
        return 0;
    }
    /* Ok, compute the rectangle of overlap: */
    if (bottom1 > bottom2)
    {
        over_bottom = bottom2;
    }
    else
    {
        over_bottom = bottom1;
    }
    if (top1 < top2)
    {
        over_top = top2;
    }
    else
    {
        over_top = top1;
    }
    if (right1 > right2)
    {
        over_right = right2;
    }
    else
    {
        over_right = right1;
    }
    if (left1 < left2)
    {
        over_left = left2;
    }
    else
    {
        over_left = left1;
    }
    over_width = (over_right - over_left);
    over_height = (over_bottom - over_top);

    /* Now compute starting offsets into both objects' bitmaps: */

    pixel1y = (over_top - obj1.y);
    pixel2y = (over_top - obj2.y);

    pixel1x = (over_left - obj1.x);
    pixel2x = (over_left - obj2.x);

    /*
     * Now start scanning the whole rectangle of overlap,
     * checking the corresponding pixel of each object's
     * bitmap to see if they're both non-zero:
     */

    SDL_LockSurface(obj1.image);
    SDL_LockSurface(obj2.image);
    for (i = 0; i < over_height; i++)
    {
        for (j = 0; j < over_width; j++)
        {
            if (((getpixel(obj1.image, pixel1x + j, pixel1y + i)) > 0) && ((getpixel(obj2.image, pixel2x + j, pixel2y +
                    i)) > 0))
            {
                SDL_UnlockSurface(obj1.image);
                SDL_UnlockSurface(obj2.image);
                return 1;
            }
        }
    }
    /*
     * Worst case!  We scanned through the whole darn rectangle of overlap
     * and couldn't find a single colliding pixel!
     */

    SDL_UnlockSurface(obj1.image);
    SDL_UnlockSurface(obj2.image);

    return 0;
}

/*
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

void CleanUp(void)
{
#ifndef NOSOUND
    Mix_CloseAudio();
#endif

    SDL_Quit();
}

void press_any_key(SDLKey quit_key)
{
    SDL_Event event;
    int quit = 0;

    while (!quit)
    {
        /* wait for keypress */

        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT)
        {
            exit(0);
        }
        if (event.type == SDL_KEYDOWN)
        {
            quit = 1;
            if (quit_key != NULL)
            {
                if (event.key.keysym.sym == quit_key)
                {
                    exit(0);
                }
            }
        }
    }                                   /* end while */
}
