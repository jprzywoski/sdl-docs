/* GL_blit: an exhaustive example about blitting sprites with OpenGL
 * Copyright (C) 2005 Angelo "Encelo" Theodorou
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
 *
 * NOTE:
 *
 * This program is part of "Mars, Land of No Mercy" SDL examples,
 * you can find others examples on http://mars.sourceforge.net
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <GL/gl.h>

#include "gl_blit.h"

int main(int argc, char **argv)
{
    SDL_Surface *screen;
    SDL_Event event;
    int quit = 0;
    const SDL_VideoInfo *info = NULL;
    int width = 640;
    int height = 480;
    int bpp = 0;
    int flags = 0;
    Uint32 interval;
    GLfloat angle;
    char *imagefile = "mars.png";
    struct Sprite *glsprite;

    /* ----- SDL init --------------- */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
        exit(-1);
    }
    atexit(SDL_Quit);

    info = SDL_GetVideoInfo();
    bpp = info->vfmt->BitsPerPixel;

    /* ----- OpenGL attribute setting via SDL --------------- */
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    /*	flags = SDL_OPENGL | SDL_FULLSCREEN; */
    flags = SDL_OPENGL;
    /* ----- Setting up the screen surface --------------- */
    if ((screen = SDL_SetVideoMode(width, height, bpp, flags)) == 0)
    {
        fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
        exit(-1);
    }
    SDL_WM_SetCaption("GL_blit by Encelo", NULL);

    /* ----- OpenGL init --------------- */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glDisable(GL_DEPTH);                /* This is a 2d program, no need for depth test */

    /* ----- Sprite init --------------- */
    SDL_Color colorkey = { 0xFF, 0x00, 0xFF, 0 };

    if ((glsprite = AllocSprite(imagefile, &colorkey)) == 0)
    {
        fprintf(stderr, "Error loading the image file!\n");
        exit(-1);
    }
    glEnable(GL_TEXTURE_2D);
    glsprite->y = height / 2;
    /* ----- Event cycle --------------- */
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quit = 1;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                case SDLK_q:
                    quit = 1;
                    break;
                case SDLK_F1:
                    SDL_WM_ToggleFullScreen(screen);
                    break;
                }
            }
        }
        interval = FrameTiming();

        /* ----- Blitting on the screen --------------- */
        glClear(GL_COLOR_BUFFER_BIT);

        /* --- Rotating background triangles ---*/
        angle += 0.125 * interval;
        glMatrixMode(GL_MODELVIEW);
        glDisable(GL_TEXTURE_2D);

        int i, j, ntri = 10;

        for (i = 1; i <= ntri; i++)
        {
            for (j = 1; j <= ntri; j++)
            {
                glLoadIdentity();
                glTranslatef(j * (width / ntri) - (width / ntri) / 2, i * (height / ntri) - (height / ntri) / 2, 0);
                glRotatef(angle + 10 * j + 10 * i, 0.0f, 0.0f, 1.0f);

                glBegin(GL_TRIANGLES);
                glColor3f(1.0f, 0.0f, 0.0f);
                glVertex2i(0, -(height / ntri) / 2);
                glColor3f(0.0f, 1.0f, 0.0f);
                glVertex2i(-(width / ntri) / 2, (height / ntri) / 2);
                glColor3f(0.0f, 0.0f, 1.0f);
                glVertex2i((width / ntri) / 2, (height / ntri) / 2);
                glEnd();
            }
        }
        glEnable(GL_TEXTURE_2D);
        glLoadIdentity();

        /* --- Moving the sprite ---*/
        static int mv_flag = 0;

        if (glsprite->x < (width - glsprite->w) && mv_flag == 0)
        {
            glsprite->x += 0.1 * interval;
        }
        else
        {
            mv_flag = 1;
        }
        if (glsprite->x > 0 && mv_flag == 1)
        {
            glsprite->x -= 0.1 * interval;
        }
        else
        {
            mv_flag = 0;
        }

        /* --- Scaling the sprite ---*/
        static int sz_flag = 0;

        if (glsprite->xsize < 2 && sz_flag == 0)
        {
            glsprite->xsize += 0.001 * interval;
            glsprite->ysize += 0.001 * interval;
        }
        else
        {
            sz_flag = 1;
        }
        if (glsprite->xsize > 1 && sz_flag == 1)
        {
            glsprite->xsize -= 0.001 * interval;
            glsprite->ysize -= 0.001 * interval;
        }
        else
        {
            sz_flag = 0;
        }
        /* Rotating and sinus moving the sprite*/
        glsprite->y = height / 2 + sinf(M_PI / 180 * (glsprite->angle * 3)) * 30;
        glsprite->angle += 0.1 * interval;
        BlitSprite(glsprite);

        glFlush();
        SDL_GL_SwapBuffers();

        /*		SDL_Delay(20); */  /* Decomment this if you want 1/50th screen update */
    }
    /* ----- Quitting --------------- */
    FreeSprite(glsprite);
    SDL_Quit();
    return 0;
}

/* Allocate and fill the sprite structure. Create an OpenGL texture */
struct Sprite *AllocSprite(char *filename, SDL_Color *colorkey)
{
    SDL_Surface *temp, *image;
    GLuint texnum;
    struct Sprite *newsprite;
    Uint32 key;
    Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    newsprite = malloc(sizeof (struct Sprite));

