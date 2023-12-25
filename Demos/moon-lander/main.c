/* moon_lander.c
 *
 * copyright 06/26/2001
 * magigames and dave blood - geekd@yahoo.com
 *
 * 07/03/2001 - Mike Heckman
 *  Added pause capability and compiler options to turn off sound
 *
 * 08/14/2001 - Mike Heckman added AI to play demo game, geekd added options
 * and save/load user options, random backgrounds, and merged MH's AI in
 *
 * 08/16/2001 - found most memory leaks and patched them (geekd)
 *
 * 08/17/2001 - cleaned up some of the messy code, fixed last of the memory leaks,
 *  added ryan daniels' ship and thrusters
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

#ifndef WIN32_BUILD
#include <pwd.h>
#endif

#include "gamelib.h"
#include "DT_drawtext.h"
#include <SDL/SDL_image.h>

#define XSIZE		640
#define YSIZE		480
#define TERRAIN_YSIZE	(YSIZE / 2)
#define FPS		(1000 / 35)
#define DATAPATH	""

#define FRESHRUN	0
#define GAMEOVER	1
#define LOST		2
#define WON		3
#define NEWGAME		4
#define AIGAME		5
#define BONUS_SHIP	6

/************************************************/

typedef struct
{
    int r;
    int g;
    int b;
} RGBcolor;

/************************************************/
typedef struct
{
    float difference;
    float vdiff;
    float max_y;
    float target;
    int pad;
    int distance;
    int direction;
    int state;
} AI;

/************************************************/
typedef struct
{
    Sprite sprite;
    int landing_x[5];
    int landing_y[5];
    int landing_w[5];
    float landing_speed[5];
    int landing_score[5];
    RGBcolor landing_color[5];
    int difficulty;
    int fuel;
    int num_landings;
} Level;

/************************************************/
typedef struct
{
    int ships_remaining;
    unsigned int fuel;
    int score;
    int difficulty;
    float gravity;
    int big_font;
    int small_font;
    int demo_mode;
    int autopilot;
    int state;
    int ActualTime, LastTime;
    int landing_pad;
    int back_no;
    SDL_Surface *screen;
    Sprite explosion[25];
    Sprite ship;
    Sprite background;
    Sprite thrust;
    Sprite thrustb;
    Sprite thrust_left;
    Sprite thrust_right;
    Sprite miniship;
    Sprite logo;
    Sprite gameover_screen;
    Sprite magigames;
    Level current_level;
    int opt_prog_grav;
    int opt_lp_bonus;
    int opt_lp_warn;
    int opt_num_lives;
    int opt_fancy_terrain;
    AI ai;

#ifndef NOSOUND

    Mix_Chunk *engine;
    Mix_Chunk *new_life;
    Mix_Chunk *explosion_a;
    Mix_Chunk *on;
    Mix_Chunk *off;
    Mix_Chunk *ready;
    Mix_Chunk *go;
    Mix_Chunk *eagle_landed;
#endif
} Game;

/************************************************/

void delay(int t)
{
    SDL_Event event;
    Uint8 *key_table;
    int i;

    for (i = 0; i < t; i++)
    {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT)
        {
            exit(0);
        }
        key_table = SDL_GetKeyState(NULL);
        if (key_table[SDLK_q])
        {
            exit(0);
        }
        SDL_Delay(20);
    }
}

/************************************************/

void frame_rate_limiter(Game *game)
{
    /* timer - so that things run at an even speed regardless of cpu speed */

    game->ActualTime = SDL_GetTicks();
    if (game->ActualTime < game->LastTime + FPS)
    {
        SDL_Delay(game->LastTime + FPS - game->ActualTime);
    }
    game->LastTime = game->ActualTime;
}

/************************************************/

void get_new_background(Game *game)
{
    char filename[1024];
    DIR *dir;
    struct dirent *files[100];
    int done = 0;
    int count = 0;

    /* read images/backgrounds dir and choose a random image from there.
     *  put it's filename in image_file
     */

    sprintf(filename, "%simages/backgrounds", DATAPATH);
    if (!(dir = opendir(filename)))
    {
        /* error */
        printf("cannot open dir %s\n", filename);
        exit(0);
    }
    while (!done)
    {
        if (files[count] = readdir(dir))
        {
            /*printf("I see - %d %s\n", count, files[count]->d_name); */
            count++;
        }
        else
        {
            done = 1;
        }
        if (count > 99)
        {
            done = 1;
        }
    }
    closedir(dir);
    if (!count)
    {
        printf("I got no images for backgrounds - ERROR\n");
        exit(0);
    }
    game->back_no++;
    if (game->back_no < 2)
    {
        game->back_no = 2;
    }
    if (game->back_no >= count)
    {
        game->back_no = 2;
    }
    if (game->background.image != NULL)
    {
        /*printf("about to free background\n"); */
        SDL_FreeSurface(game->background.image);
    }
    else
    {
        /*printf("background was NULL\n"); */
    }
    /*printf("about to get new background: %d\n", game->back_no ); */

    sprintf(filename, "%simages/backgrounds/%s", DATAPATH, files[game->back_no]->d_name);

    /* printf("got %s\n", filename); */

    new_sprite(&(game->background), filename, 0, 0, 0, 0);
    /*  printf("got new background\n"); */
}

/************************************************/

void save_game(Game *game)
{
    char filename[200];
    FILE *file;

#ifndef WIN32_BUILD

    struct passwd *pwp;

    /* get user home dir */
    if (!(pwp = getpwuid(getuid())))
    {
        /* error */
        printf("attempt to get current user failed\n");
        exit(0);
    }
    sprintf(filename, "%s/.moon_lander", pwp->pw_dir);
#endif

#ifdef WIN32_BUILD
    sprintf(filename, "moon_lander.conf");
#endif

    printf("saving game options in %s\n", filename);
    if ((file = fopen(filename, "w")) != NULL)
    {
        fprintf(file, "%d %d %d %d %d", game->opt_num_lives, game->opt_lp_bonus, game->opt_lp_warn, game->opt_prog_grav,
                game->opt_fancy_terrain);
    }
    else
    {
        printf("cannot open file for saving: %s\n", filename);
    }
    fclose(file);
}

/************************************************/

