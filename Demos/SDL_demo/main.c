/* SDL_Demo: a little demo based on SDL direct pixel blitting
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
 */

#include <stdlib.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include "libsdl_draw.h"

int demo(void *data);                   /* The demo thread */

SDL_Surface *screen;
int quit = 0;

int main(int argc, char **argv)
{
    SDL_Thread *thread;
    SDL_Event event;
    Mix_Music *music;

    /* Audio and video init */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("SDL_init() error: %s\n", SDL_GetError());
        exit(-1);
    }
    /* At program exit SDL_Quit will make a clean up */
    atexit(SDL_Quit);
    /* Opening a window */
    if ((screen = SDL_SetVideoMode(640, 480, 0, 0)) == NULL)
    {
        printf("SDL_SetVideoMode() error: %s\n", SDL_GetError());
        exit(-1);
    }
    SDL_WM_SetCaption("SDL_Demo by Encelo", NULL);
    /* Init of SDL_ttf */
    if (TTF_Init())
    {
        printf("TTF_init error: %s", TTF_GetError());
        exit(-1);
    }
    /* Opening the audio device */
    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        printf("Mix_OpenAudio() error: %s\n", Mix_GetError());
        exit(-1);
    }
    /* Allocating only the channels we need */
    Mix_AllocateChannels(1);
    /* Loading the music */
    if ((music = Mix_LoadMUS("purple-shades.mod")) == NULL)
    {
        printf("Music loading error : %s\n", Mix_GetError());
    }
    Mix_PlayMusic(music, -1);

    /* Main demo thread */
    thread = SDL_CreateThread(demo, NULL);
    /* User events handling cycle */
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quit = 1;
                break;
            }
            if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_q)
            {
                quit = 1;
            }
        }
        /* Let's the cpu breath between a poll and another*/
        SDL_Delay(50);
    }
    /* Freeing the music, killing the thread and closing all libs */
    SDL_KillThread(thread);
    Mix_FreeMusic(music);
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();

    return 0;
}

Uint32 black, yellow, violet, blue;

