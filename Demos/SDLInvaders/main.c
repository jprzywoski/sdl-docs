/*  SDLInvaders: a clone of the famous Space Invaders game
 *  Copyright (C) 2002 Niels Weber
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <time.h>
#include <errno.h>

#include "graphics.h"
#include "alien.h"
#include "weapons.h"
#include "music.h"

#define TICKS 30

/* The images */
extern SDL_Surface *screen;
extern SDL_Surface *background;
extern SDL_Surface *fighter;
extern SDL_Surface *blockade;
extern SDL_Surface *invader;
extern SDL_Surface *alien_top[];
extern SDL_Surface *shot;
extern SDL_Surface *shot2;
extern SDL_Surface *my_font;
extern SDL_Surface *my_font2;
extern SDL_Surface *explosion;

#ifdef NOSOUND

int sound = 0;

#else

int sound = 1;

#endif

int fullscreen = 0;
extern int wavenumber;
int max_shots = INIT_MAX_SHOTS;
extern int game_points;
specialWeapon_p special_shots[MAX_SPECIAL_SHOTS];
Uint32 next_time;
int extrahard = 1;
int speed_bonus = 0;
int bonus_aliens = NUM_INVADERS / 2;
int aliens_alive = NUM_INVADERS;
miracle_t miracles[] =
{
    { miracle_repair, "Ship health" }, { miracle_hotshot, "Hotshot" }, { miracle_nextWeapon, "New weapon" },
    { miracle_specialWeapon, "Special weapon" }, { NULL }
};

Uint32 time_left()
{
    Uint32 now;

    now = SDL_GetTicks();
    if (next_time <= now)
    {
        return 0;
    }
    else
    {
        return next_time - now;
    }
}

void update_timer()
{
    next_time = SDL_GetTicks() + TICKS;
    SDL_Delay(time_left());
    next_time += TICKS;
}

/* The fighter is initialised (beginning of a new game) */
void init_fighter()
{
    int i;

    the_fighter.x = (X_RESOLUTION / 2) - 15;
    the_fighter.y = Y_RESOLUTION - INVADER_SIZE;
    the_fighter.health = 100;
    the_fighter.have_weapon = 0;
    the_fighter.active_weapon = 0;
#ifdef TEST
    for (i = 0; i < MAX_FIGHTER_SPECIAL_SHOTS; i++)
    {
        miracle_specialWeapon(NULL, &the_fighter, &max_shots);
    }

#else
    for (i = 0; i < MAX_FIGHTER_SPECIAL_SHOTS; i++)
    {
        the_fighter.bombs[i] = NULL;
    }

#endif
}

/* All shots are set inactive (beginning of a new wave)*/
void init_shots()
{
    int i;

    for (i = 0; i < max_shots; i++)
    {
        shots[i].active = 0;
        shots[i].direction = UP;
    }
    for (i = 0; i < MAX_SHOTS2; i++)
    {
        shots2[i].active = 0;
    }
    for (i = 0; i < MAX_SPECIAL_SHOTS; i++)
    {
        special_shots[i] = NULL;
    }
}

/* The blockades are positioned and set to their maximum health
 * (beginning of a new wave) */
void init_blockades()
{
    int i;

    for (i = 0; i < NUM_BLOCKADES; i++)
    {
        blockades[i].x = ((((X_RESOLUTION / NUM_BLOCKADES) / 2) - 15) + (i * (X_RESOLUTION / NUM_BLOCKADES)));
        blockades[i].y = (Y_RESOLUTION - ((INVADER_SIZE * 2) + 10));
        blockades[i].health = BLOCKADE_HITPOINTS;
    }
}

/* direction == 1 means: move fighter left,
 * direction != 1 means: move fighter right.
 * The fighter shouldn't move outside the screen. */
void move_fighter(int direction)
{
    if (direction == 1)
    {
        if (the_fighter.x > 10)
        {
            the_fighter.x -= 10;
        }
    }
    else
    {
        if (the_fighter.x < X_RESOLUTION - 40)
        {
            the_fighter.x += 10;
        }
    }
}

