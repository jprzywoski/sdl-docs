#include "main.h"

static unsigned int *color;
static unsigned char *scr1;
static unsigned char *scr2;

main(int argc, char *argv[])
{
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
    init_x(0, 0, W, H, 0, NAME);
#endif

    /* Init Colormap */

    init_colormap();

    /* Init Scr */

    scr1 = (unsigned char *)calloc(W * (H + 4), 1);
    scr2 = (unsigned char *)calloc(W * (H + 4), 1);

    /* Init FPS */

    init_fps();
    /* Boucle */
    while (event_x())
    {
        refresh();
        conv();
        update_x();
        next_fps();
    }
    /* Aff FPS */

    aff_fps(NAME);

    /* Free */

    free(color);
    free(scr1);
    free(scr2);

    /* Close X */

    close_x();

    /* End */

    exit(0);
}

void conv()
{
#ifdef X11

    int i;
    int *buffer_tmp = (int *)buffer;
    unsigned char *scr_tmp = scr1;

    for (i = 0; i < W * H; i++)
    {
        *(buffer_tmp++) = color[*(scr_tmp++)];
    }

#else

    int x, y;
    unsigned char *scr_tmp = scr1;

    if (SDL_LockSurface(screen) == 0)
    {
        switch (screen->format->BytesPerPixel)
        {
        case 1:
        {
            Uint8 *row;

            for (y = 0; y < H; ++y)
            {
                row = (Uint8 *)screen->pixels + y * screen->pitch;
                for (x = 0; x < W; ++x)
                {
                    *row++ = color[*(scr_tmp++)];
                }
            }
        }
        break;
        case 2:
        {
            Uint16 *row;

            for (y = 0; y < H; ++y)
            {
                row = (Uint16 *)((Uint8 *)screen->pixels + y * screen->pitch);
                for (x = 0; x < W; ++x)
                {
                    *row++ = color[*(scr_tmp++)];
                }
            }
        }
        break;
        case 4:
        {
            Uint32 *row;

            for (y = 0; y < H; ++y)
            {
                row = (Uint32 *)((Uint8 *)screen->pixels + y * screen->pitch);
                for (x = 0; x < W; ++x)
                {
                    *row++ = color[*(scr_tmp++)];
                }
            }
        }
        break;
        }
        SDL_UnlockSurface(screen);
    }
#endif
}

void refresh()
{
    int bcl, tmp;
    unsigned char *src;
    unsigned char *dst;
    unsigned char *swp;

    for (bcl = 0; bcl < 3 * W; bcl++)
    {
        *(scr2 + W * H + bcl) = 56 + rand() % 40;
    }
    tmp = 30 + rand() % 40;
    for (bcl = 0; bcl < tmp; bcl++)
    {
        dst = scr2 + W * (H + 1) + rand() % (W - 3);

        *dst = *(dst + 1) = *(dst + 2) = *(dst + W) = *(dst + W + 1) = *(dst + W + 2) = *(dst + 2 * W + 1) = *(dst + 2 *
                                         W + 2) = *(dst + 2 * W + 3) = 149;
    }
    src = scr2 + 2 * W;
    dst = scr1 + W;
    for (bcl = 0; bcl < W * (H + 2) - 2; bcl++)
    {
        tmp = *(src + W) + *(src + 2 * W - 1) + *(src + 2 * W) + *(src + 2 * W + 1);

        tmp >>= 2;
        if (tmp != 0)
        {
            *dst++ = tmp - 1;
        }
        else
        {
            *dst++ = 0;
        }
        src++;
    }
    swp = scr1;
    scr1 = scr2;
    scr2 = swp;
}

void init_colormap()
{
    int i;
    int r, g, b;

    color = (int *)calloc(256, 4);
    for (i = 0; i < 256; i++)
    {
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
#ifdef X11
        color[i] = (r << 16) + (g << 8) + (b);
#else
        color[i] = SDL_MapRGB(screen->format, r, g, b);
#endif
    }
}