void load_game(Game *game)
{
    char filename[200];
    FILE *file;

#ifndef WIN32_BUILD

    struct passwd *pwp;

    /* get user home dir */
    if (!(pwp = getpwuid(getuid())))
    {
        /* error */
        printf("attempt to get current user failed\n");
        exit(0);
    }
    sprintf(filename, "%s/.moon_lander", pwp->pw_dir);
#endif

#ifdef WIN32_BUILD
    sprintf(filename, "moon_lander.conf");
#endif

    printf("reading game options in %s\n", filename);
    if ((file = fopen(filename, "r")) != NULL)
    {
        fscanf(file, "%d %d %d %d %d", &game->opt_num_lives, &game->opt_lp_bonus, &game->opt_lp_warn,
               &game->opt_prog_grav, &game->opt_fancy_terrain);
    }
    else
    {
        printf("cannot open file for reading: %s - loading defaults\n", filename);

        /* defaults */
        game->opt_num_lives = 3;
        game->opt_lp_bonus = 1;
        game->opt_lp_warn = 1;
        game->opt_prog_grav = 1;
        game->opt_fancy_terrain = 1;

        return;
    }
    fclose(file);
}

/************************************************/

void options(Game *game)
{
    int done = 0;
    int *selected;
    int position = 0;
    char options[5][100];
    char display_string[150];
    char selected_text[2];
    int count;
    int value[5];
    Uint8 *key_table;
    SDL_Event event;

    sprintf(options[0], "%s", "Fancy Terrain");
    sprintf(options[1], "%s", "Progressive Gravity");
    sprintf(options[2], "%s", "Landing Pad Speed Warning");
    sprintf(options[3], "%s", "Variable Speed Landing Pads");
    sprintf(options[4], "%s", "Number Of Ships");
    /* clear event buffer */
    while (SDL_PollEvent(&event)) {}
    while (done == 0)
    {
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT)
        {
            exit(0);
        }
        key_table = SDL_GetKeyState(NULL);
        if (key_table[SDLK_q])
        {
            exit(0);
        }
        if (key_table[SDLK_ESCAPE])
        {
            done = 1;
        }
        if (key_table[SDLK_UP])
        {
            position--;
            if (position < 0)
            {
                position = 4;
            }
        }
        if (key_table[SDLK_DOWN])
        {
            position++;
            if (position > 4)
            {
                position = 0;
            }
        }
        if (key_table[SDLK_RETURN])
        {
            (*selected)++;
            if (position == 4)
            {
                if (*selected > 5)
                {
                    *selected = 1;
                }
            }
            else
            {
                if (*selected > 1)
                {
                    *selected = 0;
                }
            }
        }
        /* point selected at proper value */
        if (position == 0)
        {
            selected = &(game->opt_fancy_terrain);
        }
        else if (position == 1)
        {
            selected = &(game->opt_prog_grav);
        }
        else if (position == 2)
        {
            selected = &(game->opt_lp_warn);
        }
        else if (position == 3)
        {
            selected = &(game->opt_lp_bonus);
        }
        else if (position == 4)
        {
            selected = &(game->opt_num_lives);
        }
        value[0] = game->opt_fancy_terrain;
        value[1] = game->opt_prog_grav;
        value[2] = game->opt_lp_warn;
        value[3] = game->opt_lp_bonus;
        value[4] = game->opt_num_lives;

        /* draw the options */
        draw_sprite(game->screen, game->background);
        DT_DrawText("OPTIONS", game->screen, game->big_font, 260, 50);
        DT_DrawText("Arrow Keys Select", game->screen, game->small_font, 260, 75);
        DT_DrawText("ENTER changes value", game->screen, game->small_font, 260, 90);
        for (count = 0; count < 5; count++)
        {
            if (position == count)
            {
                sprintf(selected_text, "%s", "**");
            }
            else
            {
                sprintf(selected_text, "%s", "  ");
            }
            sprintf(display_string, "%s %s - %d", selected_text, options[count], value[count]);
            DT_DrawText(display_string, game->screen, game->big_font, 50, 150 + (count * 50));
        }
        delay(3);
        SDL_Flip(game->screen);
    }                                   /* end while */

    save_game(game);

    /* clear event buffer */
    delay(5);
    while (SDL_PollEvent(&event)) {}
}

/************************************************/

void remaining_ships(Game *game)
{
    int count;

    /* Draw in the remaining ships */
    for (count = 0; count < game->ships_remaining; count++)
    {
        game->miniship.x = (22 * count);
        game->miniship.y = 15;
        draw_sprite(game->screen, game->miniship);
    }
}

/************************************************/

void DrawPixel(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, int x, int y)
{
    Uint32 color = SDL_MapRGB(screen->format, R, G, B);
    Uint16 *bufp;

    bufp = (Uint16 *)screen->pixels + y * screen->pitch / 2 + x;
    *bufp = color;
}

/************************************************/

void draw_line(SDL_Surface *screen, Uint8 R, Uint8 G, Uint8 B, int x, int y)
{
    for (; y < TERRAIN_YSIZE; y++)
    {
        DrawPixel(screen, R, G, B, x, y);
    }
}

/************************************************/

/* This draws a line that is composed of n repeating gradients
 *  implemented by BMD  */
void draw_terrain_line(SDL_Surface *screen, int x, int y)
{
    int gradient_iterations = 3;
    int dark_color = 80;
    int gradient_color_variance = 40;
    int gradient_height = (TERRAIN_YSIZE - y) / gradient_iterations;

    for (; y < TERRAIN_YSIZE; y++)
    {
        int height = TERRAIN_YSIZE - y;
        double gradient_ratio = (double)(height % gradient_height) / gradient_height;
        int color = gradient_ratio * gradient_color_variance + dark_color;

        DrawPixel(screen, color, color, color, x, y);
    }
}

/************************************************/