/* The shots of the fighter are fired */
void fire_shot()
{
    int i, r;

    if (!the_fighter.mayshoot)
    {
        return;
    }
    if (the_fighter.active_weapon == 1)
    {
        /* get nice left/right distribution in case
         * of a single free slot */
        r = rand() % 2;
    }
    /* The first free slot for a shot is used. */
    for (i = 0; i < max_shots; i++)
    {
        if (!shots[i].active)
        {
            shots[i].active = 1;
            shots[i].x = the_fighter.x + 4;
            shots[i].y = Y_RESOLUTION - INVADER_SIZE;
            if (the_fighter.active_weapon == 0)
            {
                shots[i].direction = UP;
            }
            else if (the_fighter.active_weapon == 1)
            {
                shots[i].direction = r ? RIGHT : LEFT;
            }
            break;
        }
    }
    for (i = 0; i < max_shots; i++)
    {
        if (!shots[i].active)
        {
            shots[i].active = 1;
            shots[i].x = the_fighter.x + 24;
            shots[i].y = Y_RESOLUTION - INVADER_SIZE;
            if (the_fighter.active_weapon == 0)
            {
                shots[i].direction = UP;
            }
            else if (the_fighter.active_weapon == 1)
            {
                shots[i].direction = r ? LEFT : RIGHT;
            }
            break;
        }
    }
}

/* With a chance depending on wavenumber, each alien fires a shot */
void fire_alien_shots()
{
    int i, j, k;

    for (i = 0; i < NUM_INVADERS; i++)
    {
        /* Attention: more than 60 waves are a danger! */
        j = rand() % (61 - wavenumber);
        if (!j)
        {
            if (galiens[i].health > 0)
            {
                if (galiens[i].x > (the_fighter.x - X_RESOLUTION / 10) && galiens[i].x < (the_fighter.x + X_RESOLUTION /
                        10))
                {
                    for (k = 0; k < MAX_SHOTS2; k++)
                    {
                        if (!shots2[k].active)
                        {
                            shots2[k].active = 1;
#ifndef NOSOUND
                            if (sound)
                            {
                                play_sample(smp_invader_sound1);
                            }

#endif
                            shots2[k].x = galiens[i].x + ((INVADER_SIZE - 20) / 2);
                            shots2[k].y = galiens[i].y + INVADER_SIZE;
                            break;
                        }
                    }
                }
            }
        }
    }
}

/* The shots are moved if they are active.
 * Shots leaving the screen are deactivated. */
void move_shots()
{
    int i;

    for (i = 0; i < max_shots; i++)
    {
        if (shots[i].active)
        {
            shots[i].y -= STEPSIZE * 1.5;
            if (shots[i].y < 0)
            {
                shots[i].active = 0;
            }
            switch (shots[i].direction)
            {
            case UP:
                break;
            case RIGHT:
                shots[i].x += 10;
                if (shots[i].x > X_RESOLUTION)
                {
                    shots[i].active = 0;
                }
                break;
            case LEFT:
                shots[i].x -= 10;
                if (shots[i].x < 0)
                {
                    shots[i].active = 0;
                }
                break;
            }
        }
    }
    for (i = 0; i < MAX_SHOTS2; i++)
    {
        if (shots2[i].active)
        {
            shots2[i].y += 10;
            if (shots2[i].y > Y_RESOLUTION)
            {
                shots2[i].active = 0;
            }
        }
    }
    for (i = 0; i < MAX_SPECIAL_SHOTS; i++)
    {
        if (special_shots[i] != NULL)
        {
            if (special_shots[i]->active)
            {
                special_shots[i]->move(special_shots[i], &the_fighter, shots, shots2);
            }
            if (!special_shots[i]->active)
            {
                free(special_shots[i]);
                special_shots[i] = NULL;
            }
        }
    }
}

