/* Twig's in space - An SDL demonstration for CS 321.
 * Version 0.9.3
 * Written by Dr. Evan L. Schemm
 *            Lake Superior State University
 *            School of Mathematics and Computer Science
 *
 * Please send questions, comments, suggestions or (gasp!) bugfixes to:
 *        elschemm@furball.schemm.lssu.edu
 *
 * Copyright 2004 by LSSU School of MA/CS
 *
 * Licensed under GPL.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*
 *     Twig's in space is meant to be reminiscent of the '80's video game era.
 * The player controls a spaceship that can rotate left and right as well as
 * thrust forward.  The ship is armed with a front mounted cannon which can
 * blow up enemys.  Objects and the player will bounce off the sides of the
 * screen, and enemys will bounce off each other.  Rather than the traditional
 * 3 (or n) lives found in many games, the player has a shield which protects
 * them from collisions.  When the shield is depleted, the next collision will
 * result in death.  The shield will recharge slightly at the end of each
 * level.
 *
 *     Note:  Whilst this game does have rocks, it is not meant to be an
 * Asteroids clone.  In point of fact, the asteroids were the second enemy
 * image to be added.  (The first was the spinning rectangles)  It may also
 * have features similar to other games of the era (as do others from the
 * same timeframe), but the game and gameplay are unique to my knowledge.
 *
 *     The original purpose of the game was to serve as a cross platform
 * demonstration as well as a working example to illustrate some talking
 * points for CS 321 at Lake Superior State Univerity.  Nominally, this
 * course teaches computer graphics, but this semester we thought we would
 * step beyond the dry theory.  The code is laced with comments to remind me
 * of things to talk about in that class.
 *
 *     As a cross platform game, it has been a resounding success.  The game
 * is known to compile, and run under Windows (via Cygwin), under Linux (a
 * couple of versions), and under Solaris 9.  The game plays either as a
 * window or fullscreen in all cases, including running with fbconsole without
 * X running under Linux.  The game also plays very well on my MAME cabinet.
 *
 *     Twig is a professor at LSSU who is known for morphing peoples heads
 * onto various images.  His head in this game is a form of retribution.
 * Interestingly, the image comes from an image that Twig doctored himself.
 * What goes around comes around.
 */

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Cygwin does not have random */
#ifdef WIN32
#define RAND	rand
#endif
#ifndef WIN32
#define RAND	random
#endif

#define HEIGHT	480
#define WIDTH	500

/* Define the angles for player shots */
double pxs[16] =
{
    0.0, .3836834324, .707106781187, .9238795325, 1.0, .9238795325, .707106781187, .3836834324, 0.0, -.3836834324,
    -.707106781187, -.9238795325, -1.0, -.9238795325, -.707106781187, -.3836834324
};
double pys[16] =
{
    -1.0, -.9238795325, -.707106781187, -.3836834324, 0.0, .3836834324, .707106781187, .9238795325, 1.0, .9238795325,
    .707106781187, .3836834324, 0.0, -.3836834324, -.707106781187, -.9238795325
};

/* It would probably be a better to use an enum here, but... */
int keys[10];                           /* Allow user defineable keys */

#define UPKEY		0
#define LEFTKEY		1
#define RIGHTKEY	2
#define FIREKEY		3
#define PAUSEKEY	4
#define COINKEY		5
#define STARTKEY	6
#define QUITKEY		7
#define FSKEY		8

struct ENEMY
{
    int type;                           /* Type of enemy; */
    SDL_Surface *images;                /* Pictures for rotating enemys */
    int pos;                            /* Which position it is in */
    int speed;                          /* How fast the rotation occures */
    int speedcount;                     /* How close to next rotation */
    int numpos;                         /* Number of images */
    int dir;                            /* Direction of rotation */
    int status;                         /* Alive / Dead / Other */
    int points;                         /* Number of points for killing this enemy */
    double x;                           /* Location of enemy */
    double y;
    double xs;                          /* Speed of enemy +/- ? */
    double ys;
    int w;                              /* Width and height of this enemy */
    int h;
    int shotstatus;
    double shotx;                       /* For enemys that shoot */
    double shoty;
    double shotxs;
    double shotys;
    SDL_Rect shot_rect;
    SDL_Rect shot_mask;
    SDL_Rect mask;
    SDL_Rect rect;                      /* Rectangle for which rotation image to show */
    SDL_Rect explode;
    SDL_Rect explodemask;
};
struct PLAYER
{
    SDL_Surface *images;
    int facing;                         /* Which picture */
    int points;
    double x;
    double y;
    double xs;
    double ys;
    int jets;
    int status;
    int shield;
    int timer;                          /* Used to control blowup image and Game Over */
    SDL_Rect mask;
    SDL_Rect rect;
    SDL_Rect explode;
    SDL_Rect explodemask;
};
struct SHOT
{
    SDL_Surface *images;
    int status;
    double x;
    double y;
    double xs;
    double ys;
    SDL_Rect mask;
    SDL_Rect rect;
};
struct SCORE
{
    int inita;
    int initb;
    int initc;
    int score;
};

typedef struct ENEMY Enemy;
typedef struct PLAYER Player;
typedef struct SHOT Shot;
typedef struct SCORE Score;

/* XPMS for graphics.  No files for the user to play around with! */
#include "./images/enemy1.xpm"
#include "./images/enemy2.xpm"
#include "./images/enemy3.xpm"
#include "./images/enemy4.xpm"
#include "./images/ship1.xpm"
#include "./images/explode.xpm"
#include "./images/digits.xpm"
#include "./images/words.xpm"
#include "./images/letters.xpm"
#include "./images/Logo.xpm"
#include "./images/intro.xpm"

SDL_Surface *screen;                    /* Main screen.  Needs to be local for timers */

#define NUMC 53

SDL_Color colors[NUMC];                 /* Colormap entries for color palette; */
SDL_mutex *Lockscreen;
int screentimer;                        /* Whether to display insert coin or highscore */
int screenmode;                         /* Which teaser to display */
int fsmode;
Player player;
Shot shot[20];
SDL_Surface *explode;
SDL_Surface *digits;
SDL_Surface *words;
SDL_Surface *letters;
SDL_Surface *hiscore;
SDL_Surface *newscore;
SDL_Surface *logo;
SDL_Surface *rules;
SDL_Rect hiscore_rect;
SDL_Rect drect[10];                     /* Digit Rectangles */
SDL_Rect wrect[11];                     /* Word Rectangles */
SDL_Rect lrect[37];                     /* Letter Rectangles (A-Z plus space plus 0-9) */
Score scores[5];
Enemy enemys[1000];                     /* Each enemy in the game */
SDL_Surface *enemypics[4];              /* Imagemaps for enemy (contains all frames) */
int NUMENEMYS;
int left;
int curr_level;                         /* Level of this game */
int credits;                            /* Need quarters to play! */

#define NUM_CHANNELS	16
#define pshot		0
#define twigshot	1
#define playerexplode	2
#define shielddown	3
#define startgame	4
#define coinin		5
#define enemyexplode	6
#define thrust		7

Mix_Chunk *sounds[10];

#define bg_music	0
#define hiscore_music	1

Mix_Music *musics[10];

void do_sounds(Mix_Chunk *what);

/******************************************************************************
 *
 * Testcollide - test whether a pair of images intersect. This function only
 *   works for square bounding boxes.  It checks each corner of both squares
 *   to see if the point is inside the other square.  Zero is returned if
 *   there is no intersection, otherwise it returns a number indicating the
 *   first failure that it finds.  This could be used for a positional damage
 *   model, but is not used in this game.
 *
 *****************************************************************************/
int testcollide(int e1x, int e1y, int e1xx, int e1yy, int e2x, int e2y, int e2xx, int e2yy)
{
    if ((e2x >= e1x) && (e2x < e1xx) && (e2y >= e1y) && (e2y < e1yy))
    {
        return 1;
    }
    if ((e2x >= e1x) && (e2x < e1xx) && (e2yy >= e1y) && (e2yy < e1yy))
    {
        return 2;
    }
    if ((e2xx >= e1x) && (e2xx < e1xx) && (e2y >= e1y) && (e2y < e1yy))
    {
        return 3;
    }
    if ((e2xx >= e1x) && (e2xx < e1xx) && (e2yy >= e1y) && (e2yy < e1yy))
    {
        return 4;
    }
    if ((e1x >= e2x) && (e1x < e2xx) && (e1y >= e2y) && (e1y < e2yy))
    {
        return 5;
    }
    if ((e1x >= e2x) && (e1x < e2xx) && (e1yy >= e2y) && (e1yy < e2yy))
    {
        return 6;
    }
    if ((e1xx >= e2x) && (e1xx < e2xx) && (e1y >= e2y) && (e1y < e2yy))
    {
        return 7;
    }
    if ((e1xx >= e2x) && (e1xx < e2xx) && (e1yy >= e2y) && (e1yy < e2yy))
    {
        return 8;
    }
    return 0;
}

/****************************************************************************
 *
 * Moveenemy - This procedure will move 1 enemy according to its x and y
 *   speed.  It will also increment the rotation counter, and if it is time
 *   for a rotation, handle that as well.  Movement will 'bounce' from the
 *   edges of the screen, and does account for the width/height of the object
 *   when checking the right/bottom.
 ****************************************************************************/