    newsprite->x = newsprite->y = 0;
    newsprite->angle = 0.0f;
    newsprite->xsize = newsprite->ysize = 1;
    if ((temp = (SDL_Surface *)IMG_Load(filename)))
    {
        printf("image width = %d\n", temp->w);
        printf("image height = %d\n", temp->h);

        int max_size;

        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);
        if ((temp->w > max_size) || (temp->h > max_size))
        {
            fprintf(stderr, "Image size exceeds max texture size, which is %d pixels for each side\n", max_size);
            SDL_FreeSurface(temp);
            exit(-1);
        }
        newsprite->w = temp->w;
        newsprite->h = temp->h;
        /* Check for NPOT textures */
        if (IsNPOT(temp->w, temp->h))
        {
            printf("NPOT texture, using special code for automatic padding\n");

            GLint wreal = 0, hreal = 0;
            GLint xpad = 0, ypad = 0;

            wreal = (int)powf(2.0, ceilf(logf((float)temp->w) / logf(2.0f)));
            hreal = (int)powf(2.0, ceilf(logf((float)temp->h) / logf(2.0f)));
            xpad = (wreal - temp->w) / 2;
            ypad = (hreal - temp->h) / 2;

            printf("tex width = %d\n", wreal);
            printf("tex height = %d\n", hreal);
            if (colorkey != NULL)
            {
                image = SDL_CreateRGBSurface(SDL_SWSURFACE, wreal, hreal, 32, rmask, gmask, bmask, amask);
            }
            else
            {
                image = SDL_CreateRGBSurface(SDL_SWSURFACE, wreal, hreal, 24, rmask, gmask, bmask, 0);
            }

            SDL_Rect dst = { xpad, ypad, temp->w, temp->h };

            if (colorkey != NULL)
            {
                key = SDL_MapRGB(temp->format, colorkey->r, colorkey->g, colorkey->b);
                SDL_FillRect(image, NULL, key); /* Without GL_BLEND transparent areas will be like colorkey and not black */
                SDL_SetColorKey(temp, SDL_SRCCOLORKEY, key);
            }
            SDL_BlitSurface(temp, 0, image, &dst);
            SDL_FreeSurface(temp);
        }
        /* POT textures */
        else if (colorkey != NULL)
        {
            key = SDL_MapRGB(temp->format, colorkey->r, colorkey->g, colorkey->b);
            image = SDL_CreateRGBSurface(SDL_SWSURFACE, temp->w, temp->h, 32, rmask, gmask, bmask, amask);
            SDL_FillRect(image, NULL, key);     /* Disabling GL_BLEND transparent areas will be like colorkey and not black */
            SDL_SetColorKey(temp, SDL_SRCCOLORKEY, key);
            SDL_BlitSurface(temp, 0, image, 0);
            SDL_FreeSurface(temp);
        }
        else
        {
            image = temp;
        }
        /* OpenGL Texture creation */
        glGenTextures(1, &texnum);
        glBindTexture(GL_TEXTURE_2D, texnum);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        if (colorkey != NULL)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->w, image->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
        }
        newsprite->texnum = texnum;

        SDL_FreeSurface(image);
        return newsprite;
    }
    else
    {
        return 0;
    }
}

int FreeSprite(struct Sprite *sprite)
{
    GLuint texnum;

    if (sprite != 0)
    {
        texnum = sprite->texnum;
        if (glIsTexture(texnum))
        {
            glDeleteTextures(1, &texnum);
        }
        free(sprite);
        return 0;
    }
    else
    {
        return -1;
    }
}

/* Blit the sprite referring to the upper left corner but keeping the correct image center for further transformations */
void BlitSprite(struct Sprite *sprite)
{
    GLint wreal, hreal;
    GLint xpad = 0, ypad = 0;

    if (IsNPOT(sprite->w, sprite->h))
    {
        wreal = (int)powf(2.0, ceilf(logf((float)sprite->w) / logf(2.0f)));
        hreal = (int)powf(2.0, ceilf(logf((float)sprite->h) / logf(2.0f)));
        xpad = (wreal - sprite->w) / 2;
        ypad = (hreal - sprite->h) / 2;
    }
    else
    {
        wreal = sprite->w;
        hreal = sprite->h;
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 2 * wreal, 0, 2 * hreal, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glTranslatef(sprite->x + sprite->w / 2, sprite->y + sprite->h / 2, 0);
    glRotatef(sprite->angle, 0, 0, 1);
    glScalef(sprite->xsize, sprite->ysize, 0);

    glBindTexture(GL_TEXTURE_2D, sprite->texnum);
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTexCoord2d(xpad, ypad);
    glVertex2d(-sprite->w / 2, -sprite->h / 2);
    glTexCoord2d(xpad + sprite->w, ypad);
    glVertex2d(sprite->w / 2, -sprite->h / 2);
    glTexCoord2d(xpad + sprite->w, ypad + sprite->h);
    glVertex2d(sprite->w / 2, sprite->h / 2);
    glTexCoord2d(xpad, ypad + sprite->h);
    glVertex2d(-sprite->w / 2, sprite->h / 2);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glDisable(GL_BLEND);
}

/* Check for non-power-of-two textures */
int IsNPOT(int width, int height)
{
    if ((int)powf(2.0f, ceilf(logf((float)width) / logf(2.0f))) != width)
    {
        return 1;
    }
    if ((int)powf(2.0f, ceilf(logf((float)height) / logf(2.0f))) != height)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/* Calculate frame interval and print FPS each 5s */
int FrameTiming(void)
{
    Uint32 interval;
    static Uint32 current, last = 0, five = 0, nframes = 0;

    current = SDL_GetTicks();
    nframes++;
    if (current - five > 5 * 1000)
    {
        printf("%u frames in 5 seconds = %.1f FPS\n", nframes, (float)nframes / 5.0f);
        nframes = 0;
        five = current;
    }
    interval = current - last;
    last = current;

    return interval;
}