void random_level(Game *game)
{
    SDL_Surface *tmp;
    int x = 0;
    int y = 0;
    int yd = 0;
    int miny = 0;
    int maxy = 0;
    int yfluct = 0;
    int size = 0;
    int max = 0;
    float speed;
    RGBcolor rgb;
    float distance;
    int xdiff;
    int ydiff;
    char filename[100];

    /*printf("random level\n"); */

    game->current_level.num_landings = 0;
    game->current_level.difficulty = game->difficulty;

    /* printf("getting new background\n"); */

    get_new_background(game);
    while (game->current_level.num_landings == 0)
    {
        /* printf("starting while loop\n"); */
        if (game->current_level.sprite.image != NULL)
        {
            SDL_FreeSurface(game->current_level.sprite.image);
        }
        sprintf(filename, "%simages/blank_terrain.gif", DATAPATH);
        tmp = IMG_Load(filename);
        if (tmp == NULL)
        {
            fprintf(stderr, "Couldn't load %s: %s\n", filename, SDL_GetError());
            exit(1);
        }
        game->current_level.sprite.image = SDL_DisplayFormat(tmp);

        SDL_FreeSurface(tmp);

        SDL_LockSurface(game->current_level.sprite.image);

#ifndef WIN32_BUILD
        srandom(time(NULL));
        while ((y > 230) || (y < 10))
        {
            y = random() % TERRAIN_YSIZE;
        }
        miny = ((random() % (TERRAIN_YSIZE / 2)) + 10);
        maxy = ((random() % (TERRAIN_YSIZE / 2) + TERRAIN_YSIZE / 2) - 10);
#endif

#ifdef WIN32_BUILD
        srand(time(NULL));
        while ((y > 230) || (y < 10))
        {
            y = rand() % TERRAIN_YSIZE;
        }
        miny = ((rand() % (TERRAIN_YSIZE / 2)) + 10);
        maxy = ((rand() % (TERRAIN_YSIZE / 2) + TERRAIN_YSIZE / 2) - 10);
#endif
        if (y > (TERRAIN_YSIZE / 2))
        {
            yd = -1;
        }
        else
        {
            yd = 1;
        }
        /* start to draw */
        for (x = 0; x < XSIZE; x++)
        {
            if ((x % 3) == 0)
            {
#ifndef WIN32_BUILD
                yfluct = ((random() % 10) - 5);
#endif
#ifdef WIN32_BUILD
                yfluct = ((rand() % 10) - 5);
#endif
            }
            if (((y < maxy) && (y > miny)) || ((y > maxy) && (yfluct < 1)) || ((y < maxy) && (yfluct > -1)))
            {
                y = y + yfluct;
            }
            /* draw landscape */
            if (game->opt_fancy_terrain)
            {
                draw_terrain_line(game->current_level.sprite.image, x, y);
            }
            else
            {
                draw_line(game->current_level.sprite.image, 80, 80, 80, x, y);
            }
            /* check for change direction */
            if (yd == 1)
            {
                if (y > maxy)
                {
                    if ((x < (XSIZE - 60)) && (game->current_level.num_landings < 4))
                    {
                        /* landing pad */

                        size = (40 + (15 - (game->difficulty / 2)));
                        max = x + size;

                        game->current_level.landing_x[game->current_level.num_landings] = x;
                        game->current_level.landing_y[game->current_level.num_landings] = (y + TERRAIN_YSIZE);
                        game->current_level.landing_w[game->current_level.num_landings] = size;

                        /* get distance from center-top */
                        xdiff = ((XSIZE / 2) - x);
                        ydiff = (y);
                        if (xdiff == 0)
                        {
                            distance = ydiff;
                        }
                        else if (ydiff == 0)
                        {
                            distance = xdiff;
                        }
                        else
                        {
                            distance = sqrt((xdiff * xdiff) + (ydiff * ydiff));
                        }
                        game->current_level.landing_speed[game->current_level.num_landings] = 1.00;
                        game->current_level.landing_score[game->current_level.num_landings] = (((16 - (size - 45)) *
                                100) + ((distance / 4) * ((game->difficulty / 2) + 1)));

                        /* Default landing pad color is blue */
                        rgb.r = 0;
                        rgb.g = 0;
                        rgb.b = 255;
                        /* ----------- bonus colored landing pads if on ------- */
                        if (game->opt_lp_bonus)
                        {
                            if (game->current_level.num_landings)
                            {
#ifndef WIN32_BUILD
                                speed = (random() % 3);
#endif

#ifdef WIN32_BUILD
                                speed = (rand() % 3);
#endif
                                /* Magenta is for landings at .90 */
                                if (speed > 0)
                                {
                                    rgb.r = 255;
                                    rgb.g = 0;
                                    rgb.b = 255;
                                }
                                /* Green for landings at .80 */
                                if (speed > 1)
                                {
                                    rgb.r = 0;
                                    rgb.g = 255;
                                    rgb.b = 0;
                                }
                                /* Set the landing speed and score */
                                game->current_level.landing_speed[game->current_level.num_landings] -= (speed * .1);
                                game->current_level.landing_score[game->current_level.num_landings] += (speed * 100);
                            }
                        }
                        game->current_level.landing_color[game->current_level.num_landings] = rgb;
                        /* Write the landing pads onto the terrain */
                        for (; x < max; x++)
                        {
                            DrawPixel(game->current_level.sprite.image, rgb.r, rgb.g, rgb.b, x, y);
                            DrawPixel(game->current_level.sprite.image, rgb.r, rgb.g, rgb.b, x, y + 1);
                            DrawPixel(game->current_level.sprite.image, rgb.r, rgb.g, rgb.b, x, y + 2);
                            if (game->opt_fancy_terrain)
                            {
                                draw_terrain_line(game->current_level.sprite.image, x, y + 3);
                            }
                            else
                            {
                                draw_line(game->current_level.sprite.image, 80, 80, 80, x, y + 3);
                            }
                        }
                        x--;
                        game->current_level.num_landings++;
                    }
                    yd = -1;
#ifndef WIN32_BUILD
                    miny = ((random() % (TERRAIN_YSIZE / 2)) + 10);
                    maxy = ((random() % (TERRAIN_YSIZE / 2) + TERRAIN_YSIZE / 2) - 25);
#endif
#ifdef WIN32_BUILD
                    miny = ((rand() % (TERRAIN_YSIZE / 2)) + 10);
                    maxy = ((rand() % (TERRAIN_YSIZE / 2) + TERRAIN_YSIZE / 2) - 25);
#endif
                }
            }
            if (yd == -1)
            {
                if (y < miny)
                {
                    yd = 1;
#ifndef WIN32_BUILD
                    miny = ((random() % (TERRAIN_YSIZE / 2)) + 10);
                    maxy = ((random() % (TERRAIN_YSIZE / 2) + TERRAIN_YSIZE / 2) - 10);
#endif
#ifdef WIN32_BUILD
                    miny = ((rand() % (TERRAIN_YSIZE / 2)) + 10);
                    maxy = ((rand() % (TERRAIN_YSIZE / 2) + TERRAIN_YSIZE / 2) - 10);
#endif
                }
            }
            y = y + yd;
        }
        SDL_UnlockSurface(game->current_level.sprite.image);

        new_sprite_surface(&(game->current_level.sprite), game->current_level.sprite.image, 0, TERRAIN_YSIZE, 1);

        game->current_level.fuel = (700 - (game->difficulty * 25));
        if (game->current_level.fuel < 300)
        {
            game->current_level.fuel = 300;
        }
    }
}

/************************************************/