void moveenemy(Enemy *enemy, int who, Enemy all[])
{
    int i;
    double maxx;
    double maxy;
    double emx;
    double emy;
    double pmx;
    double pmy;
    double d;
    double a;
    double oldx;
    double oldy;
    double temp;
    int dochannel;

    if (enemy->status == 0)
    {
        return;
    }
    /* Compute rightmost and bottommost valid pixel */
    maxx = WIDTH - (enemy->w + 1);
    maxy = HEIGHT - (enemy->h + 1);

    oldx = enemy->x;
    oldy = enemy->y;

    pmx = player.x + (player.rect.w / 2.0);
    pmy = player.y + (player.rect.h / 2.0);
    emx = enemy->x + (enemy->w / 2.0);
    emy = enemy->y + (enemy->h / 2.0);
    /*
     * Type 0 enemys are boring.  They just sit there.  Boring as rocks.
     * Type 1 enemys will home in on the player if they get too close
     */
    if ((enemy->type == 1) && (player.status == 1))
    {
        if (sqrt(((pmx - emx) * (pmx - emx)) + ((pmy - emy) * (pmy - emy))) < 90)
        {
            enemy->xs = pmx - emx;
            enemy->ys = pmy - emy;
            /* Scale attack speed to something reasonable! */
            d = sqrt((enemy->xs * enemy->xs) + (enemy->ys * enemy->ys));
            if (d > 3)
            {
                if (enemy->xs == 0)
                {
                    if (enemy->ys > 0)
                    {
                        enemy->ys = 3.0;
                    }
                    else
                    {
                        enemy->ys = -3.0;
                    }
                }
                else
                {
                    if (enemy->ys == 0)
                    {
                        a = atan2(enemy->ys, enemy->xs);
                    }
                    else
                    {
                        a = atan2(-enemy->ys, enemy->xs);
                    }
                    enemy->xs = 3.0 * cos(a);
                    enemy->ys = -3.0 * sin(a);
                }
            }
        }
    }                                   /* End type 1 enemy! */
    /* Type 2 enemys will randomly change direction! */
    if ((enemy->type == 2) && ((RAND() % 100) < 4))
    {
        enemy->xs = ((RAND() % 70) / 10.0) - 3.0;
        enemy->ys = ((RAND() % 70) / 10.0) - 3.0;
        while ((abs(enemy->xs) < 1.1) && (abs(enemy->ys) < 1.1))
        {
            enemy->xs = ((RAND() % 70) / 10.0) - 3.0;
            enemy->ys = ((RAND() % 70) / 10.0) - 3.0;
        }
    }                                   /* End type 2 enemy */
    /* Type three enemys will randomly shoot at the player. */
    if ((enemy->type == 3) && (player.status == 1))
    {
        if ((enemy->shotstatus == 0) && ((RAND() % 100) < 5))
        {
            /*
             * Fire a new shot
             * Sound effect for shot
             */
            do_sounds(sounds[twigshot]);

            /* Figure out speed and direction */
            enemy->shotxs = pmx - emx;
            enemy->shotys = pmy - emy;
            d = sqrt((enemy->shotxs * enemy->shotxs) + (enemy->shotys * enemy->shotys));
            if (d > 5)
            {
                if (enemy->shotxs == 0)
                {
                    if (enemy->shotys > 0)
                    {
                        enemy->shotys = 5.0;
                    }
                    else
                    {
                        enemy->shotys = -5.0;
                    }
                }
                else
                {
                    if (enemy->shotys == 0)
                    {
                        a = atan2(enemy->shotys, enemy->shotxs);
                    }
                    else
                    {
                        a = atan2(-enemy->shotys, enemy->shotxs);
                    }
                    enemy->shotxs = 5.0 * cos(a);
                    enemy->shotys = -5.0 * sin(a);
                }
            }
            enemy->shotx = emx + (5 * enemy->shotxs);
            enemy->shoty = emy + (5 * enemy->shotys);
            enemy->shotstatus = 1;
        }
    }
    if ((enemy->type == 3) && (enemy->shotstatus == 1))
    {
        enemy->shotx += enemy->shotxs;
        enemy->shoty += enemy->shotys;
        if ((enemy->shotx <= 1) || (enemy->shoty <= 1) || (enemy->shotx >= WIDTH - 4) || (enemy->shoty >= HEIGHT - 4))
        {
            enemy->shotstatus = 0;
        }
        if ((testcollide(player.x, player.y, player.x + player.rect.w, player.y + player.rect.h, enemy->shotx,
                         enemy->shoty, enemy->shotx + 3, enemy->shoty + 3) > 0) && (player.status == 1))
        {
            do_sounds(sounds[shielddown]);
            enemy->shotstatus = 0;
            player.shield -= 5;
            if (player.shield < 0)
            {
                Mix_HaltMusic();
                do_sounds(sounds[playerexplode]);

                player.shield = 0;
                player.status = 2;
                player.timer = 0;
            }
        }
    }                                   /* Type 3 enemy */

    /* Adjust position by speed */
    enemy->x += enemy->xs;
    enemy->y += enemy->ys;
    /* Collision Detection */
    for (i = 0; i < NUMENEMYS; i++)
    {
        if (i == who)
        {
            continue;
        }
        if (all[i].status == 0)
        {
            continue;
        }
        if (testcollide(enemy->x, enemy->y, enemy->x + enemy->w, enemy->y + enemy->h, all[i].x, all[i].y, all[i].x +
                        all[i].w, all[i].y + all[i].h) > 0)
        {
            enemy->ys = -enemy->ys;
            enemy->xs = -enemy->xs;
            if ((RAND() % 5) == 0)
            {
                temp = enemy->ys;
                enemy->ys = enemy->xs;
                enemy->xs = temp;
            }
            enemy->x = oldx;
            enemy->y = oldy;
        }
    }
    /* Left bounce */
    if (enemy->x <= 1)
    {
        enemy->x = 2 + -enemy->x;
        enemy->xs = -enemy->xs;
    }
    /* Right bounce */
    if (enemy->x >= maxx)
    {
        enemy->x = maxx - (enemy->x - maxx);
        enemy->xs = -enemy->xs;
    }
    /* Top bounce */
    if (enemy->y <= 1)
    {
        enemy->y = 2 + -enemy->y;
        enemy->ys = -enemy->ys;
    }
    /* Bottom Bounce */
    if (enemy->y >= maxy)
    {
        enemy->y = maxy - (enemy->y - maxy);
        enemy->ys = -enemy->ys;
    }
    /*
     * A bounced enemy could end up moving into someone elses bounding box!
     * If so, move them back to their original position, but keep the change
     * in speed.
     */
    for (i = 0; i < NUMENEMYS; i++)
    {
        if (i == who)
        {
            continue;
        }
        if (all[i].status == 0)
        {
            continue;
        }
        if (testcollide(enemy->x, enemy->y, enemy->x + enemy->w, enemy->y + enemy->h, all[i].x, all[i].y, all[i].x +
                        all[i].w, all[i].y + all[i].h) > 0)
        {
            enemy->x = oldx;
            enemy->y = oldy;
        }
    }
    /* Rotation counter and rotation check */
    enemy->speedcount++;
    if (enemy->speedcount == enemy->speed)
    {
        enemy->speedcount = 0;
        enemy->pos += enemy->dir;
        if (enemy->pos == enemy->numpos)
        {
            enemy->pos = 0;
        }
        if (enemy->pos == -1)
        {
            enemy->pos += enemy->numpos;
        }
        enemy->rect.x = (enemy->pos * enemy->w) + 1;
        enemy->mask.x = (enemy->pos * enemy->w) + 1;
    }
    /* See if a shot hit us! */
    for (i = 0; i < 20; i++)
    {
        if (shot[i].status == 1)
        {
            if (testcollide(enemy->x, enemy->y, enemy->x + enemy->w, enemy->y + enemy->h, shot[i].x, shot[i].y,
                            shot[i].x + 3, shot[i].y + 3) > 0)
            {
                do_sounds(sounds[enemyexplode]);
                enemy->status = 2;
                shot[i].status = 2;
                return;
            }
        }
    }
}

/*****************************************************************************
 *
 * Placeenemy - Place an enemy on the screen in an inital position, as well as
 *     fill in the other values of the enemy struct.  The bitmap for the enemy
 *    is placed into master, and the surface blank is set also.  The enemy
 *    start position may not be near the center of the screen.
 *
 *****************************************************************************/
void placeenemy(Enemy *enemy, SDL_Surface *master, int pics, int width, int height, Enemy all[], int num, int points,
                int type)
{
    int done;                           /* Whether we have gotten a valid start point */
    int i;
    int notxl;                          /* We don't want enemys starting on top of the player */
    int notxh;
    int notyl;
    int notyh;

    enemy->type = type;
    enemy->images = master;
    enemy->pos = 0;
    enemy->numpos = pics;
    enemy->shotstatus = 0;
    enemy->points = points;
    enemy->w = width;
    enemy->h = height;
    enemy->xs = 0;
    enemy->ys = 0;

    enemy->dir = (RAND() % 2);
    if (enemy->dir == 0)
    {
        enemy->dir = -1;
    }
    ;
    /* Make sure that the enemy is moving */
    while ((abs(enemy->xs) < 1.1) && (abs(enemy->ys) < 1.1))
    {
        enemy->xs = ((RAND() % 70) / 10.0) - 3.0;
        enemy->ys = ((RAND() % 70) / 10.0) - 3.0;
    }
    /* How fast the rotation occurs */
    enemy->speed = (RAND() & 15) + 5;
    enemy->speedcount = 0;

    done = 0;

    /* Keep the middle clear */
    notxl = WIDTH / 2 - 50;
    notxh = WIDTH / 2 + 50;
    notyl = HEIGHT / 2 - 50;
    notyh = HEIGHT / 2 + 50;
    while (done == 0)
    {
        enemy->x = ((RAND() % (WIDTH * 10 - 800)) / 10.0) + 40.0;
        enemy->y = ((RAND() % (HEIGHT * 10 - 800)) / 10.0) + 40.0;
        /* Don't want it to appear in the middle of the screen */
        if ((enemy->x < notxl) || (enemy->x > notxh) || (enemy->y < notyl) || (enemy->y > notyh))
        {
            done = 1;
        }
        for (i = 0; i < num; i++)
        {
            if (testcollide(enemy->x, enemy->y, enemy->x + enemy->w, enemy->y + enemy->h, all[i].x, all[i].y, all[i].x +
                            all[i].w, all[i].y + all[i].h) > 0)
            {
                done = 0;
            }
        }
    }
    enemy->rect.x = 1; /* This keeps track of which rotation image we are showing */
    enemy->rect.y = 0;
    enemy->rect.w = width;
    enemy->rect.h = height;

    enemy->mask.x = 1;
    enemy->mask.y = height;
    enemy->mask.w = width;
    enemy->mask.h = height;

    enemy->explode.w = width;
    enemy->explode.h = height;
    enemy->explode.y = 0;

    enemy->explodemask.w = width;
    enemy->explodemask.h = height;
    enemy->explodemask.y = 40;

    enemy->shot_rect.w = 3;
    enemy->shot_rect.h = 3;
    enemy->shot_rect.x = 80;
    enemy->shot_rect.y = 30;
    enemy->shot_mask.w = 3;
    enemy->shot_mask.h = 3;
    enemy->shot_mask.x = 80;
    enemy->shot_mask.y = 70;
    switch (width)
    {
    case 40:
        enemy->explode.x = 0;
        enemy->explodemask.x = 0;
        break;
    case 30:
        enemy->explode.x = 40;
        enemy->explodemask.x = 40;
        break;
    case 20:
        enemy->explode.x = 70;
        enemy->explodemask.x = 70;
        break;
    }
    enemy->status = 1;                  /* Alive */
}

