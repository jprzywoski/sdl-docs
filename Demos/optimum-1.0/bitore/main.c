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
#include "3d.h"
#include "trifill.h"

#define N_R1	14
#define N_R2	12

#define R1	50
#define R2	23

point3d tore_point3d[2 * N_R1 * N_R2];
point3d rotated_point3d[2 * N_R1 * N_R2];
triangle tore_triangle[2 * 2 * N_R1 * N_R2];
Ztriangle tore_Ztriangle[2 * 2 * N_R1 * N_R2];

void rotate3d(float *xr, float *yr, float *zr, float ax, float ay, float az)
{
    float xr2, yr2, zr2;

    xr2 = (*xr * cos(az) + *yr * sin(az));
    yr2 = (*xr * sin(az) - *yr * cos(az));
    *xr = xr2;
    *yr = yr2;

    xr2 = (*xr * cos(ay) + *zr * sin(ay));
    zr2 = (*xr * sin(ay) - *zr * cos(ay));
    *xr = xr2;
    *zr = zr2;

    zr2 = (*zr * cos(ax) + *yr * sin(ax));
    yr2 = (*zr * sin(ax) - *yr * cos(ax));
    *zr = zr2;
    *yr = yr2;
}

void init_3d()
{
    int i, j, k;
    float a, b;
    float x, y, z;

    for (k = 0; k < 2; k++)
        for (j = 0; j < N_R1; j++)
        {
            a = j * 2 * M_PI / N_R1;
            for (i = 0; i < N_R2; i++)
            {
                b = i * 2 * M_PI / N_R2;

                y = R2 * cos(b);        /* create a ring at origin */
                z = R2 * sin(b);
                x = 0;

                y += R1;                                        /* put it at good position */

                rotate3d(&x, &y, &z, 0, k * M_PI / 2, a);       /* rotate/extrude it */

                tore_point3d[k * N_R1 * N_R2 + (i + N_R2 * j)].x = x;
                tore_point3d[k * N_R1 * N_R2 + (i + N_R2 * j)].y = y + (-23 + k * 46);
                tore_point3d[k * N_R1 * N_R2 + (i + N_R2 * j)].z = z;
            }
        }
    for (k = 0; k < 2; k++)
        for (j = 0; j < N_R1; j++)
            for (i = 0; i < N_R2; i++)
            {
                tore_triangle[2 * (k * N_R1 * N_R2 + i + N_R2 * j)].a = &rotated_point3d[k * N_R1 * N_R2 + i + N_R2 *
                        j];
                tore_triangle[2 * (k * N_R1 * N_R2 + i + N_R2 * j)].b = &rotated_point3d[k * N_R1 * N_R2 + (i + 1) %
                        N_R2 + N_R2 * j];
                tore_triangle[2 * (k * N_R1 * N_R2 + i + N_R2 * j)].c = &rotated_point3d[k * N_R1 * N_R2 + (i + 1) %
                        N_R2 + N_R2 * ((j + 1) % (N_R1))];

                tore_triangle[2 * (k * N_R1 * N_R2 + i + N_R2 * j) + 1].a = &rotated_point3d[k * N_R1 * N_R2 + i +
                        N_R2 * j];
                tore_triangle[2 * (k * N_R1 * N_R2 + i + N_R2 * j) + 1].b = &rotated_point3d[k * N_R1 * N_R2 + (i + 1) %
                        N_R2 + N_R2 * ((j + 1) % (N_R1))];
                tore_triangle[2 * (k * N_R1 * N_R2 + i + N_R2 * j) + 1].c = &rotated_point3d[k * N_R1 * N_R2 + i +
                        N_R2 * ((j + 1) % (N_R1))];
            }
}

