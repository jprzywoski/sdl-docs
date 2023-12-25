/***************************************************************************
 *                        blur.c  -  description
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

#include "blur.h"

#include "sdldemotools.h"
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

int points[3][32][3] =
{
    {
        /* cube */
        { -100, -100, 100 }, { -100, -150, 100 }, { -100, -100, 150 }, { -150, -100, 100 },     /* front upper left */
        { 100, -100, 100 }, { 100, -150, 100 }, { 150, -100, 100 }, { 100, -100, 150 },         /* front upper right */
        { 100, 100, 100 }, { 150, 100, 100 }, { 100, 150, 100 }, { 100, 100, 150 },             /* front lower right */
        { -100, 100, 100 }, { -100, 150, 100 }, { -100, 100, 150 }, { -150, 100, 100 },         /* front lower left */

        { -100, -100, -100 }, { -100, -150, -100 }, { -100, -100, -150 }, { -150, -100, -100 }, /* back upper left */
        { 100, -100, -100 }, { 100, -150, -100 }, { 150, -100, -100 }, { 100, -100, -150 },     /* back upper right */
        { 100, 100, -100 }, { 150, 100, -100 }, { 100, 150, -100 }, { 100, 100, -150 },         /* back lower right */
        { -100, 100, -100 }, { -100, 150, -100 }, { -100, 100, -150 }, { -150, 100, -100 } /* back lower left */
    }, {                                                                                        /* tetraeder */
        { -100, 100, 100 }, { -110, 100, 110 }, { -100, 110, 100 }, { -101, 101, 110 },         /* front lower left */
        { 100, 100, 100 }, { 100, 101, 100 }, { 100, 101, 100 }, { 110, 110, 100 },             /* front lower right */
        { 0, 100, -123 }, { 10, 100, -123 }, { 0, 110, -123 }, { 10, 100, -123 },               /* back lower center */
        { 0, -123, -11 }, { 0, -123, -1 }, { -10, -123, -11 }, { 0, -123, -21 },                /* top upper center */

        { -100, 100, 100 }, { -110, 100, 110 }, { -100, 110, 100 }, { -101, 101, 110 },         /* front lower left */
        { 100, 100, 100 }, { 100, 101, 100 }, { 100, 101, 100 }, { 110, 110, 100 },             /* front lower right */
        { 0, 100, -123 }, { 10, 100, -123 }, { 0, 110, -123 }, { 10, 100, -123 },               /* back lower center */
        { 0, -123, -11 }, { 0, -123, -1 }, { -10, -123, -11 }, { 0, -123, -21 } /* top upper center */
    }, {                                                                                        /* circle.. to be calculated */
        { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, {
            0, 0, 0
        }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, {
            0, 0, 0
        }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, {
            0, 0, 0
        }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }
    }
};
int lines[3][24][2] =
{
    {
        /* cube */
        { 0, 4 }, { 4, 8 }, { 8, 12 }, { 12, 0 },       /* front inner */
        { 3, 6 }, { 5, 10 }, { 9, 15 }, { 13, 1 },      /* front outer */

        { 16, 20 }, { 20, 24 }, { 24, 28 }, { 28, 16 }, /* back inner */
        { 19, 22 }, { 21, 26 }, { 25, 31 }, { 29, 17 }, /* back outer */

        { 0, 16 }, { 4, 20 }, { 8, 24 }, { 12, 28 },    /* depth inner */
        { 2, 18 }, { 7, 23 }, { 11, 27 }, { 14, 30 } /* depth outer */
    }, {                                                /* tetraeder */
        { 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }, { 4, 8 }, { 5, 9 }, { 6, 10 }, { 7, 11 }, { 8, 0 }, { 9, 1 }, { 10, 2 },
        { 11, 3 }, { 0, 12 }, { 1, 13 }, { 2, 14 }, { 3, 15 }, { 4, 12 }, { 5, 13 }, { 6, 14 }, { 7, 15 }, {
            8, 12
        }, {
            9, 13
        }, { 10, 14 }, { 11, 15 }
    }, {                                /* circle */
        { 0, 2 }, { 2, 4 }, { 4, 6 }, { 6, 8 }, { 8, 10 }, { 10, 12 }, { 12, 14 }, { 14, 0 }, { 16, 18 }, { 18, 20 },
        { 20, 22 }, { 22, 24 }, { 24, 26 }, { 26, 28 }, { 28, 30 }, { 30, 16 }, { 1, 15 }, { 3, 17 }, { 5, 19 }, {
            7, 21
        }, { 9, 23 }, { 11, 25 }, { 13, 27 }, { 15, 29 }
    }
};
float fpoints[32][3];