/* Has a shot hit something? */
void detect_hit()
{
    int c, i, j, r, n;

    for (i = 0; i < max_shots; i++)
    {
        if (shots[i].active)
        {
            /* Has an galiens shot been hit */
            for (j = 0; j < MAX_SHOTS2; j++)
            {
                if (shots2[j].active && (shots[i].x > shots2[j].x) && (shots[i].x < shots2[j].x + 10) && (shots[i].y >
                        shots2[j].y) && (shots[i].y < shots2[j].y + 30))
                {
                    shots2[j].active = 0;
                    shots2[j].y = 0;
                    shots[i].active = 0;
                    shots[i].y = 0;
                    game_points += (2 + (wavenumber / 10));
                }
            }
            /* Has a galien been hit? */
            for (j = 0; j < MAX_GALIENS; j++)
            {
                if (galiens[j].health > 0)
                {
                    if ((shots[i].x > galiens[j].x) && (shots[i].x < galiens[j].x + INVADER_SIZE) && (shots[i].y >
                            galiens[j].y) && (shots[i].y < galiens[j].y + INVADER_SIZE))
                    {
                        /* Hit! */
#ifndef NOSOUND
                        if (sound)
                        {
                            play_sample(smp_explosion1);
                        }

#endif
                        shots[i].active = 0;
                        shots[i].y = 0;
                        galiens[j].health -= 10;
                        draw_explosion(galiens[j].x, galiens[j].y);
                        if (galiens[j].alien_type == 0)
                        {
                            game_points += (9 + wavenumber);
                            if (galiens[j].health <= 0)
                            {
                                aliens_alive--;
                                /*		printf ("aliens_alive: %d\n", aliens_alive);*/
                                if (aliens_alive < bonus_aliens)
                                {
                                    speed_bonus += 2;
                                    /*		  printf ("speed_bonus: %d\n",speed_bonus);*/
                                    bonus_aliens /= 2;
                                    /*		  printf ("bonus_aliens: %d\n",bonus_aliens);*/
                                }
                            }
                        }
                        if (galiens[j].alien_type == 1)
                        {
                            game_points += (39 + wavenumber);
                            /* all we need, is a miracle... */
                            for (n = 0; miracles[n].mount; n++)
                                ;
                            r = rand() % n;
                            /* make sure, player gets a miracle */
                            c = n;
                            while (c > 0 && miracles[r].mount(&miracles[r], &the_fighter, &max_shots))
                            {
                                r = (r + 1) % n;        /* choose another miracle */
                                c--;
                            }
                        }
                    }
                }
            }
            /* Has a blockade been hit? */
            for (j = 0; j < NUM_BLOCKADES; j++)
            {
                if (blockades[j].health > 0)
                {
                    if ((shots[i].x > blockades[j].x) && (shots[i].x < blockades[j].x + INVADER_SIZE) && (shots[i].y >
                            blockades[j].y) && (shots[i].y < blockades[j].y + INVADER_SIZE))
                    {
                        /* Hit! */
                        shots[i].active = 0;
                        shots[i].y = 0;
                        blockades[j].health -= 1;
                    }
                }
            }
        }
    }
    for (i = 0; i < MAX_SHOTS2; i++)
    {
        if (shots2[i].active)
        {
            /* Has a blockade been hit? */
            for (j = 0; j < NUM_BLOCKADES; j++)
            {
                if (blockades[j].health > 0)
                {
                    if ((shots2[i].x + 10 > blockades[j].x) && (shots2[i].x + 10 < blockades[j].x + INVADER_SIZE) &&
                            (shots2[i].y + 30 > blockades[j].y) && (shots2[i].y + 30 < blockades[j].y + INVADER_SIZE))
                    {
                        /* Hit! */
                        shots2[i].active = 0;
                        shots2[i].y = 0;
                        blockades[j].health -= 1;
                    }
                }
            }
            /* Has the fighter been hit? */
            if ((shots2[i].x + 10 > the_fighter.x) && (shots2[i].x + 10 < the_fighter.x + INVADER_SIZE) &&
                    (shots2[i].y + 30 > the_fighter.y) && (shots2[i].y + 30 < the_fighter.y + INVADER_SIZE))
            {
                /* Hit! */
                shots2[i].active = 0;
                shots2[i].y = 0;
                the_fighter.health -= 10;
                if (the_fighter.health < 0)
                {
                    the_fighter.health = 0;
                }
            }
        }
    }
}