int main(int argc, char **argv)
{
#ifdef X11

    extern char *dga_addr;
    extern int dga_width;               /* Pixel */
    extern int dga_height;              /* Pixel */
#endif

    FILE *fichier;
    unsigned char data[W * (H / 2) * 4];
    int i, factor;

    /* Options */
    int val;
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
            printf("  ./%s [options]\n", argv[0]);
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
            printf("Try `./%s --help' for more options.\n", argv[0]);
            exit(0);
            break;
        }
    }
    /* Init 3D */

    init_3d();

    /* Init X */

    init_x(0, 0, W, H, 0, "demo");
    factor = (depth + 7) / 8;
    if ((factor != 2) && (factor != 4))
    {
        printf("Error: Not 16 or 32 bpp (depth = %d)\n", depth);
        close_x();
        exit(1);
    }
    /* Background */

    fichier = fopen("image.raw", "r");

#ifdef X11
    if (depth == 16)
        for (i = 0; i < W * H / 2; i++)
        {
            unsigned short c;
            c = fgetc(fichier);
            c <<= 8;
            c |= fgetc(fichier);
            memcpy(data + 2 * i, &c, 2);
        }
    else
        for (i = 0; i < W * H / 2 i++)
        {
            unsigned short c;
            c = fgetc(fichier);
            c <<= 8;
            c |= fgetc(fichier);
            data[4 * i] = (c >> 11) << 3;
            data[4 * i + 1] = ((c >> 5) & 0x3F) << 2;
            data[4 * i + 2] = (c & 0x1F) << 3;
        }
    for (i = 1; i < H; i += 2)
        memcpy(dga_addr + ((dga_width - 320) / 2 + ((dga_height - 200) / 2 + i) * dga_width) * factor, data + factor *
               (i / 2) * 320, factor * 320);
#else
    switch (factor)
    {
        unsigned char rgb[3];

    case 2:
    {
        Uint16 *pixel = (Uint16 *)data;

        for (i = 0; i < W * H / 2; i++)
        {
            unsigned short c;

            c = fgetc(fichier);
            c <<= 8;
            c |= fgetc(fichier);
            rgb[0] = (c >> 11) << 3;
            rgb[1] = ((c >> 5) & 0x3F) << 2;
            rgb[2] = (c & 0x1F) << 3;
            *pixel++ = SDL_MapRGB(screen->format, rgb[0], rgb[1], rgb[2]);
        }
        trifill = &trifill16;
    }
    break;
    case 4:
    {
        Uint32 *pixel = (Uint32 *)data;

        for (i = 0; i < W * H / 2; i++)
        {
            unsigned short c;

            c = fgetc(fichier);
            c <<= 8;
            c |= fgetc(fichier);
            rgb[0] = (c >> 11) << 3;
            rgb[1] = ((c >> 5) & 0x3F) << 2;
            rgb[2] = (c & 0x1F) << 3;
            *pixel++ = SDL_MapRGB(screen->format, rgb[0], rgb[1], rgb[2]);
        }
        trifill = &trifill32;
    }
    break;
    }
    if (SDL_LockSurface(screen) == 0)
    {
        for (i = 1; i < H; i += 2)
        {
            memcpy((Uint8 *)screen->pixels + i * screen->pitch, data + factor * (i / 2) * W, factor * W);
        }
        SDL_UnlockSurface(screen);
    }
#endif /* X11 */

    fclose(fichier);

    /* Trifill Selection */

#ifdef X11
    if (depth == 16)
    {
        trifill = &trifill16;
    }
    else
    {
        trifill = &trifill32;
    }
#endif

    /* Init FPS */

    init_fps();

    /* Boucle */

#ifdef X11
    if (depth == 16)
        while (event_x())
        {
            memset(buffer, 0, 2 * W * H);

            refresh();
            next_fps();
            for (i = 0; i < H; i += 2)
                memcpy(dga_addr + ((dga_width - W) / 2 + ((dga_height - H) / 2 + i) * dga_width) * 2, buffer + 2 * i *
                       W, 2 * W);
        }
    else
        while (event_x())
        {
            memset(buffer, 0, 4 * W * H);

            refresh();
            next_fps();
            for (i = 0; i < H; i += 2)
                memcpy(dga_addr + ((dga_width - W) / 2 + ((dga_height - H) / 2 + i) * dga_width) * 4, buffer + 4 * i *
                       W, 4 * W);
        }