void blur_mix_callback(void *udata, Uint8 *stream, int len)
{
    blur_mix_stream = (Sint16 *)stream;
    blur_mix_len = len;
}

void blur_putText(SDL_Surface *screen, int x, int y, char *s, float n)
{
    SDL_Rect srect, drect;
    int i;
    float noise = n;

    if ((noise < 1.0) || (noise > 10.0))
    {
        noise = 1.0;
    }
    srect.w = 40;
    srect.h = 100;
    drect.w = 40;
    drect.h = 100;
    for (i = 0; i < strlen(s); i++)
    {
        if ((int)s[i] != 32)
        {
            if ((int)s[i] == 33)
            {
                srect.x = ((int)s[i] - 6) * 50;    /* ! */
            }
            if ((int)s[i] == 46)
            {
                srect.x = ((int)s[i] - 20) * 50;    /* . */
            }
            if ((int)s[i] == 63)
            {
                srect.x = ((int)s[i] - 35) * 50;    /* ? */
            }
            if ((int)s[i] == 95)
            {
                srect.x = ((int)s[i] - 66) * 50;    /* _ */
            }
            if (((int)s[i] >= 48) && ((int)s[i] <= 57))
            {
                srect.x = ((int)s[i] - 18) * 50;    /* 0-9 */
            }
            if (((int)s[i] >= 97) && ((int)s[i] <= 122))
            {
                srect.x = ((int)s[i] - 97) * 50;    /* a-z */
            }
            srect.x = 5 + srect.x + (noise * rand() / RAND_MAX);
            srect.y = noise * rand() / RAND_MAX;
            drect.x = x + i * 40;
            drect.y = y;
            SDL_BlitSurface(blur_font, &srect, screen, &drect);
        }
    }
}

void rotate(float rotx, float roty, float rotz)
{
    int i;
    float x, y, z;

    for (i = 0; i < 32; i++)
    {
        /* x-rot */
        y = +fpoints[i][1] * cos(rotx * 3.141 / 180) - fpoints[i][2] * sin(rotx * 3.141 / 180);
        z = +fpoints[i][1] * sin(rotx * 3.141 / 180) + fpoints[i][2] * cos(rotx * 3.141 / 180);
        fpoints[i][1] = y;
        fpoints[i][2] = z;

        /* y-rot */
        x = +fpoints[i][0] * cos(roty * 3.141 / 180) + fpoints[i][2] * sin(roty * 3.141 / 180);
        z = -fpoints[i][0] * sin(roty * 3.141 / 180) + fpoints[i][2] * cos(roty * 3.141 / 180);
        fpoints[i][0] = x;
        fpoints[i][2] = z;

        /* z-rot */
        x = +fpoints[i][0] * cos(rotz * 3.141 / 180) - fpoints[i][1] * sin(rotz * 3.141 / 180);
        y = +fpoints[i][0] * sin(rotz * 3.141 / 180) + fpoints[i][1] * cos(rotz * 3.141 / 180);
        fpoints[i][0] = x;
        fpoints[i][1] = y;
    }
}