void draw_score(Game *game, int landing_score)
{
    char display_string[100];
    int count;

    sprintf(display_string, "Fuel %d", game->fuel);
    if (game->small_font > -1)
    {
        DT_DrawText(display_string, game->screen, game->small_font, 1, 1);
    }
    sprintf(display_string, "X Velocity %.2f", game->ship.x_vel);
    if (game->small_font > -1)
    {
        DT_DrawText(display_string, game->screen, game->small_font, 100, 1);
    }
    sprintf(display_string, "Y Velocity %.2f", game->ship.y_vel);
    if (game->small_font > -1)
    {
        DT_DrawText(display_string, game->screen, game->small_font, 250, 1);
    }
    sprintf(display_string, "Score: %d", game->score);
    if (game->small_font > -1)
    {
        DT_DrawText(display_string, game->screen, game->small_font, 500, 1);
    }
    if (landing_score)
    {
        for (count = 0; count < game->current_level.num_landings; count++)
        {
            sprintf(display_string, "%d", game->current_level.landing_score[count]);
            if (game->small_font > -1)
            {
                DT_DrawText(display_string, game->screen, game->small_font, (game->current_level.landing_x[count] + 5),
                            (game->current_level.landing_y[count] + 5));
            }
        }
    }
}

/************************************************/

void game_over(Game *game, int first_time)
{
    SDL_Event event;
    int x;
    int y = 375;
    int done = 0;
    int ticks = 0;
    Uint8 *key_table;
    int l_start;

    if (game->demo_mode)
    {
        first_time = 1;
        game->score = 0;
        game->difficulty = 0;
        game->ships_remaining = game->opt_num_lives;
        game->gravity = 0.05;
    }
    if (first_time == 0)
    {
        /* if they just lost, show them thier score and stuff and then pause for a sec */
        draw_sprite(game->screen, game->gameover_screen);
        draw_sprite(game->screen, game->logo);
        DT_DrawText("Game Over", game->screen, game->big_font, 252, 148);
        draw_score(game, 0);
        draw_sprite(game->screen, game->magigames);

        SDL_Flip(game->screen);
        delay(75);
    }
    /* start between game animation */
    l_start = 60;
    while (!done)
    {
        ticks++;

        draw_sprite(game->screen, game->gameover_screen);
        draw_sprite(game->screen, game->logo);
        draw_sprite(game->screen, game->magigames);
        /*
         * show the game name, etc
         * if they just lost, show them thier score and stuff
         */
        if (first_time == 0)
        {
            DT_DrawText("Game Over", game->screen, game->big_font, 252, 148);
        }
        draw_score(game, 0);
        DT_DrawText("Arrow keys control the ship", game->screen, game->big_font, 100, 100);
        DT_DrawText("Q quit   P pause   ESC options", game->screen, game->big_font, 75, 125);
        DT_DrawText("Press ENTER to play", game->screen, game->big_font, 175, 170);

        DT_DrawText("Score for each round = landing pad score + remaining fuel.", game->screen, game->small_font, 150,
                    280);
        DT_DrawText("Safe Landing requires X velocity < 0.5 and Y velocity < indicated by landing pad color.",
                    game->screen, game->small_font, 55, 300);
        DT_DrawText("Free ship every 10,000 points", game->screen, game->small_font, 225, 340);

        /* do the pad display thing */
        SDL_LockSurface(game->screen);
        for (x = 0; x < XSIZE; x++)
        {
            if ((x > (XSIZE / 8 - 25)) && (x < (XSIZE / 8 + 30)) && (ticks > l_start * 1) && (ticks < (l_start * 5)))
            {
                DrawPixel(game->screen, 0, 0, 255, x, y);
                DrawPixel(game->screen, 0, 0, 255, x, y + 1);
                DrawPixel(game->screen, 0, 0, 255, x, y + 2);
            }
            if ((x > (XSIZE / 2.67 - 25)) && (x < (XSIZE / 2.67 + 30)) && (game->opt_lp_warn) && (ticks > (l_start *
                    2)) && (ticks < (l_start * 6)))
            {
                DrawPixel(game->screen, 255, 0, 0, x, y);
                DrawPixel(game->screen, 255, 0, 0, x, y + 1);
                DrawPixel(game->screen, 255, 0, 0, x, y + 2);
            }
            if ((x > (XSIZE / 1.6 - 25)) && (x < (XSIZE / 1.6 + 30)) && (game->opt_lp_bonus) && (ticks > (l_start *
                    3)) && (ticks < (l_start * 7)))
            {
                DrawPixel(game->screen, 0, 255, 0, x, y);
                DrawPixel(game->screen, 0, 255, 0, x, y + 1);
                DrawPixel(game->screen, 0, 255, 0, x, y + 2);
            }
            if ((x > (XSIZE / 1.14 - 25)) && (x < (XSIZE / 1.14 + 30)) && (game->opt_lp_bonus) && (ticks > (l_start *
                    4)) && (ticks < (l_start * 8)))
            {
                DrawPixel(game->screen, 255, 0, 255, x, y);
                DrawPixel(game->screen, 255, 0, 255, x, y + 1);
                DrawPixel(game->screen, 255, 0, 255, x, y + 2);
            }
        }
        SDL_UnlockSurface(game->screen);
        if ((ticks > (l_start * 1)) && (ticks < (l_start * 5)))
        {
            DT_DrawText("Landing Vel. = 1.0", game->screen, game->small_font, (XSIZE / 8 - 50), y + 10);
        }
        if ((game->opt_lp_warn) && (ticks > (l_start * 2)) && (ticks < (l_start * 6)))
        {
            DT_DrawText("Red  = Too Fast!", game->screen, game->small_font, (XSIZE / 2.67 - 50), y + 10);
        }
        if ((game->opt_lp_bonus) && (ticks > (l_start * 3)) && (ticks < (l_start * 7)))
        {
            DT_DrawText("Landing Vel. = 0.8", game->screen, game->small_font, (XSIZE / 1.6 - 50), y + 10);
        }
        if ((game->opt_lp_bonus) && (ticks > (l_start * 4)) && (ticks < (l_start * 8)))
        {
            DT_DrawText("Landing Vel. = 0.9", game->screen, game->small_font, (XSIZE / 1.14 - 50), y + 10);
        }
        if ((game->opt_lp_bonus) && (ticks > (l_start * 9)))
        {
            game->score = 0;
            game->difficulty = 0;
            game->ships_remaining = 0;
            game->gravity = 0.05;
            game->state = AIGAME;
            done = 1;
        }
#ifndef NOSOUND
        /* play a sound when the pads appear and dissapear */
        if ((ticks == (l_start * 1)) || ((ticks == (l_start * 2)) && (game->opt_lp_warn)) || ((ticks == (l_start *
                3)) && (game->opt_lp_bonus)) || ((ticks == (l_start * 4)) && (game->opt_lp_bonus)))
        {
            play_audio(game->on, 0);
        }
        if ((ticks == (l_start * 5)) || ((ticks == (l_start * 6)) && (game->opt_lp_warn)) || ((ticks == (l_start *
                7)) && (game->opt_lp_bonus)) || ((ticks == (l_start * 8)) && (game->opt_lp_bonus)))
        {
            play_audio(game->off, 0);
        }

#endif
        if (ticks > (l_start * 9))
        {
            ticks = 0;
        }
        SDL_Flip(game->screen);

        /* press ENTER to play */
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT)
        {
            exit(0);
        }
        key_table = SDL_GetKeyState(NULL);
        if (key_table[SDLK_q])
        {
            exit(0);
        }
        if (key_table[SDLK_RETURN])
        {
            done = 1;
            game->demo_mode = 0;
            game->score = 0;
            game->difficulty = 0;
            game->ships_remaining = game->opt_num_lives;
            game->gravity = 0.05;
            game->state = NEWGAME;
        }
        if (key_table[SDLK_ESCAPE])
        {
            options(game);
        }
        frame_rate_limiter(game);
    }                                   /* end while */

    /* reset all the stuff for a new game */

    game->score = 0;
    game->difficulty = 0;
    game->ships_remaining = (game->opt_num_lives - 1);
    game->gravity = 0.05;
}