/* Is the wave finished for any reason? */
int detect_end()
{
    int i, count = 0;

    /* Count how many invaders are still alive
     * and if they reached the lower border of the screen.
     */
    for (i = 0; i < NUM_INVADERS + 1; i++)
    {
        if (galiens[i].health > 0)
        {
            if (galiens[i].y > (Y_RESOLUTION - (INVADER_SIZE * 2)))
            {
                end_screen(0);
                return 1;
            }
            count++;
        }
    }
    /* if there no more alien left, we inhibit the canons */
    if (count == 0)
    {
        the_fighter.mayshoot = 0;
    }
    /*
     * count also active shots
     * player's shots
     */
    for (i = 0; i < max_shots; i++)
        if (shots[i].active)
        {
            count++;
        }
    /* aliens shots */
    for (i = 0; i < MAX_SHOTS2; i++)
    {
        if (shots2[i].active)
        {
            count++;
        }
    }
    /* special shots */
    for (i = 0; i < MAX_SPECIAL_SHOTS; i++)
    {
        if (special_shots[i] && special_shots[i]->active)
        {
            count++;
        }
    }
    /* No invaders left! */
    if (count == 0)
    {
        SDL_Delay(1000);
        end_screen(1);
        return 2;
    }
    /* The fighter has been destroyed! */
    if (the_fighter.health <= 0)
    {
        SDL_Delay(1000);
        end_screen(0);
        return 1;
    }
    return 0;
}

/* Waits for another press of "p", then returns */
void game_pause()
{
    SDL_Event event;
    SDL_keysym keysym;

    /* clean input queue */
    while (event.type == SDL_KEYDOWN)
    {
        SDL_PollEvent(&event);
    }
    XCenteredString(screen, (Y_RESOLUTION / 2) - 24, "Unpause with p");
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    while (1 != 0)
    {
        SDL_PollEvent(&event);
        while (event.type != SDL_KEYDOWN)
        {
            SDL_PollEvent(&event);
        }
        keysym = event.key.keysym;
        if (keysym.sym == SDLK_p)
        {
            return;
        }
    }
}

/* Should be called at the start of each new wave */
void wave_start()
{
    char *text;

    init_invaders();
    init_blockades();
    init_shots();
    the_fighter.x = (X_RESOLUTION - INVADER_SIZE) / 2;
    the_fighter.mayshoot = 1;
    init_galien(1, 0, 0, 0, 5, (NUM_INVADERS + 1), alien_top[5]);

    /* don't make it too easy */
    max_shots -= 2;
    if (max_shots < INIT_MAX_SHOTS)
    {
        max_shots = INIT_MAX_SHOTS;
    }
    speed_bonus = 0;
    aliens_alive = NUM_INVADERS;
    bonus_aliens = NUM_INVADERS / 2;
    if (wavenumber > 1)
    {
        text = malloc(25);
        sprintf(text, "Prepare for wave %d", wavenumber);
        XCenteredString2(screen, &darkfont, (Y_RESOLUTION / 2) - 24, text);
        free(text);
        SDL_UpdateRect(screen, 0, 0, 0, 0);
    }
    SDL_Delay(2000);
}

/* A new special alien on top appears if there is none
 * and with a chance of 1 percent.
 */
int new_alien()
{
    if (!galiens[(NUM_INVADERS + 1)].health > 0 && the_fighter.mayshoot)
    {
        return !(rand() % 100);
    }
    return 0;
}