/***************************************************************************
 * Setup_player - Set the initial position and values of the player
 *
 **************************************************************************/
void setup_player(SDL_Surface *master)
{
    player.images = master;
    player.status = 1;
    player.facing = 0;
    player.jets = 0;

    player.x = (WIDTH - 40) / 2;
    player.y = (HEIGHT - 40) / 2;
    player.xs = 0;
    player.ys = 0;

    player.rect.y = 0;
    player.rect.x = player.facing * 40 + 1;
    player.rect.w = 40;
    player.rect.h = 40;
    player.mask.y = 80;
    player.mask.x = player.facing * 40 + 1;
    player.mask.w = 40;
    player.mask.h = 40;
    player.explode.x = 0;
    player.explode.y = 0;
    player.explode.w = 40;
    player.explode.h = 40;
    player.explodemask.x = 0;
    player.explodemask.y = 40;
    player.explodemask.w = 40;
    player.explodemask.h = 40;
}

/****************************************************************************
 *
 * Move_player - Move the player, and check for bouncing off the edge of the
 * screen.
 *
 ***************************************************************************/
void move_player()
{
    double maxx;
    double maxy;

    maxx = WIDTH - player.rect.w;
    maxy = HEIGHT - player.rect.h;

    player.x += player.xs;
    player.y += player.ys;
    if (player.x <= 1)
    {
        player.x = 2 - player.x;
        player.xs *= -1;
    }
    if (player.y <= 1)
    {
        player.y = 2 - player.y;
        player.ys *= -1;
    }
    if (player.x >= maxx)
    {
        player.x = maxx - (player.x - maxx);
        player.xs *= -1;
    }
    if (player.y >= maxy)
    {
        player.y = maxy - (player.y - maxy);
        player.ys *= -1;
    }
}

/****************************************************************************
 *
 * Drawscreen - Remove the old positions from the screen, and replace with the
 * new image at the new location.  Interval is the time until we count again
 * (it comes from the way that SDL handles a timer.  Param is the list of
 * enemys.  Param really isn't needed, as now the enemys are a global variable
 * (they didn't use to be until some of the other functions needed them, and I
 * got lazy about passing them everywhere).
 *
 ****************************************************************************/
Uint32 Drawscreen(Uint32 interval, void *param)
{
    SDL_Rect changes[1000];             /* Dirty rectangles */
    int i;                              /* Loop counter */
    int dochannel;
    int counter;                        /* Number of dirty rectangles */

    counter = 0;

    SDL_LockMutex(Lockscreen);
    /* Remove all ships from screen */
    for (i = 0; i < NUMENEMYS; i++)
    {
        /* Normal enemy erase */
        if (enemys[i].status == 1)
        {
            changes[counter].h = enemys[i].h;
            changes[counter].w = enemys[i].w;
            changes[counter].x = enemys[i].x;
            changes[counter].y = enemys[i].y;
            SDL_BlitSurface(enemys[i].images, &enemys[i].mask, screen, &changes[counter]);
            counter++;
            if (enemys[i].shotstatus == 1)
            {
                changes[counter].h = 3;
                changes[counter].w = 3;
                changes[counter].x = enemys[i].shotx;
                changes[counter].y = enemys[i].shoty;
                SDL_BlitSurface(explode, &enemys[i].shot_mask, screen, &changes[counter]);
                counter++;
            }
        }
        /* Blown up enemy erase */
        if (enemys[i].status == 3)
        {
            changes[counter].h = enemys[i].h;
            changes[counter].w = enemys[i].w;
            changes[counter].x = enemys[i].x;
            changes[counter].y = enemys[i].y;
            SDL_BlitSurface(explode, &enemys[i].explodemask, screen, &changes[counter]);
            counter++;
            enemys[i].status = 0;
        }
    }
    /* Normal player erase */
    if (player.status == 1)
    {
        changes[counter].h = player.rect.h;
        changes[counter].w = player.rect.w;
        changes[counter].x = player.x;
        changes[counter].y = player.y;
        SDL_BlitSurface(player.images, &player.mask, screen, &changes[counter]);
        counter++;
    }
    /* Universal Center Erase */
    if ((player.status == 0) || (player.status == 3))
    {
        changes[counter].h = 320;
        changes[counter].w = 320;
        changes[counter].x = (WIDTH - 320) / 2;
        changes[counter].y = (HEIGHT - 320) / 2;
        SDL_FillRect(screen, &changes[counter], SDL_MapRGB(screen->format, colors[2].r, colors[2].g, colors[2].b));
        counter++;
    }
    /* Directions erase */
    if ((player.status == 0) && (screenmode == 2))
    {
        screentimer++;
        if (screentimer == 200)
        {
            screentimer = 0;
            screenmode = 0;
        }
    }
    /* HIGH SCORE ERASE */
    if ((player.status == 0) && (screenmode == 1))
    {
        screentimer++;
        if (screentimer == 200)
        {
            screentimer = 0;
            screenmode = 2;
        }
    }
    /* INSERT COIN Erase */
    if ((player.status == 0) && (screenmode == 0))
    {
        screentimer++;
        if (screentimer == 200)
        {
            screentimer = 0;
            screenmode = 1;
        }
    }
    /* Blown up player erase */
    if ((player.status == 2) || (player.status == 3) && (player.timer == 0))
    {
        changes[counter].h = player.rect.h;
        changes[counter].w = player.rect.w;
        changes[counter].x = player.x;
        changes[counter].y = player.y;
        SDL_BlitSurface(explode, &player.explodemask, screen, &changes[counter]);
        counter++;
    }
    /* Adjust position of mask after we erase old position! */
    player.mask.x = player.rect.x;
    player.mask.y = player.rect.y + 80;
    /* Erase the missles and update position */
    for (i = 0; i < 20; i++)
    {
        if (shot[i].status > 0)
        {
            changes[counter].h = 3;
            changes[counter].w = 3;
            changes[counter].x = shot[i].x;
            changes[counter].y = shot[i].y;
            SDL_BlitSurface(shot[i].images, &shot[i].mask, screen, &changes[counter]);
            counter++;
            if (shot[i].status == 1)
            {
                shot[i].x += shot[i].xs;
                shot[i].y += shot[i].ys;
                if ((shot[i].x <= 1) || (shot[i].y <= 1) || (shot[i].x > (WIDTH - 4)) || (shot[i].y > (HEIGHT - 4)))
                {
                    shot[i].status = 0;
                }
                /* Need to check to see if we hit something! */
            }
            else     /* Shot.status == 2, so stop drawing it! */
            {
                shot[i].status = 0;
            }
        }
    }
    /* Move enemy ships and redraw in new position */
    for (i = 0; i < NUMENEMYS; i++)
    {
        if (enemys[i].status == 1)
        {
            moveenemy(&enemys[i], i, enemys);
            if (enemys[i].status == 1)
            {
                changes[counter].h = enemys[i].h;
                changes[counter].w = enemys[i].w;
                changes[counter].x = enemys[i].x;
                changes[counter].y = enemys[i].y;
                SDL_BlitSurface(enemys[i].images, &enemys[i].rect, screen, &changes[counter]);
            }
            else     /* We just got hit! status == 2 */
            {
                do_sounds(sounds[enemyexplode]);
                changes[counter].h = enemys[i].h;
                changes[counter].w = enemys[i].w;
                changes[counter].x = enemys[i].x;
                changes[counter].y = enemys[i].y;
                SDL_BlitSurface(explode, &enemys[i].explode, screen, &changes[counter]);
                enemys[i].status = 3;
                left -= 1;
                player.points += enemys[i].points;
                enemys[i].shotstatus = 0;
            }
            counter++;
            if (enemys[i].shotstatus == 1)
            {
                changes[counter].h = 3;
                changes[counter].w = 3;
                changes[counter].x = enemys[i].shotx;
                changes[counter].y = enemys[i].shoty;
                SDL_BlitSurface(explode, &enemys[i].shot_rect, screen, &changes[counter]);
                counter++;
            }
        }
    }
    /* Now we need to move the player! */
    move_player();
    if (player.status == 1)
    {
        for (i = 0; i < NUMENEMYS; i++)
        {
            if (enemys[i].status == 0)
            {
                continue;
            }
            if (testcollide(player.x, player.y, player.x + player.rect.w, player.y + player.rect.h, enemys[i].x,
                            enemys[i].y, enemys[i].x + enemys[i].w, enemys[i].y + enemys[i].h) > 0)   /* Player just got clobbered! */
            {
                do_sounds(sounds[shielddown]);
                player.shield--;
                if (player.shield < 0)
                {
                    Mix_HaltMusic();
                    do_sounds(sounds[playerexplode]);

                    player.shield = 0;
                    player.status = 2;
                    player.timer = 0;
                    break;
                }
            }
        }
    }
    /* Which image of player to draw */
    if ((player.status == 0) && (screenmode == 1))
    {
        changes[counter].h = 84;
        changes[counter].w = 120;
        changes[counter].x = (WIDTH - 120) / 2;
        changes[counter].y = (HEIGHT - 84) / 2;
        SDL_BlitSurface(hiscore, &hiscore_rect, screen, &changes[counter]);
        counter++;
    }
    /* INSERT COIN player */
    if ((player.status == 0) && (screenmode == 0))
    {
        player.timer = 0;
        changes[counter].h = 15;
        changes[counter].w = 121;
        changes[counter].x = 190;
        changes[counter].y = 232;
        SDL_BlitSurface(words, &wrect[0], screen, &changes[counter]);
        counter++;
    }
    /* New high score */
    if ((player.status == 0) && (screenmode == -1))
    {
        changes[counter].h = 84;
        changes[counter].w = 200;
        changes[counter].x = (WIDTH - 200) / 2;
        changes[counter].y = (HEIGHT - 84) / 2;
        SDL_BlitSurface(newscore, NULL, screen, &changes[counter]);
        counter++;
    }
    /* Rules */
    if ((player.status == 0) && (screenmode == 2))
    {
        changes[counter].h = 320;
        changes[counter].w = 320;
        changes[counter].x = (WIDTH - 320) / 2;
        changes[counter].y = (HEIGHT - 320) / 2;
        SDL_BlitSurface(rules, NULL, screen, &changes[counter]);
        counter++;
    }
    /* GAME OVER player */
    if (player.status == 3)
    {
        player.timer++;
        changes[counter].h = 15;
        changes[counter].w = 97;
        changes[counter].x = 202;
        changes[counter].y = 232;
        SDL_BlitSurface(words, &wrect[1], screen, &changes[counter]);
        counter++;
        if (player.timer == 200)
        {
            player.status = 0;
            screentimer = 0;
            if (player.points > scores[4].score)
            {
                screenmode = -1;

                /*SDL_FillRect(newscore,NULL,
                 *           SDL_MapRGB(newscore->format,colors[0].r,colors[0].g,
                 *           colors[0].b));*/
            }
            else
            {
                screenmode = 1;
            }
        }
    }
    /* Normal player */
    if (player.status == 1)
    {
        changes[counter].h = player.rect.h;
        changes[counter].w = player.rect.w;
        changes[counter].x = player.x;
        changes[counter].y = player.y;
        SDL_BlitSurface(player.images, &player.rect, screen, &changes[counter]);
        counter++;
        player.timer = 0;
    }
    /* Blown up player */
    if (player.status == 2)
    {
        changes[counter].h = 40; /* player.rect.h; */
        changes[counter].w = 40; /* player.rect.w; */
        changes[counter].x = player.x;
        changes[counter].y = player.y;
        SDL_BlitSurface(explode, &player.explode, screen, &changes[counter]);
        counter++;
        player.timer++;
        if (player.timer == 5)
        {
            player.status = 3;
            player.timer = 0;
        }
    }
    /* Draw new positions of the shots */
    for (i = 0; i < 20; i++)
    {
        if (shot[i].status == 1)
        {
            changes[counter].h = 3;
            changes[counter].w = 3;
            changes[counter].x = shot[i].x;
            changes[counter].y = shot[i].y;
            SDL_BlitSurface(shot[i].images, &shot[i].rect, screen, &changes[counter]);
            counter++;
        }
    }
    /* Update all movement changes */
    SDL_UpdateRects(screen, counter, changes);

    /* Blank out the score section */
    changes[0].x = 510;
    changes[0].w = 120;
    changes[0].y = 150;
    changes[0].h = 25;
    SDL_FillRect(screen, &changes[0], SDL_MapRGB(screen->format, colors[2].r, colors[2].g, colors[2].b));

    /* Write the digits for the score to the screen */
    changes[1].x = 510;
    changes[1].y = 150;
    changes[1].w = 20;
    changes[1].h = 23;
    SDL_BlitSurface(digits, &drect[(player.points / 100000) % 10], screen, &changes[1]);
    changes[2].x = 530;
    changes[2].y = 150;
    changes[2].w = 20;
    changes[2].h = 23;
    SDL_BlitSurface(digits, &drect[(player.points / 10000) % 10], screen, &changes[2]);
    changes[3].x = 550;
    changes[3].y = 150;
    changes[3].w = 20;
    changes[3].h = 23;
    SDL_BlitSurface(digits, &drect[(player.points / 1000) % 10], screen, &changes[3]);
    changes[4].x = 570;
    changes[4].y = 150;
    changes[4].w = 20;
    changes[4].h = 23;
    SDL_BlitSurface(digits, &drect[(player.points / 100) % 10], screen, &changes[4]);
    changes[5].x = 590;
    changes[5].y = 150;
    changes[5].w = 20;
    changes[5].h = 23;
    SDL_BlitSurface(digits, &drect[(player.points / 10) % 10], screen, &changes[5]);
    changes[6].x = 610;
    changes[6].y = 150;
    changes[6].w = 20;
    changes[6].h = 23;
    SDL_BlitSurface(digits, &drect[(player.points / 1) % 10], screen, &changes[6]);

    /* Write digits for the level */
    changes[7].x = 570;
    changes[7].w = 60;
    changes[7].y = 100;
    changes[7].h = 25;
    SDL_FillRect(screen, &changes[7], SDL_MapRGB(screen->format, colors[2].r, colors[2].g, colors[2].b));

    changes[8].x = 570;
    changes[8].y = 100;
    changes[8].w = 20;
    changes[8].h = 23;
    SDL_BlitSurface(digits, &drect[(curr_level / 100) % 10], screen, &changes[8]);
    changes[9].x = 590;
    changes[9].y = 100;
    changes[9].w = 20;
    changes[9].h = 23;
    SDL_BlitSurface(digits, &drect[(curr_level / 10) % 10], screen, &changes[9]);
    changes[10].x = 610;
    changes[10].y = 100;
    changes[10].w = 20;
    changes[10].h = 23;
    SDL_BlitSurface(digits, &drect[(curr_level / 1) % 10], screen, &changes[10]);

    /* Write digits for the credits */
    changes[11].x = 590;
    changes[11].w = 40;
    changes[11].y = 400;
    changes[11].h = 25;
    SDL_FillRect(screen, &changes[11], SDL_MapRGB(screen->format, colors[2].r, colors[2].g, colors[2].b));

    changes[12].x = 590;
    changes[12].y = 400;
    changes[12].w = 20;
    changes[12].h = 23;
    SDL_BlitSurface(digits, &drect[(credits / 10) % 10], screen, &changes[12]);
    changes[13].x = 610;
    changes[13].y = 400;
    changes[13].w = 20;
    changes[13].h = 23;
    SDL_BlitSurface(digits, &drect[(credits / 1) % 10], screen, &changes[13]);

    /* Write block for shield */
    changes[14].x = 530;
    changes[14].w = 40;
    changes[14].y = 250;
    changes[14].h = 100;
    SDL_FillRect(screen, &changes[14], SDL_MapRGB(screen->format, colors[2].r, colors[2].g, colors[2].b));

    /* Top of shield */
    changes[15].x = 530;
    changes[15].w = 40;
    changes[15].y = 250;
    changes[15].h = player.shield;
    SDL_FillRect(screen, &changes[15], SDL_MapRGB(screen->format, colors[3].r, colors[3].g, colors[3].b));

    /* Expired shield */
    changes[16].x = 530;
    changes[16].w = 40;
    changes[16].y = 250 + player.shield;
    changes[16].h = 100 - player.shield;
    SDL_FillRect(screen, &changes[16], SDL_MapRGB(screen->format, colors[8].r, colors[8].g, colors[8].b));

    SDL_UpdateRects(screen, 17, changes);

    /* Done with the screen */
    SDL_UnlockMutex(Lockscreen);

    /* Keep the screen redraw rate constant */
    return interval;
}