/************************************************/

void new_round(Game *game, int died_won)
{
    SDL_Event event;
    char display_string[100];

    /*
     * printf("new round\n");
     * clear event buffer
     */
    while (SDL_PollEvent(&event)) {}
    /* reset ship */
    game->ship.x = XSIZE / 2;
    game->ship.y = 1;
    game->ship.y_vel = 0;
    game->ship.x_vel = 0;
    if (died_won == 0)                  /* died */
    {
        game->ships_remaining--;
        game->fuel = game->current_level.fuel;
        /*level = current_level; */
    }
    else if (died_won == 1)             /* won */
    {
        game->difficulty++;

        /* get new level */

        random_level(game);

        game->fuel = game->current_level.fuel;
        /* prograssive gravity if on */
        if (game->opt_prog_grav)
        {
            game->gravity = 0.05 + (game->difficulty * 0.001);
        }
    }
    else if (died_won == 3)             /* starting new game */
    {
        random_level(game);
        game->fuel = game->current_level.fuel;
    }
    /* draw start round screen */

    draw_sprite(game->screen, game->background);
    remaining_ships(game);
    draw_sprite(game->screen, game->ship);
    draw_sprite(game->screen, game->current_level.sprite);

    /* message 1  */

    draw_score(game, 1);
    if (game->difficulty == 4)
    {
        DT_DrawText("Not Bad", game->screen, game->big_font, 250, 250);
    }
    else if (game->difficulty == 9)
    {
        DT_DrawText("You think you're hot shit, huh?", game->screen, game->big_font, 125, 250);
    }
    else if (game->difficulty == 14)
    {
        DT_DrawText("Starfleet called, they want to", game->screen, game->big_font, 100, 235);
        DT_DrawText("offer you a job", game->screen, game->big_font, 174, 260);
    }
    else if (game->difficulty == 19)
    {
        DT_DrawText("The force is strong in you", game->screen, game->big_font, 125, 250);
    }
    sprintf(display_string, "Level: %d", (game->difficulty) + 1);
    DT_DrawText(display_string, game->screen, game->big_font, 250, 150);

    /* flip screen so they show up */
#ifndef NOSOUND
    play_audio(game->ready, 1);
#endif
    SDL_Flip(game->screen);
    if (game->demo_mode == 1)
    {
        return;
    }
    delay(75);

    /* message 2  */

    draw_sprite(game->screen, game->background);
    remaining_ships(game);
    draw_sprite(game->screen, game->ship);
    draw_sprite(game->screen, game->current_level.sprite);

    draw_score(game, 1);

    DT_DrawText("Ready...", game->screen, game->big_font, 250, 150);

    /* flip screen so they show up */
#ifndef NOSOUND
    play_audio(game->ready, 1);
#endif
    SDL_Flip(game->screen);

    delay(70);

    draw_sprite(game->screen, game->background);
    remaining_ships(game);
    draw_sprite(game->screen, game->ship);
    draw_sprite(game->screen, game->current_level.sprite);
    /* Draw in the remaining ships */

    /* display fuel  */

    draw_score(game, 1);

    DT_DrawText("GO!", game->screen, game->big_font, 290, 150);

    /* flip screen so they show up */
#ifndef NOSOUND
    play_audio(game->go, 0);
#endif
    SDL_Flip(game->screen);

    delay(25);
    /* clear event buffer */
    while (SDL_PollEvent(&event)) {}
}

/************************************************/

void explode(Sprite exploder, Sprite terrain, Game *game)
{
    int count;

#ifndef NOSOUND
    play_audio(game->explosion_a, 0);
#endif
    for (count = 1; count < 27; count++)
    {
        game->explosion[count - 1].x = ((exploder.x + (exploder.w / 2)) - (game->explosion[count - 1].w / 2));
        game->explosion[count - 1].y = ((exploder.y + (exploder.h / 2)) - (game->explosion[count - 1].h / 2));

        draw_sprite(game->screen, game->background);
        remaining_ships(game);
        draw_sprite(game->screen, terrain);
        if (count < 26)
        {
            draw_sprite(game->screen, game->explosion[count - 1]);
        }
        /* display fuel  */

        draw_score(game, 1);

        /* Update the screen */

        SDL_Flip(game->screen);

        /* timer - so that things run at an even speed regardless of cpu speed */

        frame_rate_limiter(game);
    }
    delay(30);
}

/************************************************/

void win(Game *game, int bonus)
{
    game->score += game->current_level.landing_score[game->landing_pad];

#ifndef NOSOUND
    Mix_HaltChannel(1);
    /* congrats sound here */
    play_audio(game->eagle_landed, 0);
#endif

    draw_sprite(game->screen, game->background);
    remaining_ships(game);
    draw_sprite(game->screen, game->current_level.sprite);
    draw_score(game, 1);
    draw_sprite(game->screen, game->ship);
    if (game->big_font > -1)
    {
        DT_DrawText("Excellent", game->screen, game->big_font, (XSIZE / 2) - 66, (YSIZE / 3) - 9);
    }
    /* Update the screen */
    SDL_Flip(game->screen);
    delay(80);
    if (bonus)
    {
        game->ships_remaining++;

#ifndef NOSOUND
        play_audio(game->new_life, 1);
#endif

        draw_sprite(game->screen, game->background);
        remaining_ships(game);
        draw_sprite(game->screen, game->current_level.sprite);
        draw_score(game, 1);
        draw_sprite(game->screen, game->ship);
        if (game->big_font > -1)
        {
            DT_DrawText("Excellent", game->screen, game->big_font, (XSIZE / 2) - 66, (YSIZE / 3) - 9);
            DT_DrawText("Bonus Ship", game->screen, game->big_font, (XSIZE / 2) - 70, (YSIZE / 3) + 40);
        }
        /* Update the screen */

        SDL_Flip(game->screen);
        delay(80);
    }
}