int demo(void *data)
{
    int i;
    int center_x, center_y, x, y;
    float point[1][2] = { 1, 0 };
    float radius[1][2] = { 1, 0 };
    float triangle[3][2] =
    {
        { 0, -1 }, { -1, 1 }, { 1, 1 }
    };
    float square[4][2] =
    {
        { -1, -1 }, { -1, 1 }, { 1, 1 }, { 1, -1 }
    };

    /* Map the colors to this display (R=0xff, G=0xFF, B=0x00) */
    yellow = SDL_MapRGB(screen->format, 0xff, 0xff, 0x00);
    violet = SDL_MapRGB(screen->format, 0xff, 0x00, 0xff);
    blue = SDL_MapRGB(screen->format, 0x00, 0xff, 0xff);
    black = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);

    /* Font related data */
    TTF_Font *font;
    SDL_Color text_color;
    SDL_Surface *text;
    SDL_Rect text_rect;

    text_color.r = 0xf2;
    text_color.g = 0x5b;
    text_color.b = 0x09;

    /* Get the center of the screen */
    x = center_x = screen->w / 2;
    y = center_y = screen->h / 2;

    font = TTF_OpenFont("Keith-h.ttf", 25);

    /* ---------- */

    /* TEXT */
    draw_text(font, "Encelo's SDL_Demo (C)2004", text_color, TEXT_CENTER, TEXT_CENTER, &text_rect);
    SDL_Delay(1000);
    fade_out(2000, 20);

    /* TEXT */
    draw_text(font, "Let's start!", text_color, TEXT_CENTER, TEXT_CENTER, &text_rect);
    SDL_Delay(2000);
    erase_text(&text_rect);

    /* TEXT */
    draw_text(font, "The simpler thing, a point...", text_color, TEXT_CENTER, TEXT_CENTER, &text_rect);
    SDL_Delay(2000);
    erase_text(&text_rect);

    /* ---------- */

    scale(50, point, 1);
    /* Point rotation */
    for (i = 0; i <= 36; i++)
    {
        draw(x, y, point, 1, yellow);
        SDL_Delay(25);
        draw(x, y, point, 1, black);
        rotate(5, point, 1);
    }
    /* Point rotation with tracks (CIRCLE) */
    for (i = 0; i <= 72; i++)
    {
        draw(x, y, point, 1, yellow);
        SDL_Delay(25);
        rotate(5, point, 1);
    }
    /* Clearing the circle */
    for (i = 0; i <= 72; i++)
    {
        draw(x, y, point, 1, black);
        SDL_Delay(25);
        rotate(5, point, 1);
    }
    /* ---------- */

    /* TEXT */
    draw_text(font, "Now? A triangle!", text_color, TEXT_CENTER, TEXT_CENTER, &text_rect);
    SDL_Delay(1000);
    fade_out(1000, 10);

    /* Restoring centre of the screen coordinates */
    x = center_x;
    y = center_y;

    scale(50, triangle, 3);
    /* Triangle rotation around its center (VERTICES) */
    for (i = 0; i <= 36 * 2; i++)
    {
        draw(x, y, triangle, 3, yellow);
        SDL_Delay(25);
        draw(x, y, triangle, 3, black);
        rotate(10, triangle, 3);
    }
    /* Triangle rotation around its center (POLY) */
    for (i = 0; i <= 36 * 2; i++)
    {
        draw_poly(x, y, triangle, 3, yellow);
        SDL_Flip(screen);
        SDL_Delay(25);
        draw_poly(x, y, triangle, 3, black);
        rotate(10, triangle, 3);
    }
    scale(100, radius, 1);
    /* Both triangle and its center rotation (POLY) */
    for (i = 0; i <= 36 * 2; i++)
    {
        draw_poly(x, y, triangle, 3, yellow);
        SDL_Flip(screen);
        SDL_Delay(25);
        draw_poly(x, y, triangle, 3, black);
        rotate(10, triangle, 3);
        rotate(5, radius, 1);
        x = center_x + radius[0][X];
        y = center_y + radius[0][Y];
    }
    SDL_Flip(screen);                   /* Clearing the triangle */

    /* ---------- */

    /* TEXT */
    draw_text(font, "Now have a look at some squares!", text_color, TEXT_CENTER, TEXT_CENTER, &text_rect);
    SDL_Delay(1000);
    fade_out(1000, 10);

    /* Restoring centre of the screen coordinates */
    x = center_x;
    y = center_y;

    scale(50, square, 4);
    /* Square rotation around its center (VERTICES) */
    for (i = 0; i <= 36 * 2; i++)
    {
        draw(x - 100, y, square, 4, yellow);
        draw(x + 100, y, square, 4, yellow);
        SDL_Delay(25);
        draw(x - 100, y, square, 4, black);
        draw(x + 100, y, square, 4, black);
        rotate(10, square, 4);
    }

    float square2[4][2];
    float square3[4][2];

    copy(square, square2, 4);
    copy(square, square3, 4);
    /* Square rotation around its center (POLY) */
    for (i = 0; i <= 36 * 2; i++)
    {
        draw_poly(x + 50, y, square, 4, yellow);
        draw_poly(x - 50, y, square2, 4, violet);
        draw_poly(x, y - 100, square3, 4, blue);
        SDL_Flip(screen);
        SDL_Delay(25);
        draw_poly(x + 50, y, square, 4, black);
        draw_poly(x - 50, y, square2, 4, black);
        draw_poly(x, y - 100, square3, 4, black);
        rotate(10, square, 4);
        rotate(-10, square2, 4);
        rotate(10, square3, 4);
    }
    draw_poly(x, y, square, 4, black);

    float radius2[1][2];

    copy(radius, radius2, 1);

    int x2 = x;
    int y2 = y;

    /* Both squares and their centers rotation (POLY) */
    for (i = 0; i <= 36 * 2; i++)
    {
        draw_poly(x, y, square, 4, yellow);
        draw_poly(x2, y2, square2, 4, blue);
        SDL_Flip(screen);
        SDL_Delay(25);
        draw_poly(x, y, square, 4, black);
        draw_poly(x2, y2, square2, 4, black);
        SDL_Flip(screen);
        rotate(10, square, 4);
        rotate(-10, square2, 4);
        rotate(5, radius, 1);
        rotate(-5, radius2, 1);
        x = center_x + radius[0][X];
        y = center_y + radius[0][Y];
        x2 = center_x + radius2[0][X];
        y2 = center_y + radius2[0][Y];
    }
    /* ---------- */

    /* TEXT */
    draw_text(font, "The things are gettin' harder...", text_color, TEXT_CENTER, TEXT_CENTER, &text_rect);
    SDL_Delay(2000);
    erase_text(&text_rect);
    /* TEXT */
    draw_text(font, "It's time to check some *untested* code", text_color, TEXT_CENTER, TEXT_CENTER, &text_rect);
    SDL_Delay(2000);
    erase_text(&text_rect);
    /* TEXT */
    draw_text(font, "Rotating square with bugged filling!", text_color, TEXT_CENTER, TEXT_CENTER, &text_rect);
    SDL_Delay(2000);
    fade_out(1000, 10);

    /* ---------- */

    /* TEXT and ROT */
    draw_text(font, "Try to fix it! ;-)", text_color, TEXT_CENTER, 390, &text_rect);

    /* Restoring centre of the screen coordinates */
    x = center_x;
    y = center_y;

    SDL_Rect clip_rect;

    /* Square rotation around its center (FILL) */
    for (i = 0; i <= 36 * 3; i++)
    {
        find_clip(x, y, &clip_rect, square, 4);
        extend_clip(&clip_rect, 5);
        draw_fill(x, y, square, 4, yellow);
        SDL_Flip(screen);
        SDL_Delay(25);
        SDL_FillRect(screen, &clip_rect, black);
        rotate(10, square, 4);
    }
    /* TEXT and ROT (erase) */
    SDL_FillRect(screen, NULL, black);
    /* draw_fill is buggy and leaves traces, so I clear the whole screen*/
    erase_text(&text_rect);
    SDL_Flip(screen);                   /* Clearing square and text */

    /* ---------- */

    /* Sinus "The End" */
    SDL_Surface *text_end;

    text_end = TTF_RenderText_Solid(font, "The End", text_color);

    text_rect.x = (screen->w - text_end->w) / 2;
    text_rect.y = (screen->h - text_end->h) / 2 - 20;
    text_rect.w = text_end->w;
    text_rect.h = text_end->h;
    for (i = 0; i <= 36 * 5; i++)
    {
        text_rect.y = center_y - sin(3.1415 / 180 * (i * 10)) * 40;
        SDL_BlitSurface(text_end, NULL, screen, &text_rect);
        SDL_Flip(screen);
        SDL_Delay(25);
        erase_text(&text_rect);
    }
    SDL_FreeSurface(text_end);

    /* ---------- */

    /* Closing and returning */
    TTF_CloseFont(font);
    quit = 1;
    return 0;
}