/***************************************************************************
 *
 * setup_image - Perform two simple tasks for each SDL_Surface that we need.
 *       Mainly used to save space in main.
 *
 **************************************************************************/
void setup_image(SDL_Surface *which)
{
    int test;

    test = SDL_SetPalette(which, SDL_LOGPAL | SDL_PHYSPAL, colors, 0, NUMC);

    test = SDL_SetColorKey(which, SDL_SRCCOLORKEY, /* |SDL_RLEACCEL, */
                           SDL_MapRGB(which->format, colors[0].r, colors[0].g, colors[0].b));

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, colors[0].r, colors[0].g, colors[0].b));

    /* SDL_UpdateRect(which, 0,0,0,0); */
}

/****************************************************************************
 *
 * populate board - Initial set up of the bad guys for this level.  Number of
 * bad guys is dependant on the level of the board.  Note that level is pass
 * by value, so if level 0 (game not active) is passed, a level 10 board
 * is displayed for the teaser.  Since it is pass by value, the level stays
 * 0 in the caller.
 ***************************************************************************/
int populate_board(int level)
{
    int i;
    int max;
    int pos;

    if (level == 0)
    {
        level = 10;    /* For demo purposes! */
    }

    max = 2 + level + (level / 5);
    pos = 0;
    for (i = 0; i < max; i++)
    {
        placeenemy(&enemys[pos], enemypics[1], 8, 30, 30, enemys, pos, 15, 0);
        pos++;
    }
    max = 1 + (level / 3) + (level / 11);
    for (i = 0; i < max; i++)
    {
        placeenemy(&enemys[pos], enemypics[2], 2, 20, 20, enemys, pos, 30, 1);
        pos++;
    }
    max = (level / 4) + (level / 7);
    for (i = 0; i < max; i++)
    {
        placeenemy(&enemys[pos], enemypics[0], 4, 20, 20, enemys, pos, 50, 2);
        pos++;
    }
    max = (level / 5) + (level / 8);
    for (i = 0; i < max; i++)
    {
        placeenemy(&enemys[pos], enemypics[3], 3, 30, 30, enemys, pos, 100, 3);
        pos++;
    }
    NUMENEMYS = pos;
    left = pos;
    return pos;
}

/*****************************************************************************
 *
 * blankscreen - completely blank the screen (with the off green color), and
 * draw the edge lines around the screen.  I think this may need to have the
 * locksurface commands, but I don't really know.  It seems to work as is, so
 * I am going to leave it like this until it breaks.
 *
 ****************************************************************************/
void blankscreen()
{
    int i;
    Uint8 *pixel;
    Uint32 color;
    SDL_Rect changes;
    int test;

    test = SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, colors[2].r, colors[2].g, colors[2].b));

    SDL_UpdateRect(screen, 0, 0, 0, 0);

    color = SDL_MapRGB(screen->format, colors[1].r, colors[1].g, colors[1].b);
    SDL_LockSurface(screen);
    /* Horizontal Lines */
    for (i = 0; i < 640; i++)
    {
        pixel = (Uint8 *)screen->pixels + 0 * screen->pitch + i * screen->format->BytesPerPixel;
        *pixel = color;

        pixel = (Uint8 *)screen->pixels + (HEIGHT - 1) * screen->pitch + i * screen->format->BytesPerPixel;
        *pixel = color;
    }
    /* Vertical Lines */
    for (i = 0; i < HEIGHT; i++)
    {
        pixel = (Uint8 *)screen->pixels + i * screen->pitch + 0 * screen->format->BytesPerPixel;
        *pixel = color;

        pixel = (Uint8 *)screen->pixels + i * screen->pitch + (639) * screen->format->BytesPerPixel;
        *pixel = color;

        pixel = (Uint8 *)screen->pixels + i * screen->pitch + WIDTH * screen->format->BytesPerPixel;
        *pixel = color;

        pixel = (Uint8 *)screen->pixels + i * screen->pitch + (WIDTH + 1) * screen->format->BytesPerPixel;
        *pixel = color;
    }
    SDL_UnlockSurface(screen);

    /* Write level label */
    changes.x = 570;
    changes.w = 36;
    changes.y = 85;
    changes.h = 10;
    SDL_BlitSurface(words, &wrect[2], screen, &changes);

    /* Write score label */
    changes.x = 510;
    changes.w = 33;
    changes.y = 135;
    changes.h = 10;
    SDL_BlitSurface(words, &wrect[3], screen, &changes);

    /* Write credits label */
    changes.x = 590;
    changes.w = 47;
    changes.y = 385;
    changes.h = 10;
    SDL_BlitSurface(words, &wrect[4], screen, &changes);

    /* Write shield label */
    changes.x = 530;
    changes.w = 41;
    changes.y = 235;
    changes.h = 10;
    SDL_BlitSurface(words, &wrect[5], screen, &changes);

    /* Write copyright */
    changes.x = 508;
    changes.w = 130;
    changes.y = 455;
    changes.h = 20;
    SDL_BlitSurface(words, &wrect[6], screen, &changes);

    /* Write logo */
    changes.x = 510;
    changes.y = 5;
    changes.w = 120;
    changes.h = 60;
    SDL_BlitSurface(logo, NULL, screen, &changes);

    SDL_UpdateRect(screen, 0, 0, 0, 0);
}