/************************************************/

void gameai(int *left, int *right, int *down, Game *game)
{
    int count;

    /* -------------------------------------------------------------
     * Game AI -- written by Michael Heckman 07/17/2001
     *
     * Ok, so we're going to divide the AI into several states:
     *
     * 0 => Figure out the direction
     * 1 => Burn until we're half way there
     * 2 => Burn in the opposite direction
     * 3 => once we've reached the pad, cancel out any remaining
     * velocity.
     * 4 => Freefall with minor course changes
     *
     * ------------------------------------------------------------- */
    *left = 0;
    *right = 0;
    /*
     * AI State
     * --- state 0, locate the pad and init variables ---
     */
    if (game->ai.state == 0)
    {
        game->ai.vdiff = 0;

        /* Find which pad we're going to take. */
        game->ai.pad = 0;
        game->ai.distance = 9999;
        for (count = 0; count < game->current_level.num_landings; count++)
        {
            if (abs(320 - game->current_level.landing_x[count]) < game->ai.distance)
            {
                game->ai.distance = abs(320 - game->current_level.landing_x[count]);
                game->ai.pad = count;
            }
        }
        game->ai.max_y = game->current_level.landing_speed[game->ai.pad];

        /*
         * Calculate the half the difference between our current position
         * and the landing pad
         */
        game->ai.target = game->current_level.landing_x[game->ai.pad] + 1;
        game->ai.difference = (abs(game->ship.x - game->ai.target) / 2);
        /* Decide if we're going left or right */
        if (game->ai.target < game->ship.x)
        {
            game->ai.difference = game->ship.x - game->ai.difference - 1;
            game->ai.direction = 0;
        }
        else
        {
            game->ai.difference = game->ship.x + game->ai.difference;
            game->ai.direction = 1;
        }
        /*printf( "game->ai.difference: %d %d\n", game->ai.difference, game->ai.state ); */
    }
    /* Stay under the final velocity */
    if (game->ai.max_y < game->ship.y_vel)
    {
        *down = 1;
    }
    /* --- state 1 & 2, main flight --- */
    if (game->ai.direction == 0 && game->ai.state < 3)
    {
        if ((game->ai.state == 0) && (game->ship.x > game->ai.difference))
        {
            game->ai.state = 1;
        }
        if ((game->ai.state == 1) && (game->ship.x <= game->ai.difference))
        {
            game->ai.state = 2;
        }
        if ((game->ai.state == 2) && (game->ship.x <= game->ai.target || game->ship.x_vel > 0))
        {
            game->ai.state = 3;
        }
        if (game->ai.state == 1)
        {
            *left = 1;
        }
        if (game->ai.state == 2)
        {
            *right = 1;
        }
        /*printf( "game->ship.x1: %f %d %d %f\n", game->ship.x, game->ai.state, ( game->ship.x <= game->ai.target ), game->ai.target ); */
    }
    /* --- state 1 & 2, main flight --- */
    if (game->ai.direction == 1 && game->ai.state < 3)
    {
        if ((game->ai.state == 0) && (game->ship.x < game->ai.difference))
        {
            game->ai.state = 1;
        }
        if ((game->ai.state == 1) && (game->ship.x >= game->ai.difference))
        {
            game->ai.state = 2;
        }
        if ((game->ai.state == 2) && (game->ship.x >= game->ai.target || game->ship.x_vel < 0))
        {
            game->ai.state = 3;
        }
        if (game->ai.state == 1)
        {
            *right = 1;
        }
        if (game->ai.state == 2)
        {
            *left = 1;
        }
        /*printf( "game->ship.x2: %f %d %d %f\n", game->ship.x, game->ai.state, ( game->ship.x >= game->ai.target ), game->ai.target ); */
    }
    /* --- state 3, bring x velocity close to zero --- */
    if (game->ai.state == 3)
    {
        if (game->ship.x_vel > 0.07 || game->ship.x_vel < -0.07)
        {
            if (game->ship.x_vel > 0)
            {
                *left = 1;
            }
            else
            {
                *right = 1;
            }
        }
        else
        {
            game->ai.state = 4;
            game->ai.vdiff = game->ship.y + ((game->current_level.landing_y[game->ai.pad] - game->ship.y) / 3);
        }
        /*printf( "game->ship.x3: %f %d %d %f\n", game->ship.x, game->ai.state, ( game->ship.x >= game->ai.target ), game->ai.target ); */
    }
    /* --- state 4, close in on the landing pad --- */
    if (game->ai.state == 4)
    {
        if ((game->ship.x < game->ai.target) || (game->ship.x > game->ai.target + 2) || game->ship.x_vel != 0.0)
        {
            if (game->ship.x > game->ai.target + 2)
            {
                if (game->ship.x_vel > -0.07)
                {
                    *left = 1;
                }
                /*printf( "C1: x_vel=%f vel=%d left=%d right=%d\n", game->ship.x_vel, ( game->ship.x_vel > -0.07 ), *left, *right ); */
            }
            if (game->ship.x < game->ai.target)
            {
                if (game->ship.x_vel < 0.07)
                {
                    *right = 1;
                }
                /*printf( "C2: x_vel=%f vel=%d left=%d right=%d\n", game->ship.x_vel, ( game->ship.x_vel < 0.07 ), *left, *right ); */
            }
            /*printf( "C3: x_vel=%f vel=%d left=%d right=%d\n", game->ship.x_vel, ( game->ship.x_vel < 0.07 ), *left, *right ); */
        }
        if (game->ship.y < (game->ai.vdiff + game->ship.y_vel))
        {
            *down = 0;
        }
        /*printf( "4: game->ship.x=%f state=%d target=%f x_vel=%f y_vel=%f vdiff=%f\n", game->ship.x, game->ai.state, game->ai.target, game->ship.x_vel, game->ship.y_vel, game->ai.vdiff ); */
    }
}

/*
 ***************************************************************
 * ---------------------- gameloop ----------------------------
 */
