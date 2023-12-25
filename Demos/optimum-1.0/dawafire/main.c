/*
 *  Copyright (C) 1999 Optimum
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "main.h"

unsigned char *buffy256;
unsigned int pal[256];
struct particle
{
    int X0, Y0;
    int V0x, V0y;
    int power;
    int time;
    struct particle *next;
};

typedef struct particle particle;

particle *firstparticle;

int main(int argc, char **argv)
{
    /* Options */

    int val, i;
    static struct option long_options[] =
    {
        { "help", no_argument, NULL, 1 }, { "nofps", no_argument, NULL, 2 }, { 0, 0, 0, 0 }
    };

    optfps = 1;
    while ((val = getopt_long(argc, argv, "", long_options, NULL)) != -1)
    {
        switch (val)
        {
        case 1:
            printf("USAGE:\n");
            printf("  ./%s [options]\n", NAME);
            printf("\n");
            printf("OPTIONS:\n");
            printf("  --help            : print help\n");
            printf("  --nofps           : no fps window\n");
            printf("\n");
            printf("BUG REPORT:\n");
            printf("  email: optimum@citeweb.net\n");
            printf("    url: http://optimum.citeweb.net\n");
            printf("\n");
            exit(0);
            break;
        case 2:
            optfps = 0;
            break;
        case '?':
            printf("Try `./%s --help' for more options.\n", NAME);
            exit(0);
            break;
        }
    }
    /* Init X */

#ifdef X11
    if (init_x(0, 0, W, H, 32, NAME) != 32)
    {
        fprintf(stderr, "Erreur :\n");
        fprintf(stderr, "  Vos couleurs sont codees sur %d bits\n", depth);
        fprintf(stderr, "  La demo code les couleurs sur 32 bits\n");
        close_x();
        exit(1);
    }
#else
    init_x(0, 0, W, H, 8, NAME);
#endif
    /* 8bpp buffer */
    for (i = 0; i < 256; i++)
    {
        int r, g, b;

        r = g = b = 0;
        if ((i > 7) && (i < 32))
        {
            r = 10 * (i - 7);
        }
        if (i > 31)
        {
            r = 255;
        }
        if ((i > 32) && (i < 57))
        {
            g = 10 * (i - 32);
        }
        if (i > 56)
        {
            g = 255;
        }
        if (i < 8)
        {
            b = 8 * i;
        }
        if ((i > 7) && (i < 17))
        {
            b = 8 * (16 - i);
        }
        if ((i > 57) && (i < 82))
        {
            b = 10 * (i - 57);
        }
        if (i > 81)
        {
            b = 255;
        }
        pal[i] = (r << 16) + (g << 8) + (b);
    }
#ifdef X11
    buffy256 = (unsigned char *)calloc(W * H, 1);
#else
    buffy256 = buffer;

    {
        SDL_Color colors[256];

        for (i = 0; i < 256; i++)
        {
            colors[i].r = (pal[i] >> 16) & 0xFF;
            colors[i].g = (pal[i] >> 8) & 0xFF;
            colors[i].b = (pal[i]) & 0xFF;
        }
        SDL_SetColors(screen, colors, 0, 256);
    }
#endif

    /* Init Particle */

    firstparticle = NULL;

    /* Init FPS */

    init_fps();
    /* Boucle */
    while (event_x())
    {
        refresh();
        next_fps();
        update_x();
    }
    /* Aff FPS */

    aff_fps(NAME);

    /* Close X */

    close_x();

    /* End */

    exit(0);
}

void refresh()
{
    /* B G R X */

    static double k = 0;
    int i;
    unsigned char *buffy256_tmp = buffy256;
    unsigned char *blur = buffy256;
    particle *precurrentparticle;
    particle *currentparticle;

    /* Add new particles */
    for (i = 0; i < 8; i++)
    {
        int ii;

        for (ii = 0; ii < 8; ii++)
        {
            precurrentparticle = (particle *)malloc(sizeof (particle));

            precurrentparticle->V0x = (rand() & 31) - 16;
            precurrentparticle->V0y = (rand() & 31);

            precurrentparticle->X0 = W / 2 + W / 2.5 * sin((20 * sin(k / 20)) + i * 70) * (sin(10 + k / (10 + i)) +
                                     0.2) * cos((k + i * 25) / 10);
            precurrentparticle->Y0 = H / 2 + H / 2.5 * cos((20 * sin(k / (20 + i))) + i * 70) * (sin(10 + k / 10) +
                                     0.2) * cos((k + i * 25) / 10);

            precurrentparticle->power = 110 + 50 * sin(k / 3);
            precurrentparticle->time = 0;
            precurrentparticle->next = firstparticle;

            firstparticle = precurrentparticle;
        }
        k += 0.02;
    }
    /*
     *  Physics:
     *
     *  Vx(t) = V0x
     *  Vy(t) = V0y - G.t
     *
     *  X(t) = X0 + V0x.t
     *  Y(t) = Y0 + V0y.t - G.t^2
     *
     */

    currentparticle = firstparticle->next;
    while (currentparticle != NULL)
    {
        int dawa;
        int tx, ty;
        int time;

        time = currentparticle->time / 16;

        tx = (currentparticle->X0) + time * (currentparticle->V0x);
        ty = (currentparticle->Y0) + time * (currentparticle->V0y) - time * time * 2;

        ty = H - ty;
        if ((tx < W) && (ty > 3) && (ty < H))
        {
            dawa = buffy256[ty * W + tx];
            dawa += currentparticle->power;
            if (dawa > 255)
            {
                dawa = 255;
            }
            buffy256[ty * W + tx] = dawa;
        }
        currentparticle->time += 1;

        currentparticle->power -= ((currentparticle->power >> 4) + 1);
        if (currentparticle->power <= 1)
        {
            precurrentparticle->next = currentparticle->next;
            free(currentparticle);
        }
        else
        {
            precurrentparticle = currentparticle;
        }
        currentparticle = precurrentparticle->next;
    }
    /*
     * Blur :
     *
     *  &&&
     *  &@&
     *   &
     */

    blur += 2 * W;
    for (i = 0; i < W * (H - 3); i++)
        *(blur++) = (*(blur - W - 1) + *(blur - W) + *(blur - W + 1) + *(blur - 1) + *(blur) + ((*blur) >> 1) +
                     ((*blur) >> 2) + *(blur + 1) + *(blur + W)) >> 3;
#ifdef X11
    /* 24bpp conv */
    {
        int *buffy_tmp = (int *)buffer;

        for (i = 2 * W; i < W * (H - 1); i++)
        {
            *(buffy_tmp++) = pal[(int)*(buffy256_tmp++)];
        }
    }
#endif
}
