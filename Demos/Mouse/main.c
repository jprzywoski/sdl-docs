/*
 * SDL Custom mouse Demonstration program
 *
 * 1999 Phoenix Kokido
 * members.xoom.com/kokido
 * kokido@postmark.net
 *
 * Free code, don't care what happens to it, don't care to much about the graphics
 * either
 *
 * Usage
 *
 * cmouse
 *      or
 * cmouse --fullscreen
 *
 */

#include <SDL/SDL.h>

SDL_Surface *screen;

typedef struct
{
    SDL_Surface *image;
    SDL_Surface *bgsave;
    Uint16 x;
    Uint16 y;
    Uint16 ox;
    Uint16 oy;
} sprite, *sprite_ptr;

void RS_Blit(SDL_Surface *bitmap, Sint16 x, Sint16 y);
void play_game();

SDL_Surface *prep_image(char *file);

void draw_sprite(sprite *s);
void erase_sprite(sprite *s);

void RS_Blit(SDL_Surface *bitmap, Sint16 x, Sint16 y)
{
    SDL_Rect dest;

    dest.x = x;
    dest.y = y;
    dest.w = bitmap->w;
    dest.h = bitmap->h;
    SDL_BlitSurface(bitmap, NULL, screen, &dest);
}

SDL_Surface *prep_image(char *file)
{
    SDL_Surface *image;

    image = SDL_LoadBMP(file);
    if (image == NULL)
    {
        fprintf(stderr, "Couldn't load %s: %s\n", file, SDL_GetError());
        return NULL;
    }
    return image;
}

void get_bg(SDL_Surface *surface, int x, int y)
{
    /* Blits a surface sized chunk of background to that surface */
    SDL_Rect src;
    SDL_Rect dst;

    src.x = x;
    src.y = y;
    src.w = surface->w;
    src.h = surface->h;

    dst.x = 0;
    dst.y = 0;
    dst.w = surface->w;
    dst.h = surface->h;
    SDL_BlitSurface(screen, &src, surface, &dst);
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    if (argc > 2)
    {
        printf("usage\ncmouse\nor cmouse --fullscreen");
        return 0;
    }
    if (argc == 2)
    {
        if (!strcmp(argv[1], "--fullscreen"))
        {
            screen = SDL_SetVideoMode(640, 480, 16, SDL_SWSURFACE | SDL_FULLSCREEN);
        }
        else
        {
            printf("usage\ncmouse\nor cmouse --fullscreen");
            return 0;
        }
    }
    else
    {
        screen = SDL_SetVideoMode(640, 480, 16, SDL_SWSURFACE);
    }
    /* Initialize the display in a 640x480 8-bit palettized mode */
    if (screen == NULL)
    {
        fprintf(stderr, "Couldn't set 640x480x8 video mode: %s\n", SDL_GetError());
        exit(1);
    }
    play_game();

    SDL_Quit();
    exit(0);
}

void play_game()
{
    SDL_Event event;
    Uint8 *keys;
    int x, y;
    int ox, oy;
    int done = 0, translucent = 0;

    /* The buttons and mouse cursor surfaces */
    SDL_Surface *button1;
    SDL_Surface *button2;
    SDL_Surface *cursor;

    /* Used to hold background oblitered when mouse is blitted */
    SDL_Surface *cursor_save;

    /* Hide the existing cursor */

    SDL_ShowCursor(0);

    /* Load the cursor, and set the color key (100% blue for this one) */

    cursor = prep_image("cursor.bmp");
    SDL_SetColorKey(cursor, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(cursor->format, 0, 0, 255));

    /* Set up the save, by loading the cursor image it makes sure it is the
     * same size.  This should'nt be color keyed */

    cursor_save = prep_image("cursor.bmp");

    /* Load the buttons */
    button1 = prep_image("button1.bmp");
    button2 = prep_image("button2.bmp");

    x = 0;
    y = 0;
    get_bg(cursor_save, x, y);

    RS_Blit(button2, 0, 0);
    RS_Blit(button1, 640 - button1->w, 480 - button1->h);
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    do
    {
        SDL_GetMouseState(&x, &y);
        while (SDL_PollEvent(&event))
        {
            ox = x;
            oy = y;                     /* Save old values for erasing the cursor */
            if (event.type == SDL_QUIT)
            {
                done = 1;
                return;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if ((x > 640 - button1->w) && (x < 640))
                {
                    if ((y > 480 - button1->h) && (y < 480))
                    {
                        done = 1;
                    }
                }
                if ((x > 0) && (x < button2->w))
                {
                    if ((y > 0) && (y < button2->h))
                    {
                        if (translucent)
                        {
                            SDL_SetAlpha(cursor, SDL_SRCALPHA, 0);
                            translucent = 0;
                        }
                        else
                        {
                            SDL_SetAlpha(cursor, SDL_SRCALPHA, 127);
                            translucent = 1;
                        }
                    }
                }
            }
        }
        keys = SDL_GetKeyState(NULL);
        if (keys[SDLK_ESCAPE] == SDL_PRESSED)
        {
            done = 1;
        }
        /* Draw anything to the screen before drawing the cursor, or
         * else the cursor may be covered or and it may not restore the
         * background correctly */

        get_bg(cursor_save, x, y);
        RS_Blit(cursor, x, y);

        /* In real life redrawing the whole screen is bad (read as "slow") */
        SDL_UpdateRect(screen, 0, 0, 0, 0);

        RS_Blit(cursor_save, x, y);
    }
    while (!done);
    /* Free the surfaces */
    SDL_FreeSurface(button1);
    SDL_FreeSurface(button2);
    SDL_FreeSurface(cursor);
    SDL_FreeSurface(cursor_save);
}
