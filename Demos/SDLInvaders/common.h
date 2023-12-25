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
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "SFont.h"

/* #define TEST */ /* activate test behaviour for developers */

/* Magic numbers */
#define VERSION				"0.7.6"
#define X_RESOLUTION			800
#define Y_RESOLUTION			600
#define NUM_BLOCKADES			6
#define NUM_INVADER_ROWS		4
#define NUM_INVADERS_PER_ROW		8
#define NUM_INVADERS			(NUM_INVADER_ROWS * NUM_INVADERS_PER_ROW)
#define INVADER_SIZE			45
#define INVADER_DISTANCE		22
#define MAX_SHOTS			20
#define INIT_MAX_SHOTS			8
#define MAX_SHOTS2			10
#define MAX_SPECIAL_SHOTS		40
#define MAX_FIGHTER_SPECIAL_SHOTS	MAX_SPECIAL_SHOTS - 5
#define BLOCKADE_HITPOINTS		4
#define STEPSIZE			(INVADER_SIZE / 6)
#define MAX_GALIENS			(NUM_INVADERS + 5)
#define HIGHSCORES			7
#define HIGHSCORE_SLEN			20

extern int max_shots;

/* Structure for the fighter */
typedef struct fighter_s
{
    int x, y;                           /* Screen position */
    int health;                         /* Life energy */
    int have_weapon;                    /* Kind of weapon we have */
    int active_weapon;                  /* which one is shooting */
    struct specialWeapon_s *bombs[MAX_FIGHTER_SPECIAL_SHOTS];
    int mayshoot;                       /* enable canons */
} fighter_t, *fighter_p;

/* Structure for a general alien */
typedef struct galien_s
{
    int alien_type;                     /* Is it a normal alien or one on the top? */
    int x, y;                           /* Screen position */
    int health;                         /* Health points left */
    int frame;                          /* Which frame to draw? */
    SDL_Surface *Surface;               /* Which picture to use? */
} galien_t, *galien_p;
typedef enum direction_t { UP, RIGHT, LEFT } direction;

/* Structure for the shots */
typedef struct shot_s
{
    int x, y;                           /* Screen position */
    int active;                         /* Does the shot exist? */
    direction direction;                /* were this shot goes */
} shot_t, *shot_p;

/* Structure for the alien shots */
typedef struct shot2_s
{
    int x, y;                           /* Screen position */
    int active;                         /* Does the shot exist? */
} shot2_t, *shot2_p;

/* Structure for the blockades */
typedef struct blockade_s
{
    int x, y;                           /* Screen position */
    int health;                         /* How many hits will it take before it explodes? */
} blockade_t, *blockade_p;

/* Structure for the miracles */
typedef struct miracle_s
{
    /* function that does the settings. returns 0 on success */
    int (*mount)(struct miracle_s *this, fighter_p fighter, int *shot_slots);

    char *info;                         /* Informative text, which miracle happened? */
} miracle_t, *miracle_p;

/* Structure for the Highscore list */
typedef struct highscore_s
{
    char name[HIGHSCORE_SLEN];          /* The player name */
    int points;                         /* The points of this player */
    int wave;                           /* The wavenumber to which he got */
    int rank;                           /* The rank of the player */
} highscore_t, *highscore_p;
typedef struct specialWeapon_s
{
    /* moves the object by one step. return 0 if still active.
     * This method is supposed to also check for collisions and take
     * appropriate action. */
    int (*move)(struct specialWeapon_s *this, fighter_p fighter, struct shot_s *shots, struct shot2_s *ashots);

    /* initialize this weapon (return 0 means success) */
    int (*launch)(struct specialWeapon_s *this, fighter_p fighter);

    int x, y;                           /* current position */
    int active;

    enum { BOMB, ROCKET, SEEKROCKET, MAX_SPECIAL_WEAPON_TYPE } type;

    galien_p target;
} specialWeapon_t, *specialWeapon_p;

fighter_t the_fighter;
shot_t shots[MAX_SHOTS];
shot2_t shots2[MAX_SHOTS2];
blockade_t blockades[NUM_BLOCKADES];
galien_t galiens[MAX_GALIENS];
extern miracle_t miracles[];

/* special shots that are on their way to target */
extern specialWeapon_p special_shots[MAX_SPECIAL_SHOTS];
highscore_t highscore[HIGHSCORES];
SFont_FontInfo darkfont;
