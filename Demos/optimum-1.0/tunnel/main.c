#ifndef X11
#include <SDL/SDL_endian.h>
#endif
#include "main.h"

#define RADIUS	64
#define DIST	256

unsigned char angle[W * H];
unsigned char profondeur[W * H];
word *texture;

main(int argc, char *argv[])
{
    FILE *fichier;
    char *c;

    /* Init X */
    if (init_x(0, 0, W, H, 16, NAME) != 16)
    {
        fprintf(stderr, "Erreur :\n");
        fprintf(stderr, "  Vos couleurs sont codees sur %d bits\n", depth);
        fprintf(stderr, "  La demo code les couleurs sur 16 bits\n");
        close_x();
        exit(1);
    }
    /* Init tab */

    init_tab();

    /* Chargement de la texture */

    c = (char *)malloc(128 * 128 * 2 * sizeof (char));
    fichier = fopen("texture.data", "r");
    fgets(c, 2 * 128 * 128, fichier);
    fclose(fichier);
#ifndef X11
    {
        Uint16 *p = (Uint16 *)c;
        int i;

        for (i = 0; i < 128 * 128; ++i, ++p)
        {
            *p = SDL_SwapLE16(*p);
        }
    }
#endif
    texture = (word *)c;

    /* Init FPS */

    init_fps();
    /* Boucle */
    while (event_x())
    {
        refresh();
        update_x();
        next_fps();
    }
    /* Aff FPS */

    aff_fps(NAME);

    /* Close X */

    close_x();

    /* Fin */

    exit(0);
}

void refresh()
{
    static double turn = 0;
    unsigned char depX, depY;
    unsigned char *a = angle;
    unsigned char *p = profondeur;
    unsigned char x, y;
    word *tmp = (word *)buffer;
    int i, j;

    depX = W / 2 * (1 + sin(turn));
    depY = W / 2 * (1 + cos(turn));
    turn += 3.14 / 100;
    for (j = 0; j < H; j++)
        for (i = 0; i < W; i++)
        {
            x = ((*(a++) + depX) % 256) >> 1;
            y = ((*(p++) + depY) % 256) >> 1;
            *(tmp++) = *(texture + (y << 7) + x);
        }
}

void init_tab()
{
    int i, j;
    unsigned char *a = angle;
    unsigned char *p = profondeur;

    for (j = 0; j < H; j++)
        for (i = 0; i < W; i++)
        {
            *(a++) = (int)(128 + atan2(i - W / 2, j - H / 2) * 255 / 6.24);
            *(p++) = (RADIUS * DIST) / sqrt((i - W / 2) * (i - W / 2) + (j - H / 2) * (j - H / 2));
        }
}