#else
    switch (factor)
    {
    case 2:
    {
        register int pitch = screen->pitch;

        while (event_x())
        {
            memset(buffer, 0, 2 * W * H);

            refresh();
            next_fps();
            if (SDL_LockSurface(screen) == 0)
            {
                for (i = 0; i < H; i += 2)
                {
                    memcpy((Uint8 *)screen->pixels + i * pitch, buffer + 2 * i * W, 2 * W);
                }
                SDL_UnlockSurface(screen);
            }
            SDL_Flip(screen);
        }
    }
    break;

    case 4:
    {
        register int pitch = screen->pitch;

        while (event_x())
        {
            memset(buffer, 0, 4 * W * H);

            refresh();
            next_fps();
            if (SDL_LockSurface(screen) == 0)
            {
                for (i = 0; i < H; i += 2)
                {
                    memcpy((Uint8 *)screen->pixels + i * pitch, buffer + 4 * i * W, 4 * W);
                }
                SDL_UnlockSurface(screen);
            }
            SDL_Flip(screen);
        }
    }
    break;
    }
#endif /* X11 */

    /* Aff FPS */

    aff_fps(argv[0]);

    /* Close X */

    close_x();

    /* End */

    exit(0);
}

void refresh()
{
    static float ax = 0, ay = 0, az = 0;
    int i, j;

    for (i = 0; i < 2 * N_R1 * N_R2; i++)
    {
        float xx, yy, zz;

        xx = tore_point3d[i].x;
        yy = tore_point3d[i].y;
        zz = tore_point3d[i].z;

        rotate3d(&xx, &yy, &zz, ax, ay, az);

        rotated_point3d[i].x = xx;
        rotated_point3d[i].y = yy;
        rotated_point3d[i].z = zz;
    }
    /* tri */
    for (i = 0; i < 2 * 2 * N_R1 * N_R2; i++)
    {
        tore_Ztriangle[i].triangle = &tore_triangle[i];

        tore_Ztriangle[i].z = ((*tore_triangle[i].a).z);
    }
    for (i = 0; i < 2 * 2 * N_R1 * N_R2 - 1; i++)
        for (j = i; j < 2 * 2 * N_R1 * N_R2; j++)
            if (tore_Ztriangle[i].z > tore_Ztriangle[j].z)
            {
                float tmpz;
                triangle *tmptriangle;

                tmpz = tore_Ztriangle[i].z;
                tmptriangle = tore_Ztriangle[i].triangle;

                tore_Ztriangle[i].z = tore_Ztriangle[j].z;
                tore_Ztriangle[i].triangle = tore_Ztriangle[j].triangle;

                tore_Ztriangle[j].z = tmpz;
                tore_Ztriangle[j].triangle = tmptriangle;
            }
    /* */
    for (i = 0; i < 2 * 2 * N_R1 * N_R2; i++)
    {
        float tx1, ty1, tz1;
        float tx2, ty2, tz2;
        float nx, ny, nz;
        triangle *facette = tore_Ztriangle[i].triangle;

        tx1 = (*(*facette).a).x - (*(*facette).c).x;
        ty1 = (*(*facette).a).y - (*(*facette).c).y;
        tz1 = (*(*facette).a).z - (*(*facette).c).z;

        tx2 = (*(*facette).b).x - (*(*facette).c).x;
        ty2 = (*(*facette).b).y - (*(*facette).c).y;
        tz2 = (*(*facette).b).z - (*(*facette).c).z;

        nx = ty1 * tz2 - tz1 * ty2;
        ny = tz1 * tx2 - tx1 * tz2;
        nz = tx1 * ty2 - ty1 * tx2;
        /* */
        if (nz > 0)
        {
            (*trifill)(W / 2 + (int)(*(*facette).a).x, H / 2 + (int)(*(*facette).a).y, W / 2 + (int)(*(*facette).b).x,
                       H / 2 + (int)(*(*facette).b).y, W / 2 + (int)(*(*facette).c).x, H / 2 + (int)(*(*facette).c).y, 128 +
                       (((int)nx) / 4), 128 + (((int)ny) / 4), 128 + (((int)nz) / 4));
        }
    }
    ax += 0.06;
    ay += 0.04;
    az += 0.08;
}