void gameloop(Game *game)
{
    short int collision = 0;
    int down = 0;
    int left = 0;
    int right = 0;
    int count = 0;
    int engine_on = 0;
    int engine_on_past = 0;
    int won = 0;
    int odd_even = 0;
    int x;
    Uint8 *key_table;
    SDL_Event event;
    int pause = 0;
    int pressed = 0;
    int appressed = 0;
    char display_string[100];

    game->ai.state = 0;
    while (1)
    {
        /* zero out "show thrust" vars */

        down = 0;
        left = 0;
        right = 0;
        engine_on = 0;
        won = 0;
        odd_even++;
        /* gravity */
        if (!pause)
        {
            game->ship.y_vel = game->ship.y_vel + game->gravity;
        }
        /* read keyboard */

        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT)
        {
            exit(0);
        }
        key_table = SDL_GetKeyState(NULL);
        if (key_table[SDLK_ESCAPE])
        {
            options(game);
        }
        if (key_table[SDLK_RIGHT])
        {
            right = 1;
        }
        if (key_table[SDLK_LEFT])
        {
            left = 1;
        }
        if (key_table[SDLK_UP])
        {
            down = 1;
        }
        if (key_table[SDLK_a])
        {
            /* toggle */
            if (!appressed)
            {
                game->autopilot = (game->autopilot) ? 0 : 1;
                appressed++;
                if (game->autopilot == 1)
                {
                    game->ai.state = 0;
                }
            }
        }
        else
        {
            if (appressed)
            {
                appressed = 0;
            }
        }
        if (key_table[SDLK_q])
        {
            exit(0);
        }
        if (game->demo_mode && (left || right || down || pause))
        {
            collision = 1;
        }
        /* Toggle pause on/off */
        if (key_table[SDLK_p])
        {
            if (!pressed)
            {
                pause = (pause) ? 0 : 1;
                pressed++;
                if (pause)
                {
                    /* printf( "Pause pressed\n" ); */
                    sprintf(display_string, "**PAUSED**");
                    if (game->small_font > -1)
                    {
                        DT_DrawText(display_string, game->screen, game->small_font, 300, 20);
                    }
                }
            }
        }
        else
        {
            if (pressed)
            {
                /* printf( "Pause released\n" ); */
                pressed = 0;
            }
        }
        if (key_table[SDLK_q])
        {
            exit(0);
        }
        if (!pause)
        {
            if (game->demo_mode || game->autopilot)
            {
                game->gravity = 0.05;
                gameai(&left, &right, &down, game);
            }
            if (game->fuel > 0)
            {
                if (right == 1)
                {
                    game->ship.x_vel = game->ship.x_vel + 0.07;
                    game->fuel = game->fuel - 1;
                    engine_on = 1;
                }
                if (left == 1)
                {
                    game->ship.x_vel = game->ship.x_vel - 0.07;
                    game->fuel = game->fuel - 1;
                    engine_on = 1;
                }
                if (down == 1 && game->fuel > 1)
                {
                    game->ship.y_vel = game->ship.y_vel - 0.10;
                    game->fuel = game->fuel - 2;
                    engine_on = 1;
                }
                else
                {
                    down = 0;
                }
            }
            else
            {
                left = right = down = 0;
            }
#ifndef NOSOUND
            if ((engine_on == 1) && (!Mix_Playing(1)))
            {
                Mix_PlayChannel(1, game->engine, 0);
            }
            if ((engine_on == 0) && (engine_on_past == 1))
            {
                Mix_HaltChannel(1);
            }

#endif

            engine_on_past = engine_on;

            /* update ship position */

            game->ship.x = game->ship.x + game->ship.x_vel;
            game->ship.y = game->ship.y + game->ship.y_vel;

            /* update rocket exhaust position */

            game->thrust.x = (game->ship.x + ((game->ship.w - game->thrust.w) / 2));
            game->thrust.y = (game->ship.y + (game->ship.h - 5));

            game->thrustb.x = (game->ship.x + ((game->ship.w - game->thrust.w) / 2));
            game->thrustb.y = (game->ship.y + (game->ship.h - 5));

            game->thrust_right.x = (game->ship.x - game->thrust_right.w);
            game->thrust_right.y = ((game->ship.y + game->ship.h) - (game->thrust_right.h + 8));

            game->thrust_left.x = (game->ship.x + game->ship.w);
            game->thrust_left.y = ((game->ship.y + game->ship.h) - (game->thrust_left.h + 8));

            /* draw screen */

            draw_sprite(game->screen, game->background);
            remaining_ships(game);
            draw_sprite(game->screen, game->ship);
            draw_sprite(game->screen, game->current_level.sprite);
            if (game->demo_mode)
            {
                sprintf(display_string, "**DEMO MODE - Press Arrow Keys To Exit**");
                DT_DrawText(display_string, game->screen, game->small_font, 200, 20);
            }
            /* Write in the landing pads as red if you're going too fast - if on */
            if (game->opt_lp_warn)
            {
                for (count = 0; count < game->current_level.num_landings;
                        count++)
                {
                    if (game->ship.y_vel > game->current_level.landing_speed[count])
                    {
                        for (x = game->current_level.landing_x[count];
                                x < game->current_level.landing_x[count] + game->current_level.landing_w[count]; x++)
                        {
                            DrawPixel(game->screen, 255, 0, 0, x, game->current_level.landing_y[count]);
                            DrawPixel(game->screen, 255, 0, 0, x, game->current_level.landing_y[count] + 1);
                            DrawPixel(game->screen, 255, 0, 0, x, game->current_level.landing_y[count] + 2);
                        }
                    }
                }
            }
            if (right == 1)
            {
                draw_sprite(game->screen, game->thrust_right);
            }
            if (down == 1)
            {
                if ((odd_even % 2) == 0)
                {
                    draw_sprite(game->screen, game->thrust);
                }
                else
                {
                    draw_sprite(game->screen, game->thrustb);
                }
            }
            if (left == 1)
            {
                draw_sprite(game->screen, game->thrust_left);
            }
            /* display fuel  */

            draw_score(game, 1);

            /* flip screen so they show up */

            SDL_Flip(game->screen);
            /* check if off screen and way low */
            if (game->ship.y > YSIZE)
            {
                explode(game->ship, game->current_level.sprite, game);
                if (!game->ships_remaining)
                {
                    game->landing_pad = count;
                    game->state = GAMEOVER;
                    return;
                }
                else
                {
                    game->landing_pad = count;
                    game->ai.state = 0;
                    game->state = LOST;
                    return;
                }
            }
            /* collision detection */
            if (!collision)
            {
                collision = collision_detect_perfect(game->ship, game->current_level.sprite);
            }
            if (collision > 0)
            {
                collision = 0;
                /* landing detection */
                for (count = 0; count < game->current_level.num_landings;
                        count++)
                {
                    if (((game->ship.x + 2) > game->current_level.landing_x[count]) && (((game->ship.x - 2) +
                            game->ship.w) < (game->current_level.landing_x[count] +
                                             game->current_level.landing_w[count])) && (game->ship.y_vel <
                                                     game->current_level.landing_speed[count]) && (game->ship.x_vel < 0.5) && (game->ship.x_vel >
                                                             -0.5))
                    {
                        /* we landed! */
                        won = 1;
                        if (game->demo_mode)
                        {
                            game->landing_pad = count;
                            win(game, 0);
                            game->state = GAMEOVER;
                            return;
                        }
                        /* Bonus ship every 10000 points */
                        if ((game->score / 10000) < ((game->score + game->current_level.landing_score[count]) /
                                                     10000))
                        {
                            /* (MLH) This would be a good place to play a sound */

                            game->landing_pad = count;
                            game->state = BONUS_SHIP;
                            return;
                        }
                        game->landing_pad = count;
                        game->state = WON;
                        return;
                    }
                }
                if (won == 0)
                {
                    /* didn't land */

                    explode(game->ship, game->current_level.sprite, game);
                    if (game->demo_mode)
                    {
                        game->landing_pad = count;
                        game->state = GAMEOVER;
                        return;
                    }
                    if (!game->ships_remaining)
                    {
                        game->landing_pad = count;
                        game->state = GAMEOVER;
                        return;
                    }
                    else
                    {
                        game->landing_pad = count;
                        game->ai.state = 0;
                        game->state = LOST;
                        return;
                    }
                }
            }
        }
        else
        {
            SDL_Flip(game->screen);
        }
        /* timer - so that things run at an even speed regardless of cpu speed */
        frame_rate_limiter(game);
    }
}

