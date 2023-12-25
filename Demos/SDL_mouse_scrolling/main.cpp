/*
 * /////////////////////////////////////////////////////////////
 * mouse_scrolling.cpp
 *
 * This program implements a smooth (I hope) scrolling using mouse
 * movements.
 * When the cursor is placed on the left or on the right side of the
 * image, the background starts to scroll forwards the mouse direction
 * (just left and right, it's just an example...).
 *
 * You can try this program using the following options:
 *
 * scrolling [-f][-1|-2|-3|-4]
 *
 * -f -> fullscreen mode
 * -1 -> low use of cpu / bad video performance
 * ...
 * -4 -> high use of cpu / good video performance
 *
 * using the program without options offers the best performance
 * and the max use of cpu.
 *
 *
 * This software is released under GPL license, you can find a
 * copy of this license at http://www.gnu.org/copyleft/gpl.html
 *
 *
 * Last update date:
 *      2004-11-06
 *
 * Author:
 *      Davide Coppola - dmc@dev-labs.net
 *
 * NOTE:
 *
 * This program is part of "Mars, Land of No Mercy" SDL examples,
 * you can find others examples on http://mars.sourceforge.net
 *
 * ///////////////////////////////////////////////////////////////
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#define DIM_H		800
#define DIM_V		600

#define BG_H		400
#define BG_W		2000

#define SCROLLING_MAX	4
#define PIXEL_X_MS	0.15

#define DIST_SCROLL	40

int main(int argc, char *argv[])
{
    /* used surfaces */
    SDL_Surface *screen;
    SDL_Surface *bg;
    SDL_Surface *temp;
    SDL_Surface *cursor;

    /* video system init */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL Error: %s\n", SDL_GetError());
        return 1;
    }
    /* SQL_Quit will manage the exit */
    atexit(SDL_Quit);

    /* a 32 bit integer used to store screen's flags */
    Uint32 flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
    int i;

    /* no parmas are passed (-1..-4) upper frame bound, best video performance/max cpu usage */
    int max_fps = 1000;

    /* options */
    for (i = 1; i < argc; ++i)
    {
        /* -1 low cpu work, bad video performance */
        if (strncmp(argv[i], "-1", 2) == 0)
        {
            max_fps = 80;
        }
        else if (strncmp(argv[i], "-2", 2) == 0)
        {
            max_fps = 90;
        }
        else if (strncmp(argv[i], "-3", 2) == 0)
        {
            max_fps = 100;
        }
        /* -4 high cpu work, good video performance */
        else if (strncmp(argv[i], "-4", 2) == 0)
        {
            max_fps = 110;
        }

        /* -f FULLSCREEN mode */
        else if (strncmp(argv[i], "-f", 2) == 0)
        {
            flags ^= SDL_FULLSCREEN;
        }
    }
    /* SDL_SetVideoMode is used to have screen associated to the monitor */
    if (!(screen = SDL_SetVideoMode(DIM_H, DIM_V, 0, flags)))
    {
        printf("SDL error: %s\n", SDL_GetError());
        return 1;
    }
    /* the system cursor is hide */
    SDL_ShowCursor(0);
    /* the image is loaded on a temporary surface */
    if ((temp = IMG_Load("m_pointer.png")) == NULL)
    {
        printf("SDL error: %s\n", IMG_GetError());
        exit(1);
    }
    /* the  image is adapted to local display so that it can obtains better graphics performance */
    cursor = SDL_DisplayFormat(temp);

    /* free temp surface */
    SDL_FreeSurface(temp);

    /* set the transparent color of cursor surface */
    SDL_SetColorKey(cursor, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(cursor->format, 0xff, 0x0, 0xff));
    /* the image is loaded on a temporary surface */
    if ((temp = IMG_Load("bg.jpg")) == NULL)
    {
        printf("SDL error: %s\n", IMG_GetError());
        exit(1);
    }
    /* the  image is adapted to local display so that it can obtains better graphics performance */
    bg = SDL_DisplayFormat(temp);

    /* free temp surface */
    SDL_FreeSurface(temp);

    SDL_Rect bg_rect;
    SDL_Rect dest_rect;

    /* bg Rect */
    bg_rect.x = 0;
    bg_rect.y = 0;
    bg_rect.w = DIM_H - 1;
    bg_rect.h = BG_H;

    /* dest bg Rect */
    dest_rect.x = 0;
    dest_rect.y = 100;
    dest_rect.w = DIM_H - 1;
    dest_rect.h = BG_H;

    /* variable used to control the game loop */
    int done = 0;

    /* SDL_Event struct used to manage events */
    SDL_Event event;

    /* time values */
    unsigned long t_start, t_end;
    unsigned long t1, t2;
    float dt;

    /* cursor Rect */
    SDL_Rect rect_cur;

    rect_cur.x = 0;
    rect_cur.y = 0;
    rect_cur.w = cursor->w;
    rect_cur.h = cursor->h;

    /* drawn frames */
    Uint32 numframes = 0;

    /* let's go! */
    t1 = t_start = SDL_GetTicks();
    /* game or main loop (as you prefer) */
    while (!done)
    {
        numframes++;

        /* dt is time passed between this frame and the previous */
        t2 = SDL_GetTicks();
        dt = (t2 - t1);
        t1 = t2;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                /* quit if someone press ESC */
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    done = 1;
                }
            }
            else if (event.motion.type == SDL_MOUSEMOTION)
            {
                /* coordinates of rectangle that contains the pointer are updated */
                rect_cur.x = event.button.x;
                rect_cur.y = event.button.y;
                rect_cur.w = cursor->w;
                rect_cur.h = cursor->h;
            }
        }
        /* when cursor is on right part of the screen the bg scroll towards right */
        if (DIM_H - rect_cur.x < DIST_SCROLL & (bg_rect.x + SCROLLING_MAX) < (BG_W - DIM_H))
        {
            bg_rect.x += (int)ceil(PIXEL_X_MS * dt);
        }
        /* when cursor is on left part of the screen the bg scroll towards left */
        if (rect_cur.x < DIST_SCROLL & (bg_rect.x - SCROLLING_MAX) > 0)
        {
            bg_rect.x -= (int)ceil(PIXEL_X_MS * dt);
        }
        /* bg is blitted */
        SDL_BlitSurface(bg, &bg_rect, screen, &dest_rect);

        /* the cursor is blitted on its new position */
        SDL_BlitSurface(cursor, NULL, screen, &rect_cur);

        /* the area is updated */
        SDL_UpdateRect(screen, 0, 100, bg_rect.w, bg_rect.h);

        /*
         * if max_fps is too high, there's a delay
         * NOTE: try different parameters to obtain different video performance/cpu usage
         */
        if (((float)numframes / ((float)(t2 - t_start) / 1000)) > max_fps)
        {
            SDL_Delay(1);
        }
    }
    t_end = SDL_GetTicks();

    /* stats */
    printf("\nAverage FPS: %4.3f\n", (float)numframes * 1000 / ((float)(t_end - t_start)));

    return 0;
}
