/* stars.c		by Nathan Strong
 *
 * based on stars.bas, written by a friend of mine who goes by 'beek' in
 * the world of the internet :-) There is no copyright, no restrictions,
 * and most definitely no warranty on this software. I won't guarantee
 * that it will compile, or that it will not cause damage to your system
 * or your hardware. It's not like this is incredibly complicated code,
 * anyway, so it's your own fault for not checking the code first!
 *
 * You can do anything you want with this code. Use it in your next demo!
 * print it out and use it as wallpaper! Write me flamemail telling me how
 * much I suck at programming (my e-mail is gblues@gstis.net)!
 *
 * The only thing I ask is for credit, even if it's a "This portion
 * based on stars.c by Nathan Strong" buried deep in the source code
 * that nobody will ever see. I was nice enough to credit my friend beek
 * in the source, you can do the same for me :)
 *
 * You're still reading this? What's the matter with you? You must be
 * a lawyer or something :)
 */
/* Responsible for bad reformatting : <andreas.beck@ggi-project.org> */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <SDL/SDL.h>

static SDL_Surface *video;
static Uint32 colors[256];
static int xoff, yoff;
static double angle = 0.0;
static int frames = 0;
static int stars[100][3];
static int starsr[100][3];
static int old[100][3];

void InitGraphics(Uint32 flags);
void InitStars(void);
void CleanUp(void);
void Transform(int *ta, int *tb);

void fast_putpixel1(SDL_Surface *dst, Uint16 x, Uint16 y, Uint32 pixel)
{
    *((Uint8 *)dst->pixels + y * dst->pitch + x) = pixel;
}

void fast_putpixel2(SDL_Surface *dst, Uint16 x, Uint16 y, Uint32 pixel)
{
    *((Uint16 *)dst->pixels + y * dst->pitch / 2 + x) = pixel;
}

void fast_putpixel3(SDL_Surface *dst, Uint16 x, Uint16 y, Uint32 pixel)
{
    Uint8 *pix = (Uint8 *)dst->pixels + y * dst->pitch + x * 3;

    if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    {
        pix[2] = pixel;
        pix[1] = pixel >> 8;
        pix[0] = pixel >> 16;
    }
    else
    {
        pix[0] = pixel;
        pix[1] = pixel >> 8;
        pix[2] = pixel >> 16;
    }
}

void fast_putpixel4(SDL_Surface *dst, Uint16 x, Uint16 y, Uint32 pixel)
{
    *((Uint32 *)dst->pixels + y * dst->pitch / 4 + x) = pixel;
}

void (*fast_putpixel)(SDL_Surface *dst, Uint16 x, Uint16 y, Uint32 pixel);

main(int argc, char *argv[])
{
    int i;
    int x, y, oldx, oldy;
    double fps = 0.0;
    time_t st;
    Uint8 color = 0;

    if (argv[1] && (strcmp(argv[1], "-hw") == 0))
    {
        InitGraphics(SDL_HWSURFACE);
    }
    else
    {
        InitGraphics(SDL_SWSURFACE);
    }
    InitStars();
    st = time(NULL);
    do
    {
        if (SDL_MUSTLOCK(video))
        {
            if (SDL_LockSurface(video) < 0)
            {
                continue;
            }
        }
        for (i = 0; i < 100; i++)
        {
            old[i][0] = starsr[i][0];
            old[i][1] = starsr[i][1];
            old[i][2] = starsr[i][2];

            starsr[i][0] = stars[i][0];
            starsr[i][1] = stars[i][1];
            starsr[i][2] = stars[i][2];

            Transform(&starsr[i][1], &starsr[i][2]);
            Transform(&starsr[i][0], &starsr[i][2]);
            Transform(&starsr[i][0], &starsr[i][1]);

            oldx = (int)floor(((256 * old[i][0]) / (old[i][2] - 1024)) + xoff);
            oldy = (int)floor(((256 * old[i][1]) / (old[i][2] - 1024)) + yoff);

            x = (int)floor(((256 * starsr[i][0]) / (starsr[i][2] - 1024)) + xoff);
            y = (int)floor(((256 * starsr[i][1]) / (starsr[i][2] - 1024)) + yoff);

            color = floor((starsr[i][2] + 721) / 5.5);
            (*fast_putpixel)(video, oldx, oldy, colors[0]);
            (*fast_putpixel)(video, x, y, colors[color]);
        }
        if (SDL_MUSTLOCK(video))
        {
            SDL_UnlockSurface(video);
        }
        SDL_UpdateRect(video, 0, 0, 0, 0);
        frames++;
        angle += 0.5;
    }
    while (SDL_PollEvent(NULL) == 0);
    if ((time(NULL) - st) != 0)
    {
        fps = (double)frames / (time(NULL) - st);
    }
    printf("%2.2f frames per second\n", fps);
    printf("beek / lightspeed\n\nPorted to GGI by Nathan Strong\n");
    printf("Ported to SDL by Sam Lantinga\n");
    CleanUp();
    return 0;
}

void Transform(int *ta, int *tb)
{
    double y = 0.0, z = 0.0;

    y = (cos((angle / 20)) * *ta) - (sin((angle / 20)) * *tb);
    z = (sin((angle / 20)) * *ta) + (cos((angle / 20)) * *tb);
    *ta = (int)floor(y);
    *tb = (int)floor(z);
}

void CleanUp(void)
{
    exit(0);
}

void InitStars(void)
{
    int i;

    srand(time(NULL));
    for (i = 0; i < 100; i++)
    {
        stars[i][0] = ((rand() % 320) + 1 - 160) * 3;
        stars[i][1] = ((rand() % 320) + 1 - 160) * 3;
        stars[i][2] = ((rand() % 128) + 1 - 64) * 5;
    }
}

void InitGraphics(Uint32 video_flags)
{
    int i;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);

    video = SDL_SetVideoMode(640, 480, 0, video_flags);
    if (video == NULL)
    {
        fprintf(stderr, "Couldn't set a 640x480 video mode: %s\n", SDL_GetError());
        exit(1);
    }
    printf("Set %dx%dx%d video mode\n", video->w, video->h, video->format->BitsPerPixel);
    xoff = video->w / 2;
    yoff = video->h / 2;
    /* Assuming 8-bit palette */
    if (video->format->palette)
    {
        SDL_Color clut[256];

        for (i = 0; i < 256; i++)
        {
            clut[i].r = i;
            clut[i].g = i;
            clut[i].b = i;
        }
        SDL_SetColors(video, clut, 0, 256);
    }
    for (i = 0; i < 256; i++)
    {
        colors[i] = SDL_MapRGB(video->format, i, i, i);
    }
    switch (video->format->BytesPerPixel)
    {
    case 1:
        fast_putpixel = fast_putpixel1;
        break;
    case 2:
        fast_putpixel = fast_putpixel2;
        break;
    case 3:
        fast_putpixel = fast_putpixel3;
        break;
    case 4:
        fast_putpixel = fast_putpixel4;
        break;
    default:
        fprintf(stderr, "Unknown video bytes-per-pixel!\n");
        exit(1);
    }
    /* We ignore all but keyboard events */
    for (i = 0; i < SDL_NUMEVENTS; ++i)
    {
        if ((i != SDL_KEYDOWN) && (i != SDL_QUIT))
        {
            SDL_EventState(i, SDL_IGNORE);
        }
    }
}
