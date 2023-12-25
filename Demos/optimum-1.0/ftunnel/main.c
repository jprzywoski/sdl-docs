#ifndef X11
#include <SDL/SDL_endian.h>
#endif
#include "main.h"

#ifndef M_PI
#define M_PI	3.14159265358979323846  /* pi */
#endif

#define RADIUS	64
#define DIST	256

float alpha[H / 8 + 1][W / 8 + 1];
float zede[H / 8 + 1][W / 8 + 1];
word *texture;

main(int argc, char *argv[])
{
    FILE *fichier;
    char *c;
    float Xd, Z, dalpha = 0;
    float aa = 0, thet = 0;             /*alpha theta de la visee en spherique */
    float eaa = 0;                      /* ecran alpha : rotation de l'ecran au bout du vecteur visee */
    float exx, exy, exz;                /* vecteur ex normal */
    float eyx, eyy, eyz;                /* ey */

    /* Init X */
    if (init_x(0, 0, W, H + 1, 16, NAME) != 16)
    {
        fprintf(stderr, "Erreur :\n");
        fprintf(stderr, "  Vos couleurs sont codees sur %d bits\n", depth);
        fprintf(stderr, "  La demo code les couleurs sur 16 bits\n");
        close_x();
        exit(1);
    }
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
        /*** pfff trigo 3d en coord spherique = caca ***/
        exx = -sin(aa);
        exy = cos(aa);
        exz = 0;
        eyx = -cos(aa) * sin(thet);
        eyy = -sin(aa) * sin(thet);
        eyz = cos(thet);

        exx *= 2;
        exy *= 2;
        exz *= 2;
        eyx *= 2;
        eyy *= 2;
        eyz *= 2;

        Xd = sin(dalpha) * 0.9;
        Z = sin(aa * 0.1 - thet * 0.2 + dalpha * 0.12001) * 700;

        aa += 0.004;
        /*aa=0; */
        thet += 0.006203;
        /*thet = M_PI/2; */
        eaa += 0.002;
        dalpha += 0.01;

        do_precalc(Xd, Z, dalpha, 4 * cos(aa) * cos(thet), 4 * sin(aa) * cos(thet), 4 * sin(thet), cos(eaa) * exx + sin(
                       eaa) * eyx, cos(eaa) * exy + sin(eaa) * eyy, cos(eaa) * exz + sin(eaa) * eyz, -sin(eaa) * exx + cos(eaa) *
                   eyx, -sin(eaa) * exy + cos(eaa) * eyy, -sin(eaa) * exz + cos(eaa) * eyz);

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

void do_precalc(float Xd, float Z, float dalpha, float cx, float cy, float cz,  /* vecteur de visee central */
                float vx1, float vy1, float vz1,                                                /* vecteur selon X ecran */
                float vx2, float vy2, float vz2                                                 /* vecteur selon Y ecran  */
               )
{
    int i, j;
    float t;
    float prec1, prec2;
    float prec3;
    float x, y, z;

    prec3 = (Xd * Xd - 1);
    vx1 /= W / 8.0;
    vy1 /= W / 8.0;
    vz1 /= W / 8.0;
    for (j = 0; j <= H / 8; j++)
    {
        x = cx - vx1 * 4 / W + (j - H / 16) * 8 * vx2 / H;
        y = cy - vy1 * 4 / W + (j - H / 16) * 8 * vy2 / H;
        z = cz - vz1 * 4 / W + (j - H / 16) * 8 * vz2 / H;
        for (i = 0; i <= W / 8; i++)
        {
            x += vx1;
            y += vy1;
            z += vz1;

            prec1 = x * Xd;
            prec2 = (y * y + x * x);
            if (prec2 >= 0.00001)
            {
                t = (-prec1 + sqrt(prec1 * prec1 - prec3 * prec2)) / prec2;

                /* remplacer sqrt par ident, sin, ... ca fait des
                 * trucs marrants */

                alpha[j][i] = (atan2(t * y, Xd + t * x) + dalpha) * 128 / M_PI;

                /* remplacer atan2 par "+" , "*" ou 1 des 2 termes...
                 * trucs marrants... */

                zede[j][i] = Z + 8 * t * z;

                /* pkoi 8 ?
                 * paceke sinon ca marche pas bien */
            }
            else                        /* trop dans l'alignement du tunnel ... ~div by 0 */
            {
                alpha[j][i] = 0;
                zede[j][i] = 1000;
            }
        }
    }
}

void refresh()
{
    int i, j;                           /* macro-bloc */
    int ii, jj;                         /* dans bloc (interpolation) */
    int a, ai, z, zi;                   /* 16-16 (fixed-point = lsb) (/pixel)*/
    int a0, a0i, z0, z0i;               /*  (/ligne) (colone de gauche)*/
    int a1, a1i, z1, z1i;               /*  (/ligne) (c de droite )*/
    int a22;
    float al0, al1, al2, al3;
    word *tmp, *tmp1;

    for (j = 0; j < H / 8; j++)
        for (i = 0; i < W / 8; i++)
        {
            al0 = alpha[j][i];
            a0 = ((int)65536 * al0);
            z0 = ((int)65536 * zede[j][i]);

            al1 = alpha[j + 1][i];
            if (abs(al1 - al0) > 100)   /* detecter la jonction entre les bords */
            {
                al1 = al0;
            }
            /* c'est du kludge pas efficace la...
             * il faudrait renormaliser tout ca et garder
             * la valeur modulo 1 par exemple, plutot que d'egaliser
             * (ce qui produit des marches d'escalier) */

            a22 = ((int)(65536 * al1));

            a0i = (a22 - a0) / 8;
            z0i = (((int)(65536 * zede[j + 1][i])) - z0) / 8;

            al2 = alpha[j][i + 1];
            if (abs(al2 - al0) > 100)
            {
                al2 = al0;
            }
            a1 = ((int)65536 * al2);
            z1 = ((int)65536 * zede[j][i + 1]);

            al3 = alpha[j + 1][i + 1];
            if (abs(al3 - al2) > 100)
            {
                al3 = al2;
            }
            a1i = (((int)(65536 * al3)) - a1) / 8;
            z1i = (((int)(65536 * zede[j + 1][i + 1])) - z1) / 8;

            tmp1 = (word *)buffer + i * 8 + (j * 8) * W;
            for (jj = 8; jj; jj--)
            {
                tmp = tmp1 += W;

                a = a0;
                z = z0;
                ai = (a1 - a0) >> 3;
                zi = (z1 - z0) >> 3;
                for (ii = 8; ii; ii--)
                {
                    *(tmp++) = *(texture + (((a >> 17) & 127) | ((z >> 8) & (127 << 7))));
                    a += ai;
                    z += zi;
                }
                a0 += a0i;
                z0 += z0i;

                a1 += a1i;
                z1 += z1i;
            }
        }
}
