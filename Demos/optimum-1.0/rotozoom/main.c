#include "main.h"

#ifndef M_PI
#define M_PI	3.14159265358979323846  /* pi */
#endif

#define PAL	"texture.pal"
#define DATA	"texture.data"

static unsigned char texture[4 * 128 * 128];
int teta = 0;
int h_cos[360];
int h_sin[360];

main(int argc, char *argv[])
{
    /* Init VGA */
    if (init_x(0, 0, W, H, 8, NAME) != 8)
    {
        fprintf(stderr, "Erreur :\n");
        fprintf(stderr, "  Vos couleurs sont codees sur %d bits\n", depth);
        fprintf(stderr, "  La demo code les couleurs sur 8 bits\n");
        close_x();
        exit(1);
    }
    /* Init Colormap */

    init_colormap();

    /* Init Tables */

    init_tables();

    /* Init Texture */

    init_texture();

    /* Init FPS */

    init_fps();
    /* Boucle */
    while (event_x())
    {
        refresh();
        update_x();
        next_fps();

        teta = (teta + 2) % 360;
    }
    /* Aff FPS */

    aff_fps(NAME);

    /* Close VGA */

    close_x();

    /* Fin */

    exit(0);
}

void refresh()
{
    const int c = h_cos[teta];
    const int s = h_sin[teta];
    const int xi = -(W / 2) * c;
    const int yi = (W / 2) * s;
    const int xj = -(H / 2) * s;
    const int yj = -(H / 2) * c;
    unsigned char *scrtmp = buffer;
    int i, j;
    int x, y;
    int xprime = xj;
    int yprime = yj;

    for (j = 0; j < H; j++)
    {
        x = xprime + xi;
        xprime += s;

        y = yprime + yi;
        yprime += c;
        for (i = 0; i < W; i++)
        {
            x += c;
            y -= s;

            *(scrtmp++) = *(texture + ((x >> 8) & 255) + (y & (255 << 8)));
        }
    }
}

void init_colormap()
{
    int i;
    SDL_Color colors[256];
    FILE *fichier;

    fichier = fopen(PAL, "r");
    for (i = 0; i < 256; i++)
    {
        fread(&colors[i].r, 1, 1, fichier);
        fread(&colors[i].g, 1, 1, fichier);
        fread(&colors[i].b, 1, 1, fichier);
    }
    SDL_SetColors(screen, colors, 0, 256);

    fclose(fichier);
}

void init_tables()
{
    int i;
    double h;
    double radian;

    for (i = 0; i < 360; i++)
    {
        radian = 2 * i * M_PI / 360;

        h = 2 + cos(radian);

        h_cos[i] = 256 * (h * cos(radian));
        h_sin[i] = 256 * (h * sin(radian));
    }
}

void init_texture()
{
    /* Creation du damier */

    FILE *fichier;
    int i;

    fichier = fopen(DATA, "r");
    for (i = 0; i < 128; i++)
    {
        fread(texture + 256 * i, 1, 128, fichier);
        memcpy(257 * 128 + texture + 256 * i, texture + 256 * i, 128);
    }
    for (i = 0; i < 128; i++)
    {
        fread(texture + 256 * i + 128, 1, 128, fichier);
        memcpy(256 * 128 + texture + 256 * i, texture + 256 * i + 128, 128);
    }
    fclose(fichier);
}