void blur_run(SDL_Surface *screen)
{
    SDL_Event event;
    SDL_Color col;
    SDL_Rect rect_top;
    char *msgs[8] =
    {
        "", "", "", "", "", "dont eat animals", "stop murdering", "peace to all"
    };
    char *msg;
    int i, j, x1, y1, x2, y2;
    float rx, ry, rz, drx, dry, drz;
    int alpha, font_frames = 50;
    float deform;
    int figure;
    fft_type data1[256], data2[256];
    fft_type re, im, value;

    rect_top.x = 0;
    rect_top.y = 0;
    rect_top.w = 600;
    rect_top.h = 70;
    col.r = 16;
    col.g = 16;
    col.b = 16;
    rx = 0;
    ry = 0;
    rz = 0;
    drx = 1;
    dry = 2;
    drz = 4;
    alpha = 254;
    deform = 1.0;
    figure = 0;
    /* calc circle */
    for (i = 0; i <= 15; i++)
    {
        points[2][i][0] = 200 * sin((i * 360 / 16) * 3.141 / 180);
        points[2][i][1] = 0;
        points[2][i][2] = 100 * cos((i * 360 / 16) * 3.141 / 180);
    }
    for (i = 16; i <= 31; i++)
    {
        points[2][i][0] = 200 * sin(((i - 16) * 360 / 16) * 3.141 / 180);
        points[2][i][1] = 0;
        points[2][i][2] = 100 * cos(((i - 16) * 360 / 16) * 3.141 / 180);
    }
    sdldemo_tickfps();                  /* to avoid getfps being zero */

    /* main loop */
    SDL_PollEvent(&event);
    while ((event.type != SDL_QUIT) && (event.type != SDL_KEYDOWN))
    {
        if (!Mix_PlayingMusic())
        {
            Mix_PlayMusic(blur_music, 0);
        }
        /* SDL_SetAlpha(blur_buffer,SDL_SRCALPHA | SDL_RLEACCEL,alpha); */
        alpha = alpha - 20;
        if (alpha < 1)
        {
            alpha = 254;
        }
        SDL_BlitSurface(blur_back, NULL, blur_buffer, NULL);
        SDL_BlitSurface(blur_back, &rect_top, blur_top, NULL);

        msg = msgs[(sdldemo_fpsframes / font_frames) % (sizeof (msgs) / sizeof (char *))];
        blur_putText(blur_top, 320 - strlen(msg) * 20, 0, msg, 10 - (sdldemo_fpsframes % font_frames) / 5);
        for (i = 0; i < 256; i++)
        {
            data1[i] = blur_mix_stream[i * 2];
            data2[i] = blur_mix_stream[i * 2 + 1];
        }
        RealFFT(&data1[0]);
        RealFFT(&data2[0]);
        for (i = 0; i < 128; i++)
        {
            re = data1[BitReversed[i]];
            im = data1[BitReversed[i] + 1];
            value = sqrt(re * re + im * im) / 100;
            drawLine32(blur_buffer, 96 + i, 390 - value, 96 + i, 391, col);
            re = data2[BitReversed[i]];
            im = data2[BitReversed[i] + 1];
            value = sqrt(re * re + im * im) / 100;
            drawLine32(blur_buffer, 416 + i, 390 - value, 416 + i, 391, col);
        }
        for (i = 0; i < blur_mix_len / 2; i += 2)
        {
            setPixel32(blur_buffer, 122 + i / 10, 360 + blur_mix_stream[i] / 1024, col);
            setPixel32(blur_buffer, 442 + i / 10, 360 + blur_mix_stream[i + 1] / 1024, col);
        }
        for (i = 0; i < 32; i++)
            for (j = 0; j < 3; j++)
            {
                fpoints[i][j] = (float)points[figure][i][j] * sin((sdldemo_fpsframes + i) * 3.141 * deform / 180);
            }
        if ((sdldemo_fpsframes % ((int)sdldemo_getfps() * 4 + 1)) == 0)
        {
            drx = (8.0 * rand() / RAND_MAX) - 4.0;
            dry = (8.0 * rand() / RAND_MAX) - 4.0;
            drz = (8.0 * rand() / RAND_MAX) - 4.0;
            deform = (4.0 * rand() / RAND_MAX);
        }
        if ((sdldemo_fpsframes % ((int)sdldemo_getfps() * 8 + 1)) == 0)
        {
            figure++;
            if (figure > 2)
            {
                figure = 0;
            }
        }
        rx = rx + drx;
        ry = ry + dry;
        rz = rz + drz;

        rotate(rx, ry, rz);

        j = 1000;                       /* abstand */
        for (i = 0; i < 24; i++)
        {
            /*
             * fobjekt[i].x:=round( (abstand*robjekt[i].x)/(robjekt[i].z+1000+abstand) );
             * fobjekt[i].y:=round( (abstand*robjekt[i].y)/(robjekt[i].z+1000+abstand) );
             */
            x1 = (int)((j * fpoints[lines[figure][i][0]][0]) / (fpoints[lines[figure][i][0]][2] + 1000 + j));
            y1 = (int)((j * fpoints[lines[figure][i][0]][1]) / (fpoints[lines[figure][i][0]][2] + 1000 + j));
            x2 = (int)((j * fpoints[lines[figure][i][1]][0]) / (fpoints[lines[figure][i][0]][2] + 1000 + j));
            y2 = (int)((j * fpoints[lines[figure][i][1]][1]) / (fpoints[lines[figure][i][0]][2] + 1000 + j));

            drawLineAdd32(blur_buffer, 260 + x1, 200 + y1, 260 + x2, 200 + y2, col);
        }
        SDL_BlitSurface(blur_top, NULL, blur_buffer, &rect_top);

        SDL_BlitSurface(blur_buffer, NULL, screen, NULL);
        SDL_UpdateRect(screen, 0, 0, 0, 0);

        sdldemo_tickfps();

        SDL_PollEvent(&event);
    }
}