/****************************************************************************
 *
 * Update_hiscore - Redraw the hi_score screen.  Call this each time the
 * scores variable is updated.  This function only updates the scores surface,
 * it does not acutally draw anything on the main screen.  That is handled in
 * the drawscreen function.
 *
 ****************************************************************************/
void update_hiscore()
{
    int i;
    SDL_Rect loc;
    int header[11] = { 7, 8, 6, 7, 26, 18, 2, 14, 17, 4, 18 };  /* HIGH SCORE */

    SDL_FillRect(hiscore, NULL, SDL_MapRGB(hiscore->format, colors[0].r, colors[0].g, colors[0].b));
    for (i = 0; i < 11; i++)
    {
        loc.x = 5 + i * 10;
        loc.y = 0;
        loc.w = 10;
        loc.h = 12;
        SDL_BlitSurface(letters, &lrect[header[i]], hiscore, &loc);
    }
    for (i = 0; i < 5; i++)
    {
        loc.x = 10;
        loc.y = 24 + (i * 12);
        loc.w = 10;
        loc.h = 12;
        SDL_BlitSurface(letters, &lrect[scores[i].inita], hiscore, &loc);
        loc.x = 20;
        loc.y = 24 + (i * 12);
        loc.w = 10;
        loc.h = 12;
        SDL_BlitSurface(letters, &lrect[scores[i].initb], hiscore, &loc);
        loc.x = 30;
        loc.y = 24 + (i * 12);
        loc.w = 10;
        loc.h = 12;
        SDL_BlitSurface(letters, &lrect[scores[i].initc], hiscore, &loc);

        loc.x = 50;
        loc.y = 24 + (i * 12);
        loc.w = 10;
        loc.h = 12;
        SDL_BlitSurface(letters, &lrect[(scores[i].score / 100000) % 10] + 27, hiscore, &loc);
        loc.x = 60;
        loc.y = 24 + (i * 12);
        loc.w = 10;
        loc.h = 12;
        SDL_BlitSurface(letters, &lrect[(scores[i].score / 10000) % 10] + 27, hiscore, &loc);
        loc.x = 70;
        loc.y = 24 + (i * 12);
        loc.w = 10;
        loc.h = 12;
        SDL_BlitSurface(letters, &lrect[(scores[i].score / 1000) % 10] + 27, hiscore, &loc);
        loc.x = 80;
        loc.y = 24 + (i * 12);
        loc.w = 10;
        loc.h = 12;
        SDL_BlitSurface(letters, &lrect[(scores[i].score / 100) % 10] + 27, hiscore, &loc);
        loc.x = 90;
        loc.y = 24 + (i * 12);
        loc.w = 10;
        loc.h = 12;
        SDL_BlitSurface(letters, &lrect[(scores[i].score / 10) % 10] + 27, hiscore, &loc);
        loc.x = 100;
        loc.y = 24 + (i * 12);
        loc.w = 10;
        loc.h = 12;
        SDL_BlitSurface(letters, &lrect[(scores[i].score / 1) % 10] + 27, hiscore, &loc);
    }
    hiscore_rect.x = 0;
    hiscore_rect.y = 0;
    hiscore_rect.w = 120;
    hiscore_rect.h = 84;
    SDL_UpdateRect(hiscore, 0, 0, 0, 0);
}

/***************************************************************************
 *
 * read_HISCORE - read the highscore information from the file hiscore.dat
 * if it exists.  Otherwise, make a default hiscore setup.  Note that
 * internally, the hiscore digits are represented as characters offsets in
 * the letter list.  In the file, theryy are stored as ASCII characters.
 *
 *************************************************************************/
void read_HISCORE()
{
    FILE *infile;
    int i;
    char letter;
    int num;

    infile = fopen("hiscore.dat", "r");
    if (infile == NULL)
    {
        /* Initial highscores */
        for (i = 0; i < 5; i++)
        {
            scores[i].inita = 0;
            scores[i].initb = 0;
            scores[i].initc = 0;
            scores[i].score = 100 * (5 - i) + i + 10;
        }
        update_hiscore();
        return;
    }
    for (i = 0; i < 5; i++)
    {
        fscanf(infile, "%c", &letter);
        if ((letter > 64) && (letter < 91))
        {
            scores[i].inita = letter - 65;
        }
        else if ((letter > 47) && (letter < 58))
        {
            scores[i].inita = letter - 48;
        }
        else
        {
            scores[i].inita = 26;
        }
        fscanf(infile, "%c", &letter);
        if ((letter > 64) && (letter < 91))
        {
            scores[i].initb = letter - 65;
        }
        else if ((letter > 47) && (letter < 58))
        {
            scores[i].initb = letter - 48;
        }
        else
        {
            scores[i].initb = 26;
        }
        fscanf(infile, "%c", &letter);
        if ((letter > 64) && (letter < 91))
        {
            scores[i].initc = letter - 65;
        }
        else if ((letter > 47) && (letter < 58))
        {
            scores[i].initc = letter - 48;
        }
        else
        {
            scores[i].initc = 26;
        }
        fscanf(infile, "%d", &scores[i].score);
        fscanf(infile, "%c", &letter);  /* read newline character! */
    }
    fclose(infile);
    update_hiscore();
}

/****************************************************************************
 *
 * new_HISCORE - The players score is larger than the lowest of the five best
 * highscores.  So, we need to get their initials, and add the score to the
 * appropriate place in the highscore list.  We set up a newscore Image that
 * can be blitted into the center of the main screen.  This function also sets
 * up it's own keyboard loop so that we can cycle and record the initials.
 * Once the initials have been chosen, the score is inserted into the five
 * best, displacing one score below it.  Control is then returned to the main
 * event loop.
 *
 ***************************************************************************/
void new_HISCORE(int score)
{
    int i;
    SDL_Rect loc;
    SDL_Event event;                    /* Handles event loop */
    Uint8 *keystate;                    /* Used for reading multiple keys */
    int doneclick;
    int backclick;
    int done;
    int pos;
    Score holder;
    Score temp;
    int fs_click;                                                                               /* In case they do a fs toggle in here. */
    int credit_click;
    int header[12] = { 0, 26, 13, 4, 22, 26, 17, 4, 2, 14, 17, 3 };                             /* A NEW RECORD */
    int h2[19] = { 4, 13, 19, 4, 17, 26, 24, 14, 20, 17, 26, 8, 13, 8, 19, 8, 0, 11, 18 };      /* ENTER Y */
    int h3[3];

    fs_click = 0;
    doneclick = 0;
    backclick = 0;
    credit_click = 0;

    Mix_PlayMusic(musics[hiscore_music], -1);

    /*
     * Need to lock the screen, since we are updating something that might be
     * Drawn while we are getting it ready.
     */
    SDL_LockMutex(Lockscreen);

    /* Start with clearing the image */
    SDL_FillRect(newscore, NULL, SDL_MapRGB(newscore->format, colors[0].r, colors[0].g, colors[0].b));
    /* Blit A NEW RECORD */
    for (i = 0; i < 12; i++)
    {
        loc.x = 40 + i * 10;
        loc.y = 0;
        loc.w = 10;
        loc.h = 12;
        SDL_BlitSurface(letters, &lrect[header[i]], newscore, &loc);
    }
    /* Blit ENTER YOUR INITIALS */
    for (i = 0; i < 19; i++)
    {
        loc.x = 5 + i * 10;
        loc.y = 15;
        loc.w = 10;
        loc.h = 12;
        SDL_BlitSurface(letters, &lrect[h2[i]], newscore, &loc);
    }
    /* Start out at AAA */
    h3[0] = 0;
    h3[1] = 0;
    h3[2] = 0;
    for (i = 0; i < 3; i++)
    {
        loc.x = 85 + i * 10;
        loc.y = 35;
        loc.w = 10;
        loc.h = 12;
        SDL_BlitSurface(letters, &lrect[h3[i]], newscore, &loc);
    }
    done = 0;
    pos = 0;

    loc.x = 87 + pos * 10;
    loc.y = 50;
    loc.w = 3;
    loc.h = 3;
    SDL_BlitSurface(explode, &shot[i].rect, newscore, &loc);

    SDL_UpdateRect(newscore, 0, 0, 0, 0);
    SDL_UnlockMutex(Lockscreen);
    while (done == 0)
    {
        SDL_Delay(100);                 /* Don't want to cycle the rotate too fast */
        SDL_PumpEvents();

        keystate = SDL_GetKeyState(NULL);
        /* Rotate this initial LEFT */
        if (keystate[keys[LEFTKEY]])
        {
            h3[pos]--;
            if (h3[pos] < 0)
            {
                h3[pos] = 36;
            }
            SDL_LockMutex(Lockscreen);
            loc.x = 85 + pos * 10;
            loc.y = 35;
            loc.w = 10;
            loc.h = 12;
            SDL_FillRect(newscore, &loc, SDL_MapRGB(newscore->format, colors[0].r, colors[0].g, colors[0].b));
            SDL_BlitSurface(letters, &lrect[h3[pos]], newscore, &loc);
            SDL_UpdateRect(newscore, loc.x, loc.y, loc.w, loc.h);
            SDL_UnlockMutex(Lockscreen);
        }
        /* Rotate this initial RIGHT */
        if (keystate[keys[RIGHTKEY]])
        {
            h3[pos]++;
            if (h3[pos] == 37)
            {
                h3[pos] = 0;
            }
            SDL_LockMutex(Lockscreen);
            loc.x = 85 + pos * 10;
            loc.y = 35;
            loc.w = 10;
            loc.h = 12;
            SDL_FillRect(newscore, &loc, SDL_MapRGB(newscore->format, colors[0].r, colors[0].g, colors[0].b));
            SDL_BlitSurface(letters, &lrect[h3[pos]], newscore, &loc);
            SDL_UpdateRect(newscore, loc.x, loc.y, loc.w, loc.h);
            SDL_UnlockMutex(Lockscreen);
        }
        /* lock this initial */
        if (keystate[keys[FIREKEY]])
        {
            doneclick = 1;
        }
        if ((doneclick == 1) && (!keystate[keys[FIREKEY]]))
        {
            loc.x = 87 + pos * 10;
            loc.y = 50;
            loc.w = 3;
            loc.h = 3;
            SDL_BlitSurface(explode, &shot[i].mask, newscore, &loc);
            pos++;
            if (pos < 3)
            {
                loc.x = 87 + pos * 10;
                loc.y = 50;
                loc.w = 3;
                loc.h = 3;
                SDL_BlitSurface(explode, &shot[i].rect, newscore, &loc);
            }
            doneclick = 0;
        }
        if (keystate[keys[UPKEY]])
        {
            backclick = 1;
        }
        if ((backclick == 1) && (!keystate[keys[UPKEY]]))
        {
            loc.x = 87 + pos * 10;
            loc.y = 50;
            loc.w = 3;
            loc.h = 3;
            SDL_BlitSurface(explode, &shot[i].mask, newscore, &loc);
            pos--;
            if (pos == -1)
            {
                pos = 0;
            }
            loc.x = 87 + pos * 10;
            loc.y = 50;
            loc.w = 3;
            loc.h = 3;
            SDL_BlitSurface(explode, &shot[i].rect, newscore, &loc);
            backclick = 0;
        }
        /* Keystroke for insert coin */
        if (keystate[keys[COINKEY]])
        {
            credit_click = 1;
        }
        if ((!keystate[keys[COINKEY]]) && (credit_click == 1))
        {
            do_sounds(sounds[coinin]);
            credits++;
            credit_click = 0;
        }
        /* If this was the third, we are done! */
        if (pos == 3)
        {
            done = 1;
        }
        if (keystate[keys[QUITKEY]])
        {
            exit(0);
        }
        if (keystate[keys[FSKEY]])
        {
            fs_click = 1;
        }
        if ((fs_click == 1) && (!keystate[keys[FSKEY]]))
        {
            /* Change this to use SETVIDEOMODE */
            fsmode = fsmode ^ SDL_FULLSCREEN;
            SDL_LockMutex(Lockscreen);
            screen = SDL_SetVideoMode(640, 480, 8, fsmode);
            setup_image(screen);
            blankscreen();
            SDL_UnlockMutex(Lockscreen);
            fs_click = 0;
        }
        /* In case of a QUIT even while we are getting the initials! */
        if (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            }
        }
    }                                   /* While not done */

    /*
     * holder holds the score that is not in the array.  Here it is the one
     * we are filling in.  Later it will be used to cycle scores in and out
     * of the array as scores trickle down.
     */
    holder.inita = h3[0];
    holder.initb = h3[1];
    holder.initc = h3[2];
    holder.score = score;
    /* Trickle scores down. */
    for (i = 0; i < 5; i++)
    {
        if (holder.score > scores[i].score)
        {
            temp.inita = scores[i].inita;
            temp.initb = scores[i].initb;
            temp.initc = scores[i].initc;
            temp.score = scores[i].score;
            scores[i].inita = holder.inita;
            scores[i].initb = holder.initb;
            scores[i].initc = holder.initc;
            scores[i].score = holder.score;
            holder.inita = temp.inita;
            holder.initb = temp.initb;
            holder.initc = temp.initc;
            holder.score = temp.score;
        }
    }
    /* Update the highscore screen, and return to the teaser loop */
    update_hiscore();
    screenmode = 1;
    screentimer = 0;
    Mix_FadeOutMusic(4000);
}

