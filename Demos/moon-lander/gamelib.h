/* gamelib.h - sprite libs, etc
 *
 * copyright 06/26/2001
 * dave blood - geekd@yahoo.com
 *
 *
 */

#ifndef _gamelib_h_
#define _gamelib_h_

#include <SDL/SDL.h>

#include <SDL/SDL_image.h>

#ifndef NOSOUND
#include <SDL/SDL_mixer.h>
#endif

typedef struct
{
    float x;
    float y;
    float x_vel;
    float y_vel;
    SDL_Surface *image;
    int w;
    int h;
} Sprite;

void CleanUp(void);

SDL_Surface *init_sdl(int width, int height, char *title, const char *win_icon);

void new_sprite(Sprite *s, const char *filename, int x, int y, int transparent, int alpha);
void new_sprite_surface(Sprite *temp, SDL_Surface *screen, int x, int y, int transparent);

#ifndef NOSOUND

Mix_Chunk *new_audio(char *filename);

void play_audio(Mix_Chunk *wave, int channel);

#endif

void draw_sprite(SDL_Surface *screen, Sprite obj);

short int collision_detect_perfect(Sprite obj1, Sprite obj2);

Uint32 getpixel(SDL_Surface *surface, int x, int y);
void press_any_key(SDLKey quit_key);

#endif
