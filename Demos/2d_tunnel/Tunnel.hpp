#include <malloc.h>
#include <math.h>
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#endif
#include <SDL/SDL.h>

#define PI 3.1415926535f

// une petite class tunnel

class CTunnel
{
public:
    int w;                              // hauteur
    int h;                              // Largeur
    int speed;                          // vitesse, mais ca sert pas
    int *TAngle;                        // lookup des angles
    int *TDepth;                        // lookup des profondeur

    void Set(int, int);
    void Precalc(int);
    void Draw(SDL_Surface *buffer, SDL_Surface *map, int, int);
    void Free(void);
}

Tunnel;

// sert a allouer la memoire pour les lookups ....
void CTunnel::Set(int width, int height)
{
    w = width;
    h = height;

    TAngle = new int[w * h];
    TDepth = new int[w * h];
}

// Precalcul des tables
void CTunnel::Precalc(int diametre)     // diameter = 8, 16, 32, 64 .....
{
    int cx = w / 2;
    int cy = h / 2;

    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            TAngle[x + y * w] = (int)(atan2(cy - y, cx - x) * 256 / PI);
            TDepth[x + y * w] = (int)(256 * diametre / sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy)));
        }
    }
}

// Et on dessine ! :)
void CTunnel::Draw(SDL_Surface *buffer, SDL_Surface *map, int addx, int addy)
{
    Uint32 *buffer_dat;                 // pointeur sur le buffer (ou sera dessiner le tunnel)
    Uint32 *map_dat;                    // pointeur sur la texture
    int texel_x, texel_y;

    buffer_dat = (Uint32 *)buffer->pixels;
    map_dat = (Uint32 *)map->pixels;
    // on boucle sur l'ecran
    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            // on choppe le pixel x,y par rapport aux tables (&255 car la texture fait
            // 256*256)
            texel_x = (TAngle[x + y * w] + addx) & 255;
            texel_y = (TDepth[x + y * w] + addy) & 255;

            // et on le place sur la destination
            buffer_dat[x + y * w] = map_dat[texel_x + texel_y * map->w];
        }
    }
}

void CTunnel::Free(void)
{
    // on libere les lookups....
    delete[] TAngle;
    delete[] TDepth;
}
