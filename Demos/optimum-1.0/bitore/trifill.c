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

#include "trifill.h"
#include "main.h"

#ifdef X11

typedef u_int16_t Uint16;
typedef u_int32_t Uint32;

#endif

void trifill16(int x1, int y1, int x2, int y2, int x3, int y3, unsigned char r, unsigned char g, unsigned char b)
{
    Uint16 c;
    int swap;
    float pente1 = 0;
    float pente2 = 0;
    float pente3 = 0;
    float xx1, xx2;
    int yy;
    Uint16 *buff = (Uint16 *)buffer;

    /* */

    c = ((r >> 3) << 11) + ((g >> 2) << 5) + ((b >> 3));
    /* Tri suivant la hauteur */
    if (y1 > y2)
    {
        swap = y1;
        y1 = y2;
        y2 = swap;
        swap = x1;
        x1 = x2;
        x2 = swap;
    }
    if (y1 > y3)
    {
        swap = y1;
        y1 = y3;
        y3 = swap;
        swap = x1;
        x1 = x3;
        x3 = swap;
    }
    if (y2 > y3)
    {
        swap = y2;
        y2 = y3;
        y3 = swap;
        swap = x2;
        x2 = x3;
        x3 = swap;
    }
    /* init */
    if (y3 != y1)
    {
        pente1 = (float)(x3 - x1) / (y3 - y1);
    }
    if (y2 != y1)
    {
        pente2 = (float)(x2 - x1) / (y2 - y1);
    }
    if (y3 != y2)
    {
        pente3 = (float)(x3 - x2) / (y3 - y2);
    }

    /*
     *   1 *
     *     |\        Etape 1
     *     | \
     *     |--* 2
     *     | /
     *     |/        Etape 2
     *   3 *
     */

    /* Etape 1 */

    yy = y1;
    xx1 = x1;
    xx2 = x1;
    if (y1 != y2)
    {
        if (y1 & 1)
        {
            yy++;
            xx1 += pente1;
            xx2 += pente2;
        }
        while (yy < y2)
        {
            int i;
            int xxx1, xxx2;

            xxx1 = (int)xx1;
            xxx2 = (int)xx2;
            if (xxx1 > xxx2)
            {
                int xtmp;

                xtmp = xxx1;
                xxx1 = xxx2;
                xxx2 = xtmp;
            }
            for (i = xxx1; i <= xxx2; i++)
            {
                buff[i + W * yy] = c;
            }
            xx1 += 2 * pente1;
            xx2 += 2 * pente2;

            yy += 2;
        }
    }
    /* Etape 2 */

    yy = y2;
    xx2 = x2;
    if (y2 != y3)
    {
        if (y2 & 1)
        {
            yy++;
            xx2 += pente3;
        }
        while (yy < y3)
        {
            int i;
            int xxx1, xxx2;

            xxx1 = (int)xx1;
            xxx2 = (int)xx2;
            if (xxx1 > xxx2)
            {
                int xtmp;

                xtmp = xxx1;
                xxx1 = xxx2;
                xxx2 = xtmp;
            }
            for (i = xxx1; i <= xxx2; i++)
            {
                buff[i + W * yy] = c;
            }
            xx1 += 2 * pente1;
            xx2 += 2 * pente3;

            yy += 2;
        }
    }
}

void trifill32(int x1, int y1, int x2, int y2, int x3, int y3, unsigned char r, unsigned char g, unsigned char b)
{
    Uint32 c;
    int swap;
    float pente1 = 0;
    float pente2 = 0;
    float pente3 = 0;
    float xx1, xx2;
    int yy;
    Uint32 *buff = (Uint32 *)buffer;

    /* */

    c = (r << 16) + (g << 8) + (b);
    /* Tri suivant la hauteur */
    if (y1 > y2)
    {
        swap = y1;
        y1 = y2;
        y2 = swap;
        swap = x1;
        x1 = x2;
        x2 = swap;
    }
    if (y1 > y3)
    {
        swap = y1;
        y1 = y3;
        y3 = swap;
        swap = x1;
        x1 = x3;
        x3 = swap;
    }
    if (y2 > y3)
    {
        swap = y2;
        y2 = y3;
        y3 = swap;
        swap = x2;
        x2 = x3;
        x3 = swap;
    }
    /* init */
    if (y3 != y1)
    {
        pente1 = (float)(x3 - x1) / (y3 - y1);
    }
    if (y2 != y1)
    {
        pente2 = (float)(x2 - x1) / (y2 - y1);
    }
    if (y3 != y2)
    {
        pente3 = (float)(x3 - x2) / (y3 - y2);
    }

    /*
     *   1 *
     *     |\        Etape 1
     *     | \
     *     |--* 2
     *     | /
     *     |/        Etape 2
     *   3 *
     */

    /* Etape 1 */

    yy = y1;
    xx1 = x1;
    xx2 = x1;
    if (y1 != y2)
    {
        if (y1 & 1)
        {
            yy++;
            xx1 += pente1;
            xx2 += pente2;
        }
        while (yy < y2)
        {
            int i;
            int xxx1, xxx2;

            xxx1 = (int)xx1;
            xxx2 = (int)xx2;
            if (xxx1 > xxx2)
            {
                int xtmp;

                xtmp = xxx1;
                xxx1 = xxx2;
                xxx2 = xtmp;
            }
            for (i = xxx1; i <= xxx2; i++)
            {
                buff[i + W * yy] = c;
            }
            xx1 += 2 * pente1;
            xx2 += 2 * pente2;

            yy += 2;
        }
    }
    /* Etape 2 */

    yy = y2;
    xx2 = x2;
    if (y2 != y3)
    {
        if (y2 & 1)
        {
            yy++;
            xx2 += pente3;
        }
        while (yy < y3)
        {
            int i;
            int xxx1, xxx2;

            xxx1 = (int)xx1;
            xxx2 = (int)xx2;
            if (xxx1 > xxx2)
            {
                int xtmp;

                xtmp = xxx1;
                xxx1 = xxx2;
                xxx2 = xtmp;
            }
            for (i = xxx1; i <= xxx2; i++)
            {
                buff[i + W * yy] = c;
            }
            xx1 += 2 * pente1;
            xx2 += 2 * pente3;

            yy += 2;
        }
    }
}