void game_loop()
{
    SDL_Event event;
    SDL_keysym keysym;
    int ende = 0;
    unsigned char *keys;
    int i, n;

    /* clean input queue */
    while (SDL_PollEvent(&event))
        ;
    wave_start();
    while (!ende)
    {
        /* Does a new alien on top appear? */
        if (new_alien())
        {
            init_galien(1, X_RESOLUTION, 30, 10, 5, (NUM_INVADERS + 1), alien_top[5]);
        }
        /* The invaders are moved */
        check_move_galiens();

        /* The invaders fire their shots */
        fire_alien_shots();

        /* The shots are moved */
        move_shots();

        /* Was there a hit? */
        detect_hit();

        update_graphics();
        /* New events? Store them. */
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_MOUSEBUTTONDOWN:
                switch (event.button.button)
                {
                case SDL_BUTTON_LEFT:   /* fire shot */
                    fire_shot();
                    break;
                case SDL_BUTTON_RIGHT:  /* change weapon */
                    the_fighter.active_weapon = (the_fighter.active_weapon + 1) % (the_fighter.have_weapon + 1);
                    break;
                case SDL_BUTTON_MIDDLE: /* ignore */
                    break;
                }
                break;
            case SDL_MOUSEMOTION:
                the_fighter.x = event.motion.x;
                if (the_fighter.x > X_RESOLUTION - 40)
                {
                    the_fighter.x = X_RESOLUTION - 40;
                }
                break;
            case SDL_KEYDOWN:
                keysym = event.key.keysym;
                if (keysym.sym == SDLK_q)
                {
                    exit(0);
                }
                else if (keysym.sym == SDLK_p)
                {
                    game_pause();
                }
                else if (keysym.sym == SDLK_h)
                {
                    help_screen();
                }
                else if (keysym.sym == SDLK_b)
                {
                    for (i = 0; i < MAX_FIGHTER_SPECIAL_SHOTS; i++)
                    {
                        if (the_fighter.bombs[i] && the_fighter.bombs[i]->type == BOMB)
                        {
                            the_fighter.bombs[i]->launch(the_fighter.bombs[i], &the_fighter);
                            the_fighter.bombs[i] = NULL;
                            break;
                        }
                    }
                }
                else if (keysym.sym == SDLK_r)
                {
                    for (i = 0; i < MAX_FIGHTER_SPECIAL_SHOTS; i++)
                    {
                        if (the_fighter.bombs[i] && the_fighter.bombs[i]->type == ROCKET)
                        {
                            the_fighter.bombs[i]->launch(the_fighter.bombs[i], &the_fighter);
                            the_fighter.bombs[i] = NULL;
                            break;
                        }
                    }
                }
                else if (keysym.sym == SDLK_s)
                {
                    for (i = 0; i < MAX_FIGHTER_SPECIAL_SHOTS; i++)
                    {
                        if (the_fighter.bombs[i] && the_fighter.bombs[i]->type == SEEKROCKET)
                        {
                            the_fighter.bombs[i]->launch(the_fighter.bombs[i], &the_fighter);
                            the_fighter.bombs[i] = NULL;
                            break;
                        }
                    }
                }
                else if (keysym.sym >= SDLK_1 && keysym.sym <= SDLK_9)
                {
                    n = keysym.sym - SDLK_1;
                    if (the_fighter.have_weapon >= n)
                    {
                        the_fighter.active_weapon = n;
                    }
                }
            }
        }
        /* fetch keyboard status */
        keys = SDL_GetKeyState(&n);
        /* provide actions for tracked events */
        if (keys[SDLK_SPACE])
        {
            fire_shot();
        }
        if (keys[SDLK_LEFT])
        {
            move_fighter(1);
        }
        else if (keys[SDLK_RIGHT])
        {
            move_fighter(2);
        }
        /* game finished? */
        ende = detect_end();
        if (ende == 2)
        {
            wavenumber++;
            ende = 0;
            wave_start();
        }
        update_timer();
    }
    /* clean input queue */
    while (event.type == SDL_KEYDOWN)
    {
        SDL_PollEvent(&event);
    }
}

/* This function is from Penguin Command, Copyright Karl Bartel */
void TextHelp(char *argv[])
{
    puts("The newest version can be obtained at http://www.sdlinvaders.de/\n");
    printf("Usage: %s [options]\n", argv[0]);
    puts("  [-f, --fullscreen]         start in fullscreen mode (default)");
    puts("  [-w, --windowed]           start in windowed mode");
    puts("  [-s, --nosound]            start without sound");
    puts("  [-e, --easy]               play in easy mode - only for wimps");
    puts("  [-h, --help]               this text\n");

    exit(0);
}

/* This function is from Penguin Command, Copyright Karl Bartel
 * Modifications by me */
void ReadCommandLine(char *argv[])
{
    int i;

    for (i = 1; argv[i]; i++)
    {
        if ((strcmp(argv[i], "--fullscreen") == 0) || (strcmp(argv[i], "-f") == 0))
        {
            fullscreen = 1;
        }
        else
        {
            if ((strcmp(argv[i], "--windowed") == 0) || (strcmp(argv[i], "-w") == 0))
            {
                fullscreen = 0;
            }
            else
            {
                if ((strcmp(argv[i], "--nosound") == 0) || (strcmp(argv[i], "-s") == 0))
                {
                    sound = 0;
                }
                else
                {
                    if ((strcmp(argv[i], "--easy") == 0) || (strcmp(argv[i], "-e") == 0))
                    {
                        extrahard = 0;
                    }
                    else
                    {
                        if ((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "-h") == 0))
                        {
                            TextHelp(argv);
                        }
                        else
                        {
                            printf("\nUnknown parameter: \"%s\" \n", argv[i]);
                            TextHelp(argv);
                        }
                    }
                }
            }
        }
    }
}