/*************************************************************************
 *
 * write_HISCORE - write out the contents of the hiscore array to the file
 * hiscore.dat.  The internal representation of the initials is not the same
 * as used in the file.  See read_HISCORE for details.  This function is
 * automatically called upon a normal exit.
 *
 **************************************************************************/
void write_HISCORE()
{
    FILE *outfile;
    int i;
    char letter;

    outfile = fopen("hiscore.dat", "w+");
    for (i = 0; i < 5; i++)
    {
        if (scores[i].inita < 26)
        {
            fprintf(outfile, "%c", (char)(scores[i].inita + 65));
        }
        if (scores[i].inita > 26)
        {
            fprintf(outfile, "%c", (char)(scores[i].inita + 48));
        }
        if (scores[i].inita == 26)
        {
            fprintf(outfile, "%c", ' ');
        }
        if (scores[i].initb < 26)
        {
            fprintf(outfile, "%c", (char)(scores[i].initb + 65));
        }
        if (scores[i].initb > 26)
        {
            fprintf(outfile, "%c", (char)(scores[i].initb + 48));
        }
        if (scores[i].initb == 26)
        {
            fprintf(outfile, "%c", ' ');
        }
        if (scores[i].initc < 26)
        {
            fprintf(outfile, "%c", (char)(scores[i].initc + 65));
        }
        if (scores[i].initc > 26)
        {
            fprintf(outfile, "%c", (char)(scores[i].initc + 48));
        }
        if (scores[i].initc == 26)
        {
            fprintf(outfile, "%c", ' ');
        }
        fprintf(outfile, " %d\n", scores[i].score);
    }
    fclose(outfile);
}

/***************************************************************************
 *
 * read_CONFIG - Read in a config file (if it doesn't exist, use defaults)
 *   to set which keys are used for which operations.  It also lets the user
 *   determine whether to start in full screen or windowed mode.
 *
 ****************************************************************************/
void read_CONFIG()
{
    FILE *infile;
    char array[800];

    /* Set defaults */
    keys[UPKEY] = SDLK_UP;
    keys[LEFTKEY] = SDLK_LEFT;
    keys[RIGHTKEY] = SDLK_RIGHT;
    keys[FIREKEY] = SDLK_SPACE;
    keys[PAUSEKEY] = SDLK_p;
    keys[COINKEY] = SDLK_5;
    keys[STARTKEY] = SDLK_1;
    keys[QUITKEY] = SDLK_ESCAPE;
    keys[FSKEY] = SDLK_F11;

    fsmode = SDL_SWSURFACE | SDL_RLEACCEL | SDL_SRCCOLORKEY;    /* Windowed */

    infile = fopen("tis.conf", "r");
    /* If there is no config file, use default settings */
    if (infile == NULL)
    {
        return;
    }
    /* Otherwise, read file */
    while (fscanf(infile, "%80s", array) != EOF)
    {
        /* Got a mode line */
        if (strcmp(array, "mode") == 0)
        {
            fscanf(infile, "%80s", array);
            if (strcmp(array, "FULLSCREEN") == 0)
            {
                fsmode = SDL_SWSURFACE | SDL_FULLSCREEN | SDL_RLEACCEL | SDL_SRCCOLORKEY;
            }
            else
            {
                fsmode = SDL_SWSURFACE | SDL_RLEACCEL | SDL_SRCCOLORKEY;
            }
        }
        /* Got an upkey line */
        if (strcmp(array, "upkey") == 0)
        {
            fscanf(infile, "%d", &keys[UPKEY]);
        }
        /* Got a leftkey line */
        if (strcmp(array, "leftkey") == 0)
        {
            fscanf(infile, "%d", &keys[LEFTKEY]);
        }
        /* Got a rightkey line */
        if (strcmp(array, "rightkey") == 0)
        {
            fscanf(infile, "%d", &keys[RIGHTKEY]);
        }
        /* Got a firekey line */
        if (strcmp(array, "firekey") == 0)
        {
            fscanf(infile, "%d", &keys[FIREKEY]);
        }
        /* Got a pausekey line */
        if (strcmp(array, "pausekey") == 0)
        {
            fscanf(infile, "%d", &keys[PAUSEKEY]);
        }
        /* Got a coinkey line */
        if (strcmp(array, "coinkey") == 0)
        {
            fscanf(infile, "%d", &keys[COINKEY]);
        }
        /* Got a startkey line */
        if (strcmp(array, "startkey") == 0)
        {
            fscanf(infile, "%d", &keys[STARTKEY]);
        }
        /* Got a quitkey line */
        if (strcmp(array, "quitkey") == 0)
        {
            fscanf(infile, "%d", &keys[QUITKEY]);
        }
        /* Got a fskey line */
        if (strcmp(array, "fskey") == 0)
        {
            fscanf(infile, "%d", &keys[FSKEY]);
        }
    }                                   /* End while */

    fclose(infile);
}

/***************************************************************************
 *
 * setup_rules() - This function sets up the rules screen.  It basically
 * takes the bulk of the rules from the xpm.  Onto that, it blits an example
 * of each type of enemy in front of it's description.  This Surface is used
 * as part of the insert coin rotation.
 *
 **************************************************************************/
void setup_rules()
{
    SDL_Rect pos;
    SDL_Rect bound;

    rules = IMG_ReadXPMFromArray(intro_xpm);
    setup_image(rules);

    pos.x = 20;
    pos.y = 133;
    pos.w = 30;
    pos.h = 30;
    bound.x = 1;
    bound.y = 0;
    bound.w = 30;
    bound.h = 30;
    SDL_BlitSurface(enemypics[1], &bound, rules, &pos);

    pos.x = 30;
    pos.y = 180;
    pos.w = 20;
    pos.h = 20;
    bound.x = 1;
    bound.y = 0;
    bound.w = 20;
    bound.h = 20;
    SDL_BlitSurface(enemypics[2], &bound, rules, &pos);

    pos.x = 30;
    pos.y = 225;
    pos.w = 20;
    pos.h = 20;
    bound.x = 1;
    bound.y = 0;
    bound.w = 20;
    bound.h = 20;
    SDL_BlitSurface(enemypics[0], &bound, rules, &pos);

    pos.x = 20;
    pos.y = 265;
    pos.w = 30;
    pos.h = 30;
    bound.x = 30;
    bound.y = 0;
    bound.w = 30;
    bound.h = 30;
    SDL_BlitSurface(enemypics[3], &bound, rules, &pos);

    SDL_UpdateRect(rules, 0, 0, 0, 0);
}

/****************************************************************************
 *
 * do_sounds - play a sound out of the LRU channel.  Makes the addition of
 * playing a sound a one liner from anywhere else in the code.  The function
 * will find an unused channel if one is available, otherwise it will pick
 * the oldest one.
 ****************************************************************************/
