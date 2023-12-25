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
#include "common.h"

/* The images */
SDL_Surface *screen;
SDL_Surface *background;
SDL_Surface *fighter;
SDL_Surface *blockade;
SDL_Surface *invader;
SDL_Surface *invader2;
SDL_Surface *alien_top[6];
SDL_Surface *shot;
SDL_Surface *shot2;
SDL_Surface *bomb;
SDL_Surface *rocket;
SDL_Surface *seekrocket;
SDL_Surface *explosion;
SDL_Surface *my_font;
SDL_Surface *my_font2;
SDL_Surface *start_picture;
SDL_Surface *help_picture;
int game_points = 0;
int wavenumber = 1;

/* used when a wave is won */
char *good_word[] =
{
    "Now it's getting interesting", "May the source be with you", "I'll call you CLEANER", "Haven't we met before...?",
    "You are 1337!", ""
};

/* used when a wave is lost */
char *bad_word[] =
{
    "We really tried to make it easy", "Too bad sunnyboy", "Sorry, we don't have training levels here",
    "Hey man, you lost your planet!", "You better look for a new job", "They blew you up, man!",
    "Life ain't easy...", "You Suck!", ""
};

/* loads the end of wave screen
 * won: 1 if player won this wave, 0 if he lost
 */
void end_screen(int won)
{
    SDL_Surface *end_picture;
    SDL_Rect src, dest;
    char *points_string;
    int i;

    /* Load the win or lose picture */
    if (won == 1)
    {
        end_picture = IMG_Load("data/siegbild.jpg");
        if (end_picture == NULL)
        {
            printf("Unable to load end_picture bitmap.\n");
            exit(1);
        }
    }
    if (won == 0)
    {
        end_picture = IMG_Load("data/alien.jpg");
        if (end_picture == NULL)
        {
            printf("Unable to load end_picture bitmap.\n");
            exit(1);
        }
    }
    /* display it */
    src.x = 0;
    src.y = 0;
    src.w = end_picture->w;
    src.h = end_picture->h;
    dest.x = 0;
    dest.y = 0;
    dest.w = screen->w;
    dest.h = screen->h;
    SDL_BlitSurface(end_picture, &src, screen, &dest);

    points_string = malloc(20);
    sprintf(points_string, "Points: %d", game_points);
    /* Display some informative text */
    if (won == 0)
    {
        for (i = 0; *bad_word[i]; i++)
            ;
        i = rand() % i;
        XCenteredString2(screen, &darkfont, 100, bad_word[i]);
        XCenteredString2(screen, &darkfont, 200, points_string);
    }
    if (won == 1)
    {
        for (i = 0; *good_word[i]; i++)
            ;
        i = rand() % i;
        XCenteredString2(screen, &darkfont, 100, good_word[i]);
        XCenteredString2(screen, &darkfont, 200, points_string);
    }
    /* Update the entire screen. */
    SDL_UpdateRect(screen, 0, 0, 0, 0);

    SDL_Delay(3000);
}