/*
 ***********************************************
 *******************   main   ******************
 ***********************************************
 */
int main(int argc, char **argv)
{
    char name[100];
    int count;
    char filename[100];
    Game game;

#ifndef NOSOUND
    printf("Sound active\n");
#endif

#ifdef NOSOUND
    printf("Sound disabled\n");
#endif

    game.demo_mode = 0;
    game.autopilot = 0;

    load_game(&game);

    game.score = 0;
    game.difficulty = 0;
    game.ships_remaining = (game.opt_num_lives - 1);
    game.gravity = 0.05;

    game.current_level.sprite.image = NULL;
    game.current_level.sprite.x = -1;
    game.background.image = NULL;
    game.background.x = -1;
    game.back_no = 0;

    /* initialize SDL */
    sprintf(filename, "%simages/win_icon.bmp", DATAPATH);
    game.screen = init_sdl(XSIZE, YSIZE, "Moon Lander", filename);

    printf("screen initialized\n");

    /* initialize sprites */

    sprintf(filename, "%simages/miniship2.bmp", DATAPATH);
    new_sprite(&(game.miniship), filename, 0, 0, 1, 0);

    sprintf(filename, "%simages/newship.png", DATAPATH);
    new_sprite(&(game.ship), filename, XSIZE / 2, 1, 1, 0);

    sprintf(filename, "%simages/thrust1.png", DATAPATH);
    new_sprite(&(game.thrust), filename, 0, 0, 1, 0);

    sprintf(filename, "%simages/thrust2.png", DATAPATH);
    new_sprite(&(game.thrustb), filename, 0, 0, 1, 0);

    sprintf(filename, "%simages/thrust_left.bmp", DATAPATH);
    new_sprite(&(game.thrust_left), filename, 0, 0, 1, 0);

    sprintf(filename, "%simages/thrust_right.bmp", DATAPATH);
    new_sprite(&(game.thrust_right), filename, 0, 0, 1, 0);

    sprintf(filename, "%simages/logo.png", DATAPATH);
    new_sprite(&(game.logo), filename, 130, 25, 1, 0);

    sprintf(filename, "%simages/backgrounds/red_plain.jpg", DATAPATH);
    new_sprite(&(game.gameover_screen), filename, 0, 0, 0, 0);

    sprintf(filename, "%simages/magigames_steel.gif", DATAPATH);
    new_sprite(&(game.magigames), filename, 194, 410, 0, 0);

#ifndef NOSOUND
    sprintf(filename, "%ssounds/Blaster_1.wav", DATAPATH);
    game.on = new_audio(filename);
    Mix_VolumeChunk(game.on, 20);

    sprintf(filename, "%ssounds/Retro_3.wav", DATAPATH);
    game.off = new_audio(filename);
    Mix_VolumeChunk(game.off, 20);

    sprintf(filename, "%ssounds/jet_lp.wav", DATAPATH);
    game.engine = new_audio(filename);
    Mix_VolumeChunk(game.engine, 40);

    sprintf(filename, "%ssounds/Space_Bubbles_2.wav", DATAPATH);
    game.new_life = new_audio(filename);

    sprintf(filename, "%ssounds/beep1b.wav", DATAPATH);
    game.ready = new_audio(filename);
    Mix_VolumeChunk(game.ready, 50);

    sprintf(filename, "%ssounds/honk.wav", DATAPATH);
    game.go = new_audio(filename);
    Mix_VolumeChunk(game.go, 100);

    sprintf(filename, "%ssounds/explosion2.wav", DATAPATH);
    game.explosion_a = new_audio(filename);

    sprintf(filename, "%ssounds/eagle_has_landed.wav", DATAPATH);
    game.eagle_landed = new_audio(filename);
    Mix_VolumeChunk(game.eagle_landed, 20);
#endif

    /* intitalize timer */

    game.ActualTime = SDL_GetTicks();
    game.LastTime = game.ActualTime;

    /* load fonts */

    sprintf(filename, "%sfonts/ConsoleFont.bmp", DATAPATH);
    game.small_font = DT_LoadFont(filename, TRANS_FONT);

    sprintf(filename, "%sfonts/LargeFont.bmp", DATAPATH);
    game.big_font = DT_LoadFont(filename, TRANS_FONT);
    /*
     * printf("getting random level for first time\n");
     * random_level(&game);
     * load explosion sprites here, because it takes a second or two
     */
    for (count = 1; count < 26; count++)
    {
        sprintf(name, "%simages/kablam/exp%02d.png", DATAPATH, count);
        new_sprite(&(game.explosion[count - 1]), name, 0, 0, 1, 200);
    }
    game.state = FRESHRUN;
    /* everything is now loaded up */
    while (1)
    {
#ifndef NOSOUND
        Mix_HaltChannel(1);
#endif
        switch (game.state)
        {
        case FRESHRUN:
        {
            game_over(&game, 1);
            break;
        }

        case GAMEOVER:
        {
            game_over(&game, 0);
            break;
        }

        case LOST:
        {
            new_round(&game, 0);
            gameloop(&game);
            break;
        }

        case WON:
        {
            win(&game, 0);
            new_round(&game, 1);
            gameloop(&game);
            break;
        }

        case BONUS_SHIP:
        {
            win(&game, 1);
            new_round(&game, 1);
            gameloop(&game);
            break;
        }

        case NEWGAME:
        {
            game.demo_mode = 0;
            new_round(&game, 3);
            gameloop(&game);
            break;
        }

        case AIGAME:
        {
            game.demo_mode = 1;
            new_round(&game, 3);
            gameloop(&game);
            break;
        }
        }                               /* end switch */
    }
    return 0;
}
