#include "xutils.h"
#include "main.h"

#define LIM		25
#define TAILLE_BLOB	50
#define INTER		360
#define NB_BLOB		25
#define DEUXPI		6.2831853
#define DECAL		1
#define SATURATION	6000

static word *blob;
static unsigned int posx[INTER];
static unsigned int posy[INTER];

main(int argc, char *argv[])
{
    int i, j;
    float val;

    /* Securite */
    if ((W < TAILLE_BLOB) || (H < TAILLE_BLOB))
    {
        printf("BLOB Error:\n");
        printf("Ecran trop petit ou taille du blob trop grande !\n");
        exit(0);
    }
    /* Init X */
    if (init_x(0, 0, W, H, 16, NAME) != 16)
    {
        fprintf(stderr, "Erreur :\n");
        fprintf(stderr, "  Vos couleurs sont codees sur %d bits\n", depth);
        fprintf(stderr, "  La demo code les couleurs sur 16 bits\n");
        close_x();
        exit(1);
    }
    /* Init Position */
    for (i = 0; i < INTER; i++)
    {
        posx[i] = (W - TAILLE_BLOB) / 2 * (1 + cos(5 * i * DEUXPI / INTER));
        posy[i] = (H - TAILLE_BLOB) / 2 * (1 + sin(3 * i * DEUXPI / INTER));
    }
    /* Init Sphere */

    blob = (word *)malloc(2 * TAILLE_BLOB * TAILLE_BLOB);
    for (j = 0; j < TAILLE_BLOB; j++)
        for (i = 0; i < TAILLE_BLOB; i++)
        {
            val = (i - TAILLE_BLOB / 2) * (i - TAILLE_BLOB / 2) + (j - TAILLE_BLOB / 2) * (j - TAILLE_BLOB / 2);
            if (val < LIM * LIM)
            {
                val = val / (LIM * LIM);
                val = 1 - val;
                val = val * val;
            }
            else
            {
                val = 0;
            }
            blob[i + j * TAILLE_BLOB] = (65535 / NB_BLOB) * val;
        }
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
    int i;
    word *clear = (word *)buffer;
    static unsigned int locate = 0;

    /* On efface */
    for (i = 0; i < W * H; i++)
    {
        *(clear++) = 0;
    }
    /* On additionne les blobs */
    for (i = 0; i < NB_BLOB; i++)
    {
        cpy_blob(posx[(locate + DECAL * i) % INTER], posy[(locate + DECAL * i) % INTER]);
    }
    /* On convertit en couleurs */

    conv_coul();

    /* On increment les positions */

    locate = (locate + 1) % INTER;
}

void cpy_blob(x, y)
{
    static int val = W - TAILLE_BLOB;
    int i, j;
    word *tmp_ecr, *tmp_blob;

    tmp_ecr = (word *)buffer + x + y * W;
    tmp_blob = blob;
    for (j = 0; j < TAILLE_BLOB; j++)
    {
        for (i = 0; i < TAILLE_BLOB; i++)
        {
            *(tmp_ecr++) += *(tmp_blob++);
        }
        tmp_ecr += val;
    }
}

void conv_coul()
{
    int i;
    int val;
    word *tmp_ecr;

    tmp_ecr = (word *)buffer;
    for (i = 0; i < H * W; i++)
    {
        val = *tmp_ecr;
        if (val > SATURATION)
        {
            *(tmp_ecr++) = 65535;
        }
        else
        {
            val = (val << 5) / SATURATION;
            *(tmp_ecr++) = (val << 6) + val;
        }
    }
}