/* Show the help screen */
void help_screen()
{
    SDL_Rect src, dest;
    SDL_Event event;
    SDL_keysym keysym;

    /* clean input queue */
    while (event.type == SDL_KEYDOWN)
    {
        SDL_PollEvent(&event);
    }
    /* display it */
    src.x = 0;
    src.y = 0;
    src.w = help_picture->w;
    src.h = help_picture->h;
    dest.x = 0;
    dest.y = 0;
    dest.w = screen->w;
    dest.h = screen->h;
    SDL_BlitSurface(help_picture, &src, screen, &dest);

    XCenteredString(screen, 50, "SDLInvaders Quickhelp");
    PutString(screen, 50, 140, "Use the arrow keys or the mouse to");
    PutString(screen, 50, 180, "move the ship from left to right.");
    PutString(screen, 50, 230, "Use Space or left mouse button to");
    PutString(screen, 50, 270, "fire the primary weapon");
    PutString(screen, 50, 320, "Use number keys or right mouse button");
    PutString(screen, 50, 360, "to switch the primary weapon");
    PutString(screen, 50, 410, "Use 'b', 's' or 'r' to fire the");
    PutString(screen, 50, 450, "special weapons (if you got any).");
    PutString(screen, 50, 500, "Press 'p' to (un)pause.");
    PutString(screen, 50, 550, "Press 'q' to quit.");

    /* Update the entire screen. */
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    while (1 != 0)
    {
        SDL_WaitEvent(&event);
        if (event.type == SDL_KEYDOWN)
        {
            keysym = event.key.keysym;
            if (keysym.sym == SDLK_q)
            {
                exit(0);
            }
            if (keysym.sym == SDLK_SPACE)
            {
                return;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            return;
        }
    }
}

/* Show the start screen with menu */
int start_screen()
{
    SDL_Rect src, dest;
    SDL_Event event;
    SDL_keysym keysym;

    /* clean input queue */
    while (event.type == SDL_KEYDOWN)
    {
        SDL_PollEvent(&event);
    }
    /* display it */
    src.x = 0;
    src.y = 0;
    src.w = start_picture->w;
    src.h = start_picture->h;
    dest.x = 0;
    dest.y = 0;
    dest.w = screen->w;
    dest.h = screen->h;
    SDL_BlitSurface(start_picture, &src, screen, &dest);

    XCenteredString2(screen, &darkfont, 100, "Welcome to SDLInvaders!");
    XCenteredString2(screen, &darkfont, 250, "Start:            Space");
    XCenteredString2(screen, &darkfont, 300, "Help:               h  ");
    XCenteredString2(screen, &darkfont, 350, "Quit:               q  ");

    /* Update the entire screen. */
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    while (1 != 0)
    {
        SDL_WaitEvent(&event);
        if (event.type == SDL_KEYDOWN)
        {
            keysym = event.key.keysym;
            if (keysym.sym == SDLK_q)
            {
                return 1;
            }
            if (keysym.sym == SDLK_h)
            {
                help_screen();
                SDL_BlitSurface(start_picture, &src, screen, &dest);

                XCenteredString2(screen, &darkfont, 100, "Welcome to SDLInvaders!");
                XCenteredString2(screen, &darkfont, 250, "Start:            Space");
                XCenteredString2(screen, &darkfont, 300, "Help:               h  ");
                XCenteredString2(screen, &darkfont, 350, "Quit:               q  ");

                /* Update the entire screen. */
                SDL_UpdateRect(screen, 0, 0, 0, 0);
            }
            if (keysym.sym == SDLK_SPACE)
            {
                return 0;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            return 0;
        }
    }
}

/* This is used to get the name of the player and put it into the
 * Highscore list at the correct position. */
void enter_name()
{
    char namestring[20] = "", dummy_name[20] = "";
    int i, dummy_points, dummy_wave;

    XCenteredString2(screen, &darkfont, 300, "Enter your name for Highscore list");
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    SFont_Input2(screen, &darkfont, 250, 350, 400, namestring);
    strcpy(highscore[HIGHSCORES - 1].name, namestring);
    highscore[HIGHSCORES - 1].points = game_points;
    highscore[HIGHSCORES - 1].wave = wavenumber;
    for (i = HIGHSCORES - 1; i > 0; i--)
    {
        if (highscore[i].points > highscore[i - 1].points)
        {
            strcpy(dummy_name, highscore[i - 1].name);
            dummy_points = highscore[i - 1].points;
            dummy_wave = highscore[i - 1].wave;

            strcpy(highscore[i - 1].name, highscore[i].name);
            highscore[i - 1].points = highscore[i].points;
            highscore[i - 1].wave = highscore[i].wave;

            strcpy(highscore[i].name, dummy_name);
            highscore[i].points = dummy_points;
            highscore[i].wave = dummy_wave;
        }
    }
}

/*  Displays the highscore list */
void highscore_screen()
{
    SDL_Rect src, dest;
    SDL_Event event;
    SDL_keysym keysym;
    char *text, *text2;
    int i;

    /* clean input queue */
    while (event.type == SDL_KEYDOWN)
    {
        SDL_PollEvent(&event);
    }
    /* display it */
    src.x = 0;
    src.y = 0;
    src.w = start_picture->w;
    src.h = start_picture->h;
    dest.x = 0;
    dest.y = 0;
    dest.w = screen->w;
    dest.h = screen->h;
    SDL_BlitSurface(start_picture, &src, screen, &dest);

    XCenteredString2(screen, &darkfont, 50, "Highscores");
    PutString2(screen, &darkfont, 50, 100, "Name");
    PutString2(screen, &darkfont, 370, 100, "Points");
    PutString2(screen, &darkfont, 520, 100, "Wave");
    PutString2(screen, &darkfont, 640, 100, "Rank");
    text = malloc(20);
    text2 = malloc(80);
    for (i = 0; i < HIGHSCORES; i++)
    {
        sprintf(text, "%s", highscore[i].name);
        PutString2(screen, &darkfont, 50, (150 + (50 * i)), text);
        sprintf(text2, "%6d", highscore[i].points);
        PutString2(screen, &darkfont, 370, (150 + (50 * i)), text2);
        sprintf(text2, "%4d", highscore[i].wave);
        PutString2(screen, &darkfont, 540, (150 + (50 * i)), text2);
    }
    free(text);
    free(text2);

    /* Update the entire screen. */
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    while (1 != 0)
    {
        SDL_WaitEvent(&event);
        if (event.type == SDL_KEYDOWN)
        {
            keysym = event.key.keysym;
            if (keysym.sym == SDLK_q)
            {
                exit(0);
            }
            if (keysym.sym == SDLK_SPACE)
            {
                return;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            return;
        }
    }
}

void draw_blockades()
{
    SDL_Rect src, dest;
    int i;

    for (i = 0; i < NUM_BLOCKADES; i++)
    {
        if (blockades[i].health > 0)
        {
            src.x = 0;
            src.y = 0;
            src.w = blockade->w;
            src.h = blockade->h;
            dest.x = blockades[i].x;
            dest.y = blockades[i].y;
            dest.w = blockade->w;
            dest.h = blockade->h;
            SDL_BlitSurface(blockade, &src, screen, &dest);
        }
    }
}

void info_health()
{
    SDL_Rect dest;
    Uint32 green = SDL_MapRGB(screen->format, 0, 255, 0);
    Uint32 yellow = SDL_MapRGB(screen->format, 255, 255, 0);
    Uint32 red = SDL_MapRGB(screen->format, 255, 0, 0);
    Uint32 black = SDL_MapRGB(screen->format, 0, 0, 0);
    Uint32 blue = SDL_MapRGB(screen->format, 0, 0, 255);
    const int health_max = 100;

    dest.x = 0;
    dest.y = 0;
    dest.h = fighter->h;

    /* paint the fighter */
    dest.w = fighter->w;
    SDL_BlitSurface(fighter, NULL, screen, &dest);
    dest.x += dest.w;

    /* paint the blue rectangle of the health bar */
    dest.x--;
    dest.y = (fighter->h / 4) - 1;
    dest.h = (fighter->h / 2) + 2;
    dest.w = health_max + 2;
    SDL_FillRect(screen, &dest, blue);

    /* paint the black rectangle of the health bar */
    dest.x++;
    dest.y++;
    dest.h -= 2;
    dest.w = health_max;
    SDL_FillRect(screen, &dest, black);

    /* paint the health into the health bar */
    dest.x++;
    dest.y++;
    dest.h -= 2;
    dest.w = the_fighter.health;
    /* green, yellow or red? */
    if (the_fighter.health > 60)
    {
        SDL_FillRect(screen, &dest, green);
    }
    else
    {
        if (the_fighter.health > 30)
        {
            SDL_FillRect(screen, &dest, yellow);
        }
        else
        {
            SDL_FillRect(screen, &dest, red);
        }
    }
}

void info_shots(int shots_passive)
{
    SDL_Rect dest;
    int i;

    dest.x = 0;
    dest.y = fighter->h;
    dest.w = shot->w;
    dest.h = shot->h;
    for (i = 0; i < shots_passive; i++)
    {
        SDL_BlitSurface(shot, NULL, screen, &dest);
        dest.x += (dest.w) * 2;
    }
}

void info_special_weapons(int nrockets, int nbombs, int nseekrockets)
{
    SDL_Surface *p;
    SDL_Rect dest;
    char c, weapon_string[8];
    int n, i;

    dest.x = X_RESOLUTION / 2 - 180;
    for (i = 0; i < 3; i++)
    {
        switch (i)
        {
        case 0:
            p = rocket;
            n = nrockets;
            c = 'R';
            break;
        case 1:
            p = bomb;
            n = nbombs;
            c = 'B';
            break;
        case 2:
            p = seekrocket;
            n = nseekrockets;
            c = 'S';
            break;
        }
        /* dest.y = fighter->h + shot->h; */
        dest.y = 0;
        dest.w = p->w;
        dest.h = p->h;
        SDL_BlitSurface(p, NULL, screen, &dest);
        dest.x += dest.w;
        sprintf(weapon_string, "%c:%d", c, n);
        PutString(screen, dest.x, dest.y, weapon_string);
        dest.x += TextWidth(weapon_string);
    }
}

void draw_background()
{
    SDL_Rect src, dest;
    /* char *health_string; */
    char *points_string;
    char *shot_string;
    int special_weapons[MAX_SPECIAL_WEAPON_TYPE] = { 0, 0, 0 };
    int shots_passive = 0;
    int i;

    src.x = 0;
    src.y = 0;
    src.w = background->w;
    src.h = background->h;
    dest.x = 0;
    dest.y = 0;
    dest.w = screen->w;
    dest.h = screen->h;
    SDL_BlitSurface(background, &src, screen, &dest);

    /* health_string = malloc(12);
     * sprintf(health_string, "Health: %3d", the_fighter.health);
     * PutString(screen, 0, 0, health_string);
     * free(health_string); */

    info_health();

    points_string = malloc(15);
    sprintf(points_string, "Points: %5d", game_points);
    PutString(screen, X_RESOLUTION - TextWidth(points_string), 0, points_string);
    free(points_string);
    for (i = 0; i < MAX_FIGHTER_SPECIAL_SHOTS; i++)
    {
        if (the_fighter.bombs[i] != NULL)
        {
            special_weapons[the_fighter.bombs[i]->type]++;
        }
    }
    shot_string = malloc(40);
    sprintf(shot_string, "R%d B%d S%d Shots: %d", special_weapons[ROCKET], special_weapons[BOMB],
            special_weapons[SEEKROCKET], max_shots);
    /* PutString(screen, X_RESOLUTION / 2 - 180, 0, shot_string); */
    for (i = 0; i < max_shots; i++)
    {
        if (shots[i].active == 0)
        {
            shots_passive++;
        }
    }
    info_shots(shots_passive);
    info_special_weapons(special_weapons[ROCKET], special_weapons[BOMB], special_weapons[SEEKROCKET]);
    free(shot_string);
}

void draw_shots()
{
    SDL_Rect src, dest;
    int i;
    specialWeapon_p s_shot;

    for (i = 0; i < max_shots; i++)
    {
        if (shots[i].active)
        {
            src.x = 0;
            src.y = 0;
            src.w = shot->w;
            src.h = shot->h;
            dest.x = shots[i].x;
            dest.y = shots[i].y;
            dest.w = shot->w;
            dest.h = shot->h;
            SDL_BlitSurface(shot, &src, screen, &dest);
        }
    }
    for (i = 0; i < MAX_SHOTS2; i++)
    {
        if (shots2[i].active)
        {
            src.x = 0;
            src.y = 0;
            src.w = shot2->w;
            src.h = shot2->h;
            dest.x = shots2[i].x;
            dest.y = shots2[i].y;
            dest.w = shot2->w;
            dest.h = shot2->h;
            SDL_BlitSurface(shot2, &src, screen, &dest);
        }
    }
    for (i = 0; i < MAX_SPECIAL_SHOTS; i++)
    {
        if (special_shots[i] && special_shots[i]->active)
        {
            s_shot = special_shots[i];
            src.x = 0;
            src.y = 0;
            switch (s_shot->type)
            {
            case BOMB:
                src.w = bomb->w;
                src.h = bomb->h;
                dest.x = s_shot->x;
                dest.y = s_shot->y;
                dest.w = bomb->w;
                dest.h = bomb->h;
                SDL_BlitSurface(bomb, &src, screen, &dest);
                break;
            case ROCKET:
                src.w = rocket->w;
                src.h = rocket->h;
                dest.x = s_shot->x;
                dest.y = s_shot->y;
                dest.w = rocket->w;
                dest.h = rocket->h;
                SDL_BlitSurface(rocket, &src, screen, &dest);
                break;
            case SEEKROCKET:
                src.w = seekrocket->w;
                src.h = seekrocket->h;
                dest.x = s_shot->x;
                dest.y = s_shot->y;
                dest.w = seekrocket->w;
                dest.h = seekrocket->h;
                SDL_BlitSurface(seekrocket, &src, screen, &dest);
                break;
            default:
                printf("warning: unknown weapon type [1]\n");
                break;
            }
        }
    }
}

void draw_fighter()
{
    SDL_Rect src, dest;

    src.x = 0;
    src.y = 0;
    src.w = fighter->w;
    src.h = fighter->h;
    dest.x = the_fighter.x;
    dest.y = the_fighter.y;
    dest.w = INVADER_SIZE;
    dest.h = INVADER_SIZE;
    SDL_BlitSurface(fighter, &src, screen, &dest);
}

void draw_explosion(int x, int y)
{
    SDL_Rect src, dest;

    src.x = 0;
    src.y = 0;
    src.w = explosion->w;
    src.h = explosion->h;
    dest.x = x;
    dest.y = y;
    dest.w = explosion->w;
    dest.h = explosion->h;

    SDL_BlitSurface(explosion, &src, screen, &dest);

    SDL_UpdateRect(screen, dest.x, dest.y, dest.w, dest.h);
}

int draw_galien(int id)
{
    SDL_Rect src, dest;

    src.x = 0;
    src.y = 0;
    src.w = INVADER_SIZE;
    src.h = INVADER_SIZE;
    dest.x = galiens[id].x;
    dest.y = galiens[id].y;
    dest.w = INVADER_SIZE;
    dest.h = INVADER_SIZE;
    SDL_BlitSurface(galiens[id].Surface, &src, screen, &dest);

    return 0;
}

/* Checks for all aliens if they need to be drawn
 * and if they do, call draw_galien for it.
 */
void check_draw_galiens()
{
    int i;

    for (i = 0; i < MAX_GALIENS; i++)
    {
        if (galiens[i].health > 0)
        {
            draw_galien(i);
        }
    }
}

void update_graphics()
{
    draw_background();
    draw_blockades();
    draw_fighter();
    check_draw_galiens();
    draw_shots();

    /* Update the entire screen. */
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}

/* This function returns a SDL_Surface which it loads from filename.
 * An alpha value of 0 means there is no alpha channel, instead
 * a colorkey of white is used for transparency.
 * With an alpha value of 1, the alpha channel of the picture is used.
 */
SDL_Surface *load_bitmap(char *filename, int alpha)
{
    SDL_Surface *temp_surf, *surface;
    Uint32 colorkey;

    /* load the bitmap */
    temp_surf = IMG_Load(filename);
    if (temp_surf == NULL)
    {
        printf("Unable to load %s bitmap.\n", filename);
        return NULL;
    }
    /* If there is no alpha channel, set the colorkey to white,
     * if there is an alpha channel, use it. */
    if (alpha == 0)
    {
        surface = SDL_DisplayFormat(temp_surf);
        SDL_FreeSurface(temp_surf);
        colorkey = SDL_MapRGB(surface->format, 255, 255, 255);
        SDL_SetColorKey(surface, SDL_SRCCOLORKEY, colorkey);
        return surface;
    }
    else
    {
        SDL_SetAlpha(temp_surf, SDL_SRCALPHA, 0);
        return temp_surf;
    }
    return NULL;
}

int load_images()
{
    int i;
    char file[100];

    /* load background */
    background = load_bitmap("data/background.jpg", 0);
    if (background == NULL)
    {
        return 1;
    }
    /* load start picture */
    start_picture = load_bitmap("data/startbild.jpg", 0);
    if (start_picture == NULL)
    {
        return 1;
    }
    /* load help background picture */
    help_picture = load_bitmap("data/night-city.jpg", 0);
    if (help_picture == NULL)
    {
        return 1;
    }
    /* load blockade */
    blockade = load_bitmap("data/blockade.png", 0);
    if (blockade == NULL)
    {
        return 1;
    }
    /* load fighter */
    fighter = load_bitmap("data/fighter.png", 1);
    if (fighter == NULL)
    {
        return 1;
    }
    /* load invader */
    invader = load_bitmap("data/invader.png", 1);
    if (invader == NULL)
    {
        return 1;
    }
    /* load invader2 */
    invader2 = load_bitmap("data/invader2.png", 1);
    if (invader2 == NULL)
    {
        return 1;
    }
    /* load alien on top (6 frames) */
    for (i = 0; i < 6; i++)
    {
        sprintf(file, "data/alien_top%d.png", i);
        alien_top[i] = load_bitmap(file, 1);
        if (alien_top[i] == NULL)
        {
            return 1;
        }
    }
    /* load player shot */
    shot = load_bitmap("data/shot.png", 1);
    if (shot == NULL)
    {
        return 1;
    }
    /* load alien shot */
    shot2 = load_bitmap("data/shot2.png", 1);
    if (shot2 == NULL)
    {
        return 1;
    }
    /* load bomb */
    bomb = load_bitmap("data/bomb.png", 1);
    if (bomb == NULL)
    {
        return 1;
    }
    /* load rocket */
    rocket = load_bitmap("data/rocket.png", 1);
    if (rocket == NULL)
    {
        return 1;
    }
    /* load seekrocket */
    seekrocket = load_bitmap("data/seekrocket.png", 1);
    if (seekrocket == NULL)
    {
        return 1;
    }
    /* load explosion */
    explosion = load_bitmap("data/explosion1.png", 1);
    if (explosion == NULL)
    {
        return 1;
    }
    /* load font 1 */
    my_font = load_bitmap("data/NeonFont.png", 1);
    if (my_font == NULL)
    {
        return 1;
    }
    /* load font 2 */
    my_font2 = load_bitmap("data/StoneFont.png", 1);
    if (my_font2 == NULL)
    {
        return 1;
    }
    return 0;
}