int init_sdl_stuff()
{
    /* Initialize SDL's video system and check for errors */
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    /* Make sure SDL_Quit gets called when the program exits! */
    atexit(SDL_Quit);

    SDL_ShowCursor(SDL_DISABLE);

    srand(time(NULL));
    /* Attempt to set a hicolor video mode */
    if (fullscreen)
    {
        screen = SDL_SetVideoMode(X_RESOLUTION, Y_RESOLUTION, 16, SDL_FULLSCREEN);
    }
    else
    {
        screen = SDL_SetVideoMode(X_RESOLUTION, Y_RESOLUTION, 16, 0);
    }
    if (screen == NULL)
    {
        printf("Unable to set video mode: %s\n", SDL_GetError());
        return 1;
    }
    return 0;
}

/* Images and fonts are loaded and initialised */
int init_images()
{
    if (load_images())
    {
        return 1;
    }
    InitFont(my_font);

    darkfont.Surface = my_font2;
    InitFont2(&darkfont);

    return 0;
}

int init_game_objects()
{
    /* The game objects are initialised */
    init_fighter();
    init_invaders();
    init_shots();
    init_blockades();
    wavenumber = 1;
    game_points = 0;

    return 0;
}

/* This is to save the Highscore file */
void save_scores()
{
    FILE *file;
    int i;
    char filename[100];

    sprintf(filename, "data/highscores");

    file = fopen(filename, "w");
    if (file == NULL)
    {
        fprintf(stderr, "\nWarning: I could not open the options file for write");
        fprintf(stderr, "\n%s\n" "The error that occured was:\n" "%s\n\n", filename, strerror(errno));
    }
    for (i = 0; i < HIGHSCORES; i++)
    {
        fprintf(file, "%s\t%d\t%d\n", highscore[i].name, highscore[i].points, highscore[i].wave);
    }
    fclose(file);
}

/* Read in the highscores file and display it.
 * I took part of this from Penguin Command, copyright Karl Bartel. */
void show_highscore()
{
    FILE *file;
    int i;
    char filename[100];

    sprintf(filename, "data/highscores");

    file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "\nWarning: I could not open the options file for read");
        fprintf(stderr, "\n%s\n" "The error that occured was:\n" "%s\n\n", filename, strerror(errno));
    }
    /* Read in the highscores from the file */
    for (i = 0; i < HIGHSCORES; i++)
    {
        fscanf(file, "%s\t%d\t%d\n", highscore[i].name, &highscore[i].points, &highscore[i].wave);
        printf("%s\t%d\t%d\n", highscore[i].name, highscore[i].points, highscore[i].wave);
    }
    fclose(file);
    if (game_points > highscore[HIGHSCORES - 1].points)
    {
        enter_name();
        save_scores();
    }
    highscore_screen();
}

int main(int argc, char *argv[])
{
    int quit = 0;

    printf("\nSDLInvaders Version %s\n\n", VERSION);

    /* Command line arguments are parsed */
    ReadCommandLine(argv);
    /* The SDL stuff is initialised */
    if (init_sdl_stuff())
    {
        return 1;
    }
    if (sound)
    {
        music_init();
    }
    SDL_WM_SetCaption("SDL Invaders", "SDL Invaders");
    /* All bitmaps are loaded and initialised */
    if (init_images())
    {
        return 1;
    }
    if (sound)
    {
        play_background_music("data/pennight.mod");
    }
    /* The game itself */
    while (quit != 1)
    {
        if (init_game_objects())
        {
            return 1;
        }
        if (start_screen() == 1)
        {
            quit = 1;
        }
        else
        {
            game_loop();

            show_highscore();
        }
    }
    if (sound)
    {
        music_cleanup();
    }
    return 0;
}