void do_sounds(Mix_Chunk *what)
{
    int channel;

    channel = Mix_GroupAvailable(1);
    if (channel == -1)
    {
        channel = Mix_GroupOldest(1);
    }
    Mix_PlayChannel(channel, what, 0);
}

/***************************************************************************
 *
 * main - SDL requires that main have argc and argv for compatibility
 *
 ***************************************************************************/
int main(int argc, char *argv[])
{
    SDL_Surface *playerpic[1];
    double d, a;                        /* Used for re-scaling the players speed */
    SDL_Event event;                    /* Handles event loop */
    Uint8 *keystate;                    /* Used for reading multiple keys */
    int i;                              /* Loop counter */
    char dummy;
    int test;                           /* Test for errors */
    int credit_click;
    int start_click;
    int pause_click;
    int fs_click;
    int dochannel;                      /* Which channel to play the sound */
    SDL_TimerID refresh;                /* Timer to redraw the screen each frame */

#ifdef WIN32
    srand(time(NULL));                  /* Random number initiation */
#endif
#ifndef WIN32
    srandom(time(NULL));
#endif

    /* Init some variables */
    credits = 0;
    screentimer = 0;
    screenmode = 0;
    /* Initialize defaults, Video and Audio */
    if ((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) == -1))
    {
        printf("Could not initialize SDL: %s.\n", SDL_GetError());
        exit(-1);
    }
    test = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 512);
    if (test == -1)
    {
        printf("Mix_OpenAudio error\n");
        printf("Oh My Goodness, an error : %s", Mix_GetError());

        exit(1);
    }
    /* Clean up on exit */

    atexit(SDL_Quit);
    atexit(Mix_CloseAudio);
    atexit(write_HISCORE);

    /*
     * Set the palette.  If you don't like the colors in the palette, you can
     * change them here.  These values will override the values in the XPMs.
     * On some monitors, color 2 (the background color) might show up as a
     * greenish.  Feel free to change it if you want.  In general, you should
     * change color 0 to something very close to prevent a noticable flast when
     * the game starts.  Colors beyonf 8 are all for the twig image.
     */
    colors[0].r = 154;
    colors[0].g = 154;
    colors[0].b = 154;
    colors[1].r = 0;
    colors[1].g = 208;
    colors[1].b = 0;
    colors[2].r = 50;
    colors[2].g = 50;
    colors[2].b = 50;
    colors[3].r = 0;
    colors[3].g = 128;
    colors[3].b = 255;
    colors[4].r = 0;
    colors[4].g = 0;
    colors[4].b = 0;
    colors[5].r = 255;
    colors[5].g = 240;
    colors[5].b = 0;
    colors[6].r = 128;
    colors[6].g = 128;
    colors[6].b = 128;
    colors[7].r = 255;
    colors[7].g = 255;
    colors[7].b = 255;
    colors[8].r = 128;
    colors[8].g = 0;
    colors[8].b = 0;
    colors[9].r = 236;
    colors[9].g = 178;
    colors[9].b = 151;
    colors[10].r = 248;
    colors[10].g = 178;
    colors[10].b = 151;
    colors[11].r = 239;
    colors[11].g = 176;
    colors[11].b = 141;
    colors[12].r = 215;
    colors[12].g = 160;
    colors[12].b = 142;
    colors[13].r = 157;
    colors[13].g = 118;
    colors[13].b = 102;
    colors[14].r = 213;
    colors[14].g = 159;
    colors[14].b = 127;
    colors[15].r = 190;
    colors[15].g = 142;
    colors[15].b = 126;
    colors[16].r = 184;
    colors[16].g = 135;
    colors[16].b = 101;
    colors[17].r = 248;
    colors[17].g = 167;
    colors[17].b = 146;
    colors[18].r = 224;
    colors[18].g = 148;
    colors[18].b = 132;
    colors[19].r = 173;
    colors[19].g = 118;
    colors[19].b = 106;
    colors[20].r = 165;
    colors[20].g = 121;
    colors[20].b = 90;
    colors[21].r = 245;
    colors[21].g = 166;
    colors[21].b = 137;
    colors[22].r = 236;
    colors[22].g = 158;
    colors[22].b = 138;
    colors[23].r = 121;
    colors[23].g = 81;
    colors[23].b = 63;
    colors[24].r = 158;
    colors[24].g = 121;
    colors[24].b = 94;
    colors[25].r = 225;
    colors[25].g = 157;
    colors[25].b = 129;
    colors[26].r = 235;
    colors[26].g = 155;
    colors[26].b = 127;
    colors[27].r = 237;
    colors[27].g = 150;
    colors[27].b = 132;
    colors[28].r = 135;
    colors[28].g = 105;
    colors[28].b = 77;
    colors[29].r = 174;
    colors[29].g = 125;
    colors[29].b = 102;
    colors[30].r = 186;
    colors[30].g = 139;
    colors[30].b = 114;
    colors[31].r = 206;
    colors[31].g = 131;
    colors[31].b = 111;
    colors[32].r = 187;
    colors[32].g = 105;
    colors[32].b = 84;
    colors[33].r = 198;
    colors[33].g = 107;
    colors[33].b = 87;
    colors[34].r = 219;
    colors[34].g = 127;
    colors[34].b = 111;
    colors[35].r = 215;
    colors[35].g = 140;
    colors[35].b = 124;
    colors[36].r = 145;
    colors[36].g = 104;
    colors[36].b = 82;
    colors[37].r = 161;
    colors[37].g = 112;
    colors[37].b = 89;
    colors[38].r = 194;
    colors[38].g = 132;
    colors[38].b = 111;
    colors[39].r = 205;
    colors[39].g = 141;
    colors[39].b = 119;
    colors[40].r = 216;
    colors[40].g = 131;
    colors[40].b = 113;
    colors[41].r = 198;
    colors[41].g = 115;
    colors[41].b = 97;
    colors[42].r = 178;
    colors[42].g = 93;
    colors[42].b = 73;
    colors[43].r = 181;
    colors[43].g = 108;
    colors[43].b = 88;
    colors[44].r = 146;
    colors[44].g = 83;
    colors[44].b = 64;
    colors[45].r = 192;
    colors[45].g = 118;
    colors[45].b = 100;
    colors[46].r = 151;
    colors[46].g = 103;
    colors[46].b = 82;
    colors[47].r = 164;
    colors[47].g = 98;
    colors[47].b = 79;
    colors[48].r = 219;
    colors[48].g = 154;
    colors[48].b = 135;
    colors[49].r = 217;
    colors[49].g = 141;
    colors[49].b = 119;
    colors[50].r = 214;
    colors[50].g = 140;
    colors[50].b = 111;
    colors[51].r = 176;
    colors[51].g = 119;
    colors[51].b = 98;
    colors[52].r = 224;
    colors[52].g = 147;
    colors[52].b = 125;

    read_CONFIG();                      /* Read custom keylayout, and whether to start FS or windowed */

    /* Init main screen; */

    /* Setup screen background */
    screen = SDL_SetVideoMode(640, 480, 8, fsmode);
    if (screen == NULL)
    {
        fprintf(stderr, "Couldn't set 640x480x8 video mode: %s\n", SDL_GetError());
        exit(1);
    }
    setup_image(screen);

    SDL_WM_SetCaption("Twig's In Space", "Twig's in Space");

    /* So that we don't try to draw to the screen while we are changing things */
    Lockscreen = SDL_CreateMutex();

    /* Read in the master images */
    enemypics[0] = IMG_ReadXPMFromArray(enemy1_xpm);
    setup_image(enemypics[0]);
    enemypics[1] = IMG_ReadXPMFromArray(enemy2_xpm);
    setup_image(enemypics[1]);
    enemypics[2] = IMG_ReadXPMFromArray(enemy3_xpm);
    setup_image(enemypics[2]);
    enemypics[3] = IMG_ReadXPMFromArray(enemy4_xpm);
    setup_image(enemypics[3]);
    playerpic[0] = IMG_ReadXPMFromArray(ship1_xpm);
    setup_image(playerpic[0]);
    /* Initialize player position */
    setup_player(playerpic[0]);

    /* Read in and setup other xpm images for explosions, numbers, and labels */
    explode = IMG_ReadXPMFromArray(explode_xpm);
    setup_image(explode);
    digits = IMG_ReadXPMFromArray(digits_xpm);
    setup_image(digits);
    words = IMG_ReadXPMFromArray(words_xpm);
    setup_image(words);
    letters = IMG_ReadXPMFromArray(letters_xpm);
    setup_image(letters);
    logo = IMG_ReadXPMFromArray(Logo_xpm);
    setup_image(logo);
    hiscore = SDL_CreateRGBSurface(SDL_SWSURFACE, 120, 84, 8, 0, 0, 0, 0);
    setup_image(hiscore);
    newscore = SDL_CreateRGBSurface(SDL_SWSURFACE, 200, 84, 8, 0, 0, 0, 0);
    setup_image(newscore);

    /* Set up enemys */
    curr_level = 0;
    populate_board(curr_level);
    /* Initialize shots */
    for (i = 0; i < 20; i++)
    {
        shot[i].images = explode;
        shot[i].status = 0;
        shot[i].rect.w = 3;
        shot[i].rect.h = 3;
        shot[i].rect.x = 80;
        shot[i].rect.y = 30;
        shot[i].mask.x = 80;
        shot[i].mask.y = 70;
        shot[i].mask.w = 3;
        shot[i].mask.h = 3;
    }
    /* Digits for score counter! */
    for (i = 0; i < 10; i++)
    {
        drect[i].x = i * 20;
        drect[i].y = 0;
        drect[i].w = 20;
        drect[i].h = 23;
    }
    /* Letters for High score board */
    for (i = 0; i < 37; i++)
    {
        lrect[i].x = i * 10;
        lrect[i].y = 0;
        lrect[i].w = 10;
        lrect[i].h = 12;
    }
    /* Set up word labels */
    wrect[0].x = 0;
    wrect[0].y = 0;
    wrect[0].w = 121;
    wrect[0].h = 15;                                             /* INSERT COIN */
    wrect[1].x = 0;
    wrect[1].y = 20;
    wrect[1].w = 97;
    wrect[1].h = 15;                    /* GAME OVER */
    wrect[2].x = 0;
    wrect[2].y = 40;
    wrect[2].w = 36;
    wrect[2].h = 10;                    /* LEVEL */
    wrect[3].x = 40;
    wrect[3].y = 40;
    wrect[3].w = 33;
    wrect[3].h = 10;                    /* SCORE */
    wrect[4].x = 80;
    wrect[4].y = 40;
    wrect[4].w = 47;
    wrect[4].h = 10;                    /* CREDITS */
    wrect[5].x = 130;
    wrect[5].y = 40;
    wrect[5].w = 41;
    wrect[5].h = 10;                    /* SHIELD */
    wrect[6].x = 0;
    wrect[6].y = 50;
    wrect[6].w = 130;
    wrect[6].h = 20;                    /* Copyright */

    /* Read in highscore file, if there is one. */
    read_HISCORE();

    /* Setup base screen */
    blankscreen();
    SDL_UpdateRect(screen, 0, 0, 0, 0);

    printf("Startup flags are %x\n", screen->flags);
    /* Set up rules teaser screen */
    setup_rules();

    /*
     * All sound samples except lasershot.wav downloaded from www.flashkit.com
     * Original filenames are used in case you want to track them down.  Note
     * that in some cases, the .wav file name is not quite the same as the
     * the zip file name.  But, most of them are close enough that you can
     * easily do an author search or keyword search.
     */

    /*
     * lasershot.wav downloaded from www.a1freesounds.com, and hand-shortened
     * using TAON under Linux.  The original had a echo effect that was too
     * long for use in this application.
     */

    /* Sound setup */
    sounds[pshot] = Mix_LoadWAV("./audio/lasershot.wav");
    sounds[twigshot] = Mix_LoadWAV("./audio/SineySpa-Mark_E_B-708.wav");
    sounds[playerexplode] = Mix_LoadWAV("./audio/Explode-eRco_Inc-686.wav");
    sounds[coinin] = Mix_LoadWAV("./audio/Robotic_-Darren_E-7447.wav");
    sounds[shielddown] = Mix_LoadWAV("./audio/Whish-Public_D-75.wav");
    sounds[startgame] = Mix_LoadWAV("./audio/chanting-Munkus_M-936.wav");
    sounds[enemyexplode] = Mix_LoadWAV("./audio/Whoosh-Public_D-76.wav");
    sounds[thrust] = Mix_LoadWAV("./audio/Slide-Mr_Still-425.wav");

    /* Music files also downloaded from www.flashkit.com */
    musics[bg_music] = Mix_LoadMUS("./audio/BitchByt-Tucibat-3245.wav");
    musics[hiscore_music] = Mix_LoadMUS("./audio/Boss_1_T-miqal-8855.wav");

    /*
     * The laser sound is way too loud relative to the others, so really silence
     * it.  Otherwise it drowns out everything.  A better solution might be
     * to edit the sound with a waveform editor, but this is easier.
     */
    Mix_VolumeChunk(sounds[pshot], 32);

    Mix_VolumeChunk(sounds[enemyexplode], 64);

    /* Channel setup */
    Mix_AllocateChannels(NUM_CHANNELS);
    Mix_ReserveChannels(NUM_CHANNELS);
    Mix_GroupChannels(0, 15, 1);        /* All channels in one group! */

    /* Disable keyboard repeat.  We are using state polling */
    SDL_EnableKeyRepeat(0, 0);

    /* Add timer to redraw screen */
    refresh = SDL_AddTimer(30, Drawscreen, enemys);

    /*
     * Clicks are used to simulate a full keypress, using the state array, all
     * We get is an up or down status.  The click vars record a previous down,
     * Which is then matched to an up for a keypress.
     */
    start_click = 0;
    credit_click = 0;
    pause_click = 0;
    fs_click = 0;
    player.status = 0;
    /****************************************************************************
     *
     * Main game loop starts here
     *
     ***************************************************************************/
    /* Main game loop */
    while (1)
    {
        /* Time to record a new highscore! */
        if (screenmode == -1)
        {
            new_HISCORE(player.points);
        }
        /* This level is complete, start next one! */
        if ((left == 0) && (player.status == 1))
        {
            do_sounds(sounds[startgame]);
            SDL_LockMutex(Lockscreen);
            curr_level++;
            populate_board(curr_level);
            setup_player(playerpic[0]);
            player.shield += 10;
            if (player.shield > 100)
            {
                player.shield = 100;
            }
            blankscreen();
            SDL_UnlockMutex(Lockscreen);
        }
        SDL_Delay(100);                 /* Make sure we don't get keystrokes too fast */

        SDL_PumpEvents();
        keystate = SDL_GetKeyState(NULL);
        /* Keystroke for pause game! */
        if (keystate[keys[PAUSEKEY]])
        {
            pause_click = 1;
        }
        if ((pause_click == 1) && (!keystate[keys[PAUSEKEY]]))
        {
            /* It's pause mode.  Stop moving the enemys! */
            SDL_RemoveTimer(refresh);
            pause_click = 0;
            while (pause_click != 2)
            {
                SDL_PumpEvents();
                keystate = SDL_GetKeyState(NULL);
                if (keystate[keys[PAUSEKEY]])
                {
                    pause_click = 1;
                }
                if ((pause_click == 1) && (!keystate[keys[PAUSEKEY]]))
                {
                    pause_click = 2;
                }
                if (keystate[keys[QUITKEY]])
                {
                    exit(0);
                }
                if (keystate[keys[FSKEY]])
                {
                    fs_click = 1;
                }
                if ((fs_click == 1) && (!keystate[keys[FSKEY]]))
                {
                    fsmode = fsmode ^ SDL_FULLSCREEN;
                    SDL_LockMutex(Lockscreen);
                    screen = SDL_SetVideoMode(640, 480, 8, fsmode);
                    setup_image(screen);
                    blankscreen();
                    SDL_UnlockMutex(Lockscreen);
                    fs_click = 0;
                }
                /* Case of pause then QUIT */
                if (SDL_PollEvent(&event) > 0)
                {
                    switch (event.type)
                    {
                    case SDL_QUIT:
                        printf("Quit requested, quitting.\n");
                        exit(0);
                    }
                }
            }
            pause_click = 0;
            /* Green light, go! */
            refresh = SDL_AddTimer(30, Drawscreen, enemys);
        }
        /* Keystroke for insert coin */
        if (keystate[keys[COINKEY]])
        {
            credit_click = 1;
        }
        if ((!keystate[keys[COINKEY]]) && (credit_click == 1))
        {
            do_sounds(sounds[coinin]);
            credits++;
            credit_click = 0;
        }
        /* Keystroke for Player 1 Start */
        if ((keystate[keys[STARTKEY]]) && (player.status == 0) && (credits > 0) && (screenmode != -1))
        {
            start_click = 1;
        }
        if ((!keystate[keys[STARTKEY]]) && (start_click == 1) && (player.status == 0) && (credits > 0))
        {
            Mix_PlayMusic(musics[bg_music], -1);
            do_sounds(sounds[startgame]);
            start_click = 0;
            credits--;
            player.status = 1;
            player.shield = 100;
            player.points = 0;
            SDL_LockMutex(Lockscreen);
            curr_level = 1;
            /* Redraw and setup new screen */
            populate_board(curr_level);
            setup_player(playerpic[0]);
            blankscreen();

            SDL_UnlockMutex(Lockscreen);
        }
        /* Keys only work when the user is playing! */
        if (player.status == 1)
        {
            /* Keyup of Thrust Key (Stop drawing engines!) */
            if (!keystate[keys[UPKEY]])
            {
                player.rect.y = 0;
            }
            /* Keydown of Thrust Key (Start drawing engines and accelerate) */
            if (keystate[keys[UPKEY]])
            {
                do_sounds(sounds[thrust]);
                player.rect.y = 40;
                player.mask.y = 120;

                player.xs += pxs[player.facing];
                player.ys += pys[player.facing];
                d = sqrt(player.xs * player.xs + player.ys * player.ys);
                if (d > 5)
                {
                    if (player.xs == 0)
                    {
                        if (player.ys > 0)
                        {
                            player.ys = 5.0;
                        }
                        else
                        {
                            player.ys = -5.0;
                        }
                    }
                    else
                    {
                        if (player.ys == 0)
                        {
                            a = atan2(player.ys, player.xs);
                        }
                        else
                        {
                            a = atan2(-player.ys, player.xs);
                        }
                        player.xs = 5.0 * cos(a);
                        player.ys = -5.0 * sin(a);
                    }
                }
            }
            /* Keydown of Right (rotate right) */
            if (keystate[keys[RIGHTKEY]])
            {
                SDL_LockMutex(Lockscreen);
                player.facing += 1;
                if (player.facing == 16)
                {
                    player.facing = 0;
                }
                player.rect.x = player.facing * 40 + 1;
                SDL_UnlockMutex(Lockscreen);
            }
            /* Keydown of Left (rotate left) */
            if (keystate[keys[LEFTKEY]])
            {
                SDL_LockMutex(Lockscreen);
                player.facing -= 1;
                if (player.facing == -1)
                {
                    player.facing = 15;
                }
                player.rect.x = player.facing * 40 + 1;
                SDL_UnlockMutex(Lockscreen);
            }
            /* The player fired a shot */
            if (keystate[keys[FIREKEY]])
            {
                i = 0;
                while ((shot[i].status == 1) && (i < 20))
                {
                    i++;
                }
                if (i < 20)   /* Otherwise, too many shots! */
                {
                    do_sounds(sounds[pshot]);
                    shot[i].status = 1;
                    shot[i].x = player.x + 20 + (25 * pxs[player.facing]);
                    shot[i].y = player.y + 20 + (25 * pys[player.facing]);
                    shot[i].xs = 10 * pxs[player.facing];
                    shot[i].ys = 10 * pys[player.facing];
                    player.points -= 1;
                    if (player.points < 0)
                    {
                        player.points = 0;
                    }
                    /* If shot starts out of bounds, cancel it. */
                    if ((shot[i].x <= 1) || (shot[i].y <= 1) || (shot[i].x >= WIDTH - 2) || (shot[i].y >= HEIGHT - 2))
                    {
                        shot[i].status = 0;
                    }
                }
            }
        }                               /* If player.status == 1 */
        if (keystate[keys[QUITKEY]])
        {
            exit(0);
        }
        if (keystate[keys[FSKEY]])
        {
            fs_click = 1;
        }
        if ((fs_click == 1) && (!keystate[keys[FSKEY]]))
        {
            fsmode = fsmode ^ SDL_FULLSCREEN;

            SDL_LockMutex(Lockscreen);

            screen = SDL_SetVideoMode(640, 480, 8, fsmode);

            setup_image(screen);

            blankscreen();
            SDL_UpdateRect(screen, 0, 0, 0, 0);
            SDL_UnlockMutex(Lockscreen);
            fs_click = 0;
        }
        /* Check for other events */
        if (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            }
        }
    }                                   /* While loop */

    exit(0);
}
