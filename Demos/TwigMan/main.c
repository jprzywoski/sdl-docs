/* TwigMan - An SDL demonstration for CSCI 321.
 * Version 0.9.0
 * Written by Dr. Evan L. Schemm
 *            Lake Superior State University
 *            School of Mathematics and Computer Science
 *
 * Please send questions, comments, suggestions or (gasp!) bugfixes to:
 *        elschemm@furball.schemm.lssu.edu
 *
 * Copyright 2007 by LSSU School of MA/CS
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
 * TwigMan is the third demonstration program produced as a talking point for
 * CSCI 321, Computer Graphics, at Lake Superior State University.
 *
 * TwigMan started as a fairly simple jumping game meant to demonstate gravity
 * and platform games.  Because of chemicals released into the building air
 * ducts, it morphed into this.
 *
 * The goal of TwigMan is for Twig to re-collect the cable testers at the
 * end of CSCI 221 (Computer Networking).  Sadly, over the course of the years,
 * a number of them have 'disappeared'.  Interestingly, the students do have
 * a remarkable tendency to leave their textbooks in the lab.  And of course,
 * Dr. Terwilliger is never far from his beverage of choice.  I'm still not
 * entirely certain how he convinced the University to move the vending machine
 * outside the lab.
 *
 * The standard keys for TwigMan are as follows:
 * 5 - Insert Coin (MAME standard)
 * 1 - Player 1 start (MAME standard)
 * Left - Move left
 * Right - Move right
 * Up - Jump
 * F11 - Toggle Fullscreen
 * P - Pause (only during normal gameplay)
 *
 * These keys can be re-mapped to any keys on the keyboard by changing the
 * default values in the file 'twigman.conf'.
 *
 *
 * With TwigPower, TwigMan can double (and triple, ...) jump.  He is also immune
 * to the FabbriCar and Falling Boger.
 *
 * You have to land on the students head to squash them (and get the points).
 * No, you don't get any cable testers they stole back (already sold on ebay).
 *
 * Every five levels, there is an intermission.  The intermissions chronicle
 * the arrival of the four CS faculty in the MA/CS department.
 *
 * There are six different level patterns, the seventh level is an identical
 * board to the first, and so on.
 *
 * Level 31 is the last level, when you complete it, you restart level 31 again.
 * You will be treated to a random intermission each time that you complete
 * level 31 (hey... it's not easy to get that far!  You deserve a reward!).
 *
 * It is possible to insert additional coins and continue from the spot that
 * you ran out of lives.  You have 9 seconds to insert additional coins and
 * press Player 1 start to do this.  It is useful if you are just short of the
 * next intermission.
 *
 * This game contains dozens, if not 100 inside jokes to the department.  If
 * you really want to know what they all mean, come up for a visit.  You can
 * find us in Sault Ste. Marie, MI!
 *
 * This game has been successfully compiled on:
 *     Linux (using gcc)
 *     Windows (cygwin gcc)
 *     Windows (Dev-Cpp)
 *     Solaris (using gcc)
 *     Mac OS X (using gcc)
 *
 * If you successfully compile it for some other platform, please let us know!
 *
 *
 * See the other SDL demonstrations at
 * http://furball.schemm.lssu.edu/SDL
 *
 */

#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>

/* Cygwin does not have random */
#ifdef WIN32
#define RAND	rand()
#endif
#ifndef WIN32
#define RAND	random()
#endif

/*
 * Need to set up the different seed random functions too.
 * Since demo_play needs a constant seed value, we can't do all
 * the function stuff here, all we can do is the function name.
 */
#ifdef WIN32
#define SRAND	srand
#endif
#ifndef WIN32
#define SRAND	srandom
#endif

/*
 * Hold all the data for one student.  Going to make an array of these for
 * all the students on the screen
 */
struct STUDENT
{
    int x;
    int y;
    int face;
    int active;
};

typedef struct STUDENT Student;

/*
 * screen_flags is primarily a global variable so that we can switch between
 * FullScreen and Windowed at will.
 */
int screen_flags;
SDL_Surface *screen;
SDL_Surface *player;
SDL_Surface *brick;
SDL_Surface *book;
SDL_Surface *crimp;
SDL_Surface *cola;
SDL_Surface *cable;
SDL_Surface *fabbri;
SDL_Surface *student;
SDL_Surface *boger;
SDL_Surface *roof;
SDL_Surface *logo;
SDL_Surface *levelglyph;
SDL_Surface *BigBoger;
SDL_Surface *ibm_360;
SDL_Surface *rock;
SDL_Surface *crater;
SDL_Surface *dino1;
SDL_Surface *dino2;
SDL_Surface *dino3;
SDL_Surface *road;
SDL_Surface *signs;
SDL_Surface *house;
SDL_Surface *trucks;
SDL_Surface *misc;
SDL_Surface *BigTwig;
SDL_Surface *BigSchemm;
SDL_Surface *BigFabbri;
SDL_Surface *movers;
SDL_Surface *cats;
SDL_Surface *caset;
SDL_Surface *zombie;
SDL_Surface *eng;
TTF_Font *font;
TTF_Font *bigfont;

#define WIDTH	600
#define HEIGHT	450
#define OFFSET	30

double ypos;                            /* The players y pos (will be rounded for screen work) */
double yvel;                            /* The players y velocity */
int xpos;                               /* The players current x position */
int xface;                              /* Whether the player is facing left or right */
int jump;                               /* Whether the player is jumping or not */
int twigpower;                          /* Whether or not Twig has SuperJump */
int fypos;                              /* Fabbri position */
int fyvel;
int fxpos;
int fxface;
int fjump;
int fstatus;
Student students[20];                   /* Student data */
int bogerx;                             /* Boger data */
int bogery;
int bogerstat;
int bogerhouse;
int coins;                              /* Number of coins in reserve */

#define GRAV	3.0                     /* Physics constants for TwigMan */
#define JUMP	-25.0
#define TERM	15

int p1score;                            /* Game data */
int newlife;
int hiscore;
int cableleft;
int lives;
int level;
int tick;                               /* Used for system tick for animations. */
int teaser_tick;

/*
 * Sound info.  Symbolic sound names make it slightly easier to read the
 * code.
 */
Mix_Chunk *sounds[25];

#define endgame		0
#define evil_laugh	1
#define appear		2
#define get_tester	3
#define money		4
#define can_open	5
#define steal		6
#define jumpping	7
#define squish		8
#define falling		9
#define death		10
#define bookdrop	11
#define gamestart	12
#define get_book	13
#define drive		14
#define intermission	15
#define crash		16
#define thud		17
#define flower		18
#define meow		19
#define purr		20
#define gun		21
#define roar		22
#define life		23
#define NUM_CHANNELS	16

Mix_Music *musics[2];

#define bg		0
#define inter1		1

void do_sounds(Mix_Chunk *what);
void read_CONFIG();

int keys[10];                           /* Allow user defineable keys */

#define JUMPKEY		0
#define LEFTKEY		1
#define RIGHTKEY	2
#define PAUSEKEY	3
#define COINKEY		4
#define STARTKEY	5
#define QUITKEY		6
#define FSKEY		7

/*
 * This is the background map.  I did it like this for two reasons.
 *   1. I wanted to be able to change it without firing up the GIMP.
 *   2. I wanted an easy way to check whether the player was standing on
 *      a block or air.
 */

int map[15][20];                        /* This holds the current map */

/* These are the master start maps for each level */
int level1[15][20] =
{
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0
    }, {
        2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 3, 0, 0, 0, 0, 0
    }, {
        1, 1, 1,
        1, 0, 0,
        0, 0, 0,
        0, 0, 0,
        1, 1, 1,
        1, 1, 1,
        0, 0
    },
    {
        0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,
        4
    },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, {
        0, 4, 0, 0, 0, 0, 0, 0, 1, 1, 1, 4,
        0,
        0, 0, 0, 0, 0, 0, 0
    }, {
        1, 1, 0, 0, 0, 1, 0, 0,
        0, 0, 0, 1, 1, 0, 0, 0,
        0, 1, 1, 1
    }, {
        0, 0, 0,
        0, 0, 0,
        0,
        0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0,
        0
    }, {
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        2,
        0,
        0,
        0,
        0,
        0,
        0
    },
    { 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0 }, {
        2, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
        0, 0, 0
    }, {
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    }, {
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 4, 0,
        0, 0
    },
    {
        1, 1, 1, 1, 1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1, 1, 1, 1, 1, 1, 1, 1
    }
};
int level2[15][20] =
{
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0
    }, {
        0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 3, 0, 0, 0, 0, 0, 0
    }, {
        0, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 1, 0,
        0, 0
    },
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,
        4
    },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 }, {
        0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0,
        0,
        0, 0, 0, 1, 1, 0, 0
    }, {
        0, 4, 0, 0, 0, 0, 1, 1,
        1, 0, 0, 0, 0, 0, 1, 1,
        0, 0, 0, 0
    }, {
        1, 1, 1,
        1, 0, 0,
        0,
        0,
        0, 0, 0,
        4, 0, 0,
        0, 0, 0,
        0, 0,
        0
    }, {
        0,
        0,
        0,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        0,
        0,
        0,
        0
    },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0 }, {
        0, 3, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1,
        1, 1, 1
    }, {
        0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    }, {
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 2,
        0, 0, 0,
        0, 0, 0,
        4, 0
    },
    {
        1, 1, 1, 1, 1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1, 1, 1, 1, 1, 1, 1, 1
    }
};
int level3[15][20] =
{
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {
        0, 4, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0,
        0, 4, 0
    }, {
        1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1,
        0, 0, 1, 1, 0, 0, 1, 1
    }, {
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0
    },
    {
        0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 0, 0, 0, 0,
        0,
        0
    },
    { 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1 }, {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,
        0, 0, 0, 0, 0, 0, 0
    }, {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0
    }, {
        1, 1, 0,
        0, 1, 1,
        0,
        0,
        1, 1, 1,
        1, 0, 0,
        1, 1, 0,
        0, 1,
        1
    }, {
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0
    },
    { 3, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 3 }, {
        1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0,
        0, 1, 1
    }, {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    }, {
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0
    },
    {
        1, 1, 1, 1, 1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1, 1, 1, 1, 1, 1, 1, 1
    }
};
int level4[15][20] =
{
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0
    }, {
        0, 4, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    }, {
        1, 1, 0,
        0, 0, 0,
        0, 0, 1,
        1, 0, 0,
        0, 0, 0,
        0, 0, 0,
        1, 1
    },
    {
        0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 4,
        0,
        0
    },
    { 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1 }, {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1,
        0, 0, 0, 0, 0, 0, 0
    }, {
        0, 0, 0, 0, 0, 0, 0, 0,
        3, 0, 0, 3, 0, 0, 0, 0,
        0, 0, 0, 0
    }, {
        0, 0, 1,
        1, 1, 1,
        1,
        1,
        1, 0, 0,
        1, 1, 1,
        1, 1, 1,
        1, 0,
        0
    }, {
        0,
        1,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        1,
        2
    },
    { 1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }, {
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 0
    }, {
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1
    }, {
        0, 0, 0,
        0, 0, 0,
        3, 0, 0,
        0, 0, 0,
        0, 0, 0,
        2, 0, 0,
        0, 0
    },
    {
        1, 1, 1, 1, 1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1, 1, 1, 1, 1, 1, 1, 1
    }
};
int level5[15][20] =
{
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {
        0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0,
        0, 0, 0
    }, {
        0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0,
        1, 0, 1, 0, 1, 0, 1, 0
    }, {
        0, 0, 0,
        0, 0, 0,
        0, 0, 3,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0
    },
    {
        1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1,
        0,
        1
    },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0 }, {
        0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0,
        1,
        0, 1, 0, 1, 0, 1, 0
    }, {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 2
    }, {
        1, 0, 1,
        0, 1, 0,
        1,
        0,
        1, 0, 0,
        1, 0, 1,
        0, 1, 0,
        1, 0,
        1
    }, {
        0,
        2,
        0,
        0,
        0,
        4,
        0,
        0,
        0,
        0,
        0,
        0,
        2,
        0,
        0,
        0,
        0,
        0,
        0,
        0
    },
    { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 }, {
        0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0
    }, {
        1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1,
        0, 1, 0, 1, 0, 1, 0, 1
    }, {
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        4, 0
    },
    {
        1, 1, 1, 1, 1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1, 1, 1, 1, 1, 1, 1, 1
    }
};
int level6[15][20] =
{
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, {
        4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0,
        0, 0, 3
    }, {
        1, 1, 1, 1, 1, 1, 0, 0, 0, 3, 0, 0,
        0, 0, 1, 1, 1, 1, 1, 1
    }, {
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        1, 1, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0
    },
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,
        0
    },
    { 0, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4 }, {
        1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
        0,
        0, 1, 1, 1, 1, 1, 1
    }, {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0
    }, {
        0, 0, 0,
        0, 0, 0,
        0,
        0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0,
        0
    }, {
        0,
        0,
        0,
        2,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        4,
        0,
        0,
        0
    },
    { 1, 1, 1, 1, 1, 1, 0, 0, 0, 3, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 }, {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 0
    }, {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    }, {
        0, 0, 0,
        0, 0, 0,
        2, 0, 0,
        0, 0, 0,
        0, 2, 0,
        0, 0, 0,
        0, 0
    },
    {
        1, 1, 1, 1, 1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1, 1, 1, 1, 1, 1, 1, 1
    }
};

/*
 * This is the matrix for the demo play mode.  The numbers correspond
 * to keystrokes that TwigMan gets.  Demo_play is always run off the same
 * Random seed, so the same set of actions yields the same play each time.
 * This set of actions gives a fairly varied show of what the gameplay is
 * like.  The demo_play mode lasts 18 seconds.
 * Left = 1, Right = 2, Up = 4
 */
int demo_play[360] =
{
    0, 0, 0, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 6, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 6,
    6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 6, 2, 2, 2, 2, 2, 2, 2, 2
};

/*
 * If you are using XPM's, you need to include the file as part of the code.
 * Note that with most graphics programs the name of the array is similar to,
 * but not identical to the name of the XPM inside the file.
 */

#include "images/NewerTwig3.xpm"
#include "images/book.xpm"
#include "images/fabbri.xpm"
#include "images/cola.xpm"
#include "images/cable.xpm"
#include "images/man.xpm"
#include "images/boger30.xpm"
#include "images/roof.xpm"
#include "images/twigman_logo.xpm"
#include "images/level.xpm"
#include "images/grider.xpm"

/* These are extra xpms for the intermissions. */
#include "inter_graphics/BigBoger2.xpm"
#include "inter_graphics/ibm360_2.xpm"
#include "inter_graphics/rock.xpm"
#include "inter_graphics/crater.xpm"
#include "inter_graphics/dino1.xpm"
#include "inter_graphics/dino2.xpm"
#include "inter_graphics/dino3.xpm"
#include "inter_graphics/road.xpm"
#include "inter_graphics/signs.xpm"
#include "inter_graphics/house.xpm"
#include "inter_graphics/trucks.xpm"
#include "inter_graphics/misc.xpm"
#include "inter_graphics/BigTwig2.xpm"
#include "inter_graphics/BigSchemm2.xpm"
#include "inter_graphics/mover.xpm"
#include "inter_graphics/cats.xpm"
#include "inter_graphics/BigFabbri2.xpm"
#include "inter_graphics/caset.xpm"
#include "inter_graphics/zombie.xpm"
#include "inter_graphics/eng.xpm"

#include "Fixed_print.c"

/*
 * xprint is a function for printing strings on the screen.  It is very
 * useful in conjunction with sprintf for printing anything on the screen!
 */

void xprint(char *str, TTF_Font *font, int x, int y)
{
    SDL_Surface *temp;
    SDL_Rect loc;
    SDL_Color color = { 255, 255, 255 };        /* You can change this to another color */
    int w, h;

    temp = TTF_RenderText_Solid(font, str, color);

    loc.x = x;
    loc.y = y;
    TTF_SizeText(font, str, &w, &h);
    loc.w = w;
    loc.h = h;

    SDL_BlitSurface(temp, NULL, screen, &loc);

    SDL_FreeSurface(temp);
}

/*
 * This is a function that I wrote just after Twigs in Space.  It provides a
 * program independent way to set the screen size. Note that screen_flags is
 * a global variable that can be changed before calling this function.  It
 * is quite possible to re-call SetVideoMode in the middle of your game to
 * resize the screen.  Of course, you lose everything that was on the old
 * screen when you do (you will need to completely redraw it).
 */
void change_screen(int x, int y)
{
    screen = SDL_SetVideoMode(x, y, 32, screen_flags);
    SDL_SetColorKey(screen, SDL_SRCCOLORKEY, 0xFE00FE);
    if (screen == NULL)
    {
        fprintf(stderr, "Error changing screen mode: %s\n", SDL_GetError());
        exit(1);
    }
}

/*
 * I got tired of this cluttering up main, so I made it a separate function.
 * You will find versions of this in most of my SDL programs.  They vary
 * slightly depending on what other libraries I am using, and other
 * pedestrian inits I am doing.
 */
void init_SDL_stuff()
{
    int test;
    Uint16 format;

    if ((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) == -1))
    {
        printf("CHEESE\n");
        printf("Could not initialize SDL: %s.\n", SDL_GetError());
        exit(-1);
    }
    TTF_Init();

    test = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024);
    if (test == -1)
    {
        printf("Mix_OpenAudio error\n");
        printf("Oh My Goodness, an error : %s", Mix_GetError());

        exit(1);
    }
    /* Clean up on exit */
    atexit(Mix_CloseAudio);
    atexit(TTF_Quit);
    atexit(SDL_Quit);

    /* Make the initial version of the main screen */
    screen_flags = SDL_SWSURFACE | SDL_SRCCOLORKEY;
    read_CONFIG();
    change_screen(WIDTH, HEIGHT + OFFSET);

    SDL_WM_SetCaption("Twig Man", "Twig Man");

    /* Font downloaded from www.1001fonts.com */
    font = TTF_OpenFont("monofont.ttf", 24);
    bigfont = TTF_OpenFont("monofont.ttf", 72);
    if (!font)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
    }
    FixedPrint_Setup();

    /* Channel setup */
    Mix_AllocateChannels(NUM_CHANNELS);
    Mix_ReserveChannels(NUM_CHANNELS);
    Mix_GroupChannels(0, 15, 1);        /* All channels in one group! */
}

/*
 * Collision detection used for Boger and Fabbri.  A simple circle-circle
 * detection algorithm.  Got rid of the sqrt to speed things up.
 */
int testcollide(int c1x, int c1y, int r1, int c2x, int c2y, int r2)
{
    return (((c2x - c1x) * (c2x - c1x)) + ((c2y - c1y) * (c2y - c1y))) < ((r1 + r2) * (r1 + r2));
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
    keys[JUMPKEY] = SDLK_UP;
    keys[LEFTKEY] = SDLK_LEFT;
    keys[RIGHTKEY] = SDLK_RIGHT;
    keys[PAUSEKEY] = SDLK_p;
    keys[COINKEY] = SDLK_5;
    keys[STARTKEY] = SDLK_1;
    keys[QUITKEY] = SDLK_ESCAPE;
    keys[FSKEY] = SDLK_F11;

    screen_flags = SDL_SWSURFACE | SDL_SRCCOLORKEY;     /* Windowed */

    infile = fopen("twigman.conf", "r");
    /* If there is no config file, use default settings */
    if (infile == NULL)
    {
        return;
    }
    /* Otherwise, read file */
    while (fscanf(infile, "%80s", array) != EOF)
    {
        while (array[0] == '#')   /* Get rid of comment lines */
        {
            if (fscanf(infile, "%80s", array) == EOF)
            {
                fclose(infile);
                return;
            }
        }
        /* Got a mode line */
        if (strcmp(array, "mode") == 0)
        {
            fscanf(infile, "%80s", array);
            if (strcmp(array, "FULLSCREEN") == 0)
            {
                screen_flags = SDL_SWSURFACE | SDL_FULLSCREEN | SDL_SRCCOLORKEY;
            }
            else
            {
                screen_flags = SDL_SWSURFACE | SDL_SRCCOLORKEY;
            }
        }
        /* Got an upkey line */
        if (strcmp(array, "jumpkey") == 0)
        {
            fscanf(infile, "%d", &keys[JUMPKEY]);
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

#include "intermission.c"

/*
 * Code to actually draw the screen.  I usually put this in a separate function
 * so that it does not clutter up main.
 */
int draw_screen(int real)
{
    SDL_Rect rect;
    SDL_Rect pos;
    int x, y;
    int loop;
    char str[100];
    SDL_Color c;

    /*
     * Not the worlds most efficient screen drawing method, but it's fast
     * enough.
     */

    /* Start by blanking the screen */
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

    /* Then draw the griders, books, colas, and testers */
    rect.y = 0;
    rect.w = 30;
    rect.h = 30;
    for (y = 0; y < 15; y++)
    {
        for (x = 0; x < 20; x++)
        {
            rect.x = 0;
            if (map[y][x] == 1)
            {
                pos.x = x * 30;
                pos.y = y * 30 + OFFSET;
                SDL_BlitSurface(brick, &rect, screen, &pos);
            }
            if (map[y][x] == 2)
            {
                pos.x = x * 30;
                pos.y = y * 30 + OFFSET;
                SDL_BlitSurface(book, &rect, screen, &pos);
            }
            if (map[y][x] == 3)
            {
                rect.x = (tick) % 6 * 30;
                pos.x = x * 30;
                pos.y = y * 30 + OFFSET;
                SDL_BlitSurface(cola, &rect, screen, &pos);
            }
            if (map[y][x] == 4)
            {
                rect.x = (tick / 10) % 6 * 30;
                pos.x = x * 30;
                pos.y = y * 30 + OFFSET;
                SDL_BlitSurface(cable, &rect, screen, &pos);
            }
        }
    }
    /* Now draw Fabbri, if he is on the screen. */
    if (fstatus != 0)
    {
        rect.x = 30 * fxface;
        rect.y = 0;
        rect.w = 30;
        rect.h = 30;
        pos.x = fxpos;
        pos.y = (int)fypos + .5 + OFFSET;
        SDL_BlitSurface(fabbri, &rect, screen, &pos);
    }
    /* Now draw any students */
    for (loop = 0; loop < 20; loop++)
    {
        if (students[loop].active == 1)
        {
            rect.x = 20 * (tick % 8);
            rect.y = 20 * students[loop].face;
            rect.w = 20;
            rect.h = 20;
            pos.x = students[loop].x;
            pos.y = students[loop].y + OFFSET;
            SDL_BlitSurface(student, &rect, screen, &pos);
        }
    }
    /* TwigPower bar */
    rect.x = 0;
    rect.y = 19;
    rect.w = twigpower * 3;
    rect.h = 11;
    SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 65, 105, 225));
    sprintf(str, "TwigPower TwigPower TwigPower TwigPower TwigPower TwigPower TwigPower TwigPower TwigPower TwigPower");
    c.r = 0;
    c.g = 0;
    c.b = 0;
    FixedPrint(screen, 0, 20, str, &c);
    /* Print boger */
    if (bogerstat > 0)
    {
        rect.x = 0;
        rect.y = 0;
        rect.w = 25;
        rect.h = 30;
        pos.x = bogerx;
        pos.y = bogery + OFFSET;
        SDL_BlitSurface(boger, &rect, screen, &pos);

        rect.x = 0;
        rect.y = 0;
        rect.w = 100;
        rect.h = 32;
        pos.x = bogerhouse;
        pos.y = 0;
        SDL_BlitSurface(roof, &rect, screen, &pos);
    }
    /* Need to show the level */
    if (level & 1)
    {
        rect.x = 0;
        rect.y = 0;
        rect.w = 20;
        rect.h = 20;
        pos.x = 570;
        pos.y = 425 + OFFSET + 1;
        SDL_BlitSurface(levelglyph, &rect, screen, &pos);
    }
    if (level & 2)
    {
        rect.x = 20;
        rect.y = 0;
        rect.w = 20;
        rect.h = 20;
        pos.x = 540;
        pos.y = 425 + OFFSET + 1;
        SDL_BlitSurface(levelglyph, &rect, screen, &pos);
    }
    if (level & 4)
    {
        rect.x = 40;
        rect.y = 0;
        rect.w = 20;
        rect.h = 20;
        pos.x = 510;
        pos.y = 425 + OFFSET + 1;
        SDL_BlitSurface(levelglyph, &rect, screen, &pos);
    }
    if (level & 8)
    {
        rect.x = 60;
        rect.y = 0;
        rect.w = 20;
        rect.h = 20;
        pos.x = 480;
        pos.y = 425 + OFFSET + 1;
        SDL_BlitSurface(levelglyph, &rect, screen, &pos);
    }
    if (level & 16)
    {
        rect.x = 80;
        rect.y = 0;
        rect.w = 20;
        rect.h = 20;
        pos.x = 450;
        pos.y = 425 + OFFSET + 1;
        SDL_BlitSurface(levelglyph, &rect, screen, &pos);
    }
    /*
     * Now the player
     * rect is the part of the player image that we are going to use.
     * Since the Surface has both a right facing and left facing image,
     * We use rect.x to specify which face to show.  We can do more complicated
     * things with this also.  See: tilemaps.
     * Pos is the location on the screen where it is going to go.  Note: ypos
     * is a float (because of the gravity math), so we need to round then
     * convert to an int.
     */
    rect.x = 30 * xface;
    rect.y = 0;
    rect.w = 30;
    rect.h = 30;
    pos.x = xpos;
    pos.y = (int)ypos + .5 + OFFSET;

    SDL_BlitSurface(player, &rect, screen, &pos);

    /* The score */
    sprintf(str, "P1: %06d", p1score);
    xprint(str, font, 1, -3);

    sprintf(str, "High Score: %06d", hiscore);
    xprint(str, font, WIDTH / 3, -3);

    sprintf(str, "Left: %02d", cableleft);
    xprint(str, font, 2 * WIDTH / 3 + 50, -3);
    /* Need to show the lives remaining */
    if (lives > 1)
    {
        rect.x = 30 * xface;
        rect.y = 0;
        rect.w = 30;
        rect.h = 30;
        pos.x = 5;
        pos.y = 420 + OFFSET + 1;

        SDL_BlitSurface(player, &rect, screen, &pos);
    }
    if (lives > 2)
    {
        rect.x = 30 * xface;
        rect.y = 0;
        rect.w = 30;
        rect.h = 30;
        pos.x = 40;
        pos.y = 420 + OFFSET + 1;

        SDL_BlitSurface(player, &rect, screen, &pos);
    }
    if (lives > 3)
    {
        rect.x = 30 * xface;
        rect.y = 0;
        rect.w = 30;
        rect.h = 30;
        pos.x = 75;
        pos.y = 420 + OFFSET + 1;

        SDL_BlitSurface(player, &rect, screen, &pos);
    }
    if (lives > 4)
    {
        rect.x = 30 * xface;
        rect.y = 0;
        rect.w = 30;
        rect.h = 30;
        pos.x = 110;
        pos.y = 420 + OFFSET + 1;

        SDL_BlitSurface(player, &rect, screen, &pos);
    }
    /* If we are doing the teaser demo */
    if ((real == 0) && ((tick / 10) % 2))
    {
        sprintf(str, "Game Over");
        xprint(str, font, 255, 210);
    }
    /*
     * Don't forget me.
     * Bonus question for those who actually looked at the code:
     * Why don't we do this Update for each SDL_BlitSurface?  Why only at the
     * end of the frame?  Hint: Copy this to the for loop above and you will
     * immediately see why (and it has nothing to do with the speed of the
     * machine that you are on.
     */
    if (real != 2)
    {
        SDL_UpdateRect(screen, 0, 0, 0, 0);
    }
}

/**************************************************************************
 *
 * Death!
 *
 **************************************************************************/
void do_death()
{
    int loop;
    SDL_Event event;

    Mix_HaltMusic();
    do_sounds(sounds[death]);
    for (loop = 0; loop < 180; loop++)
    {
        draw_screen(1);
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                }
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
            }
        }
        SDL_Delay(50);
        tick = (tick + 1) % 2520;
    }
}                                       /* End death subroutine */

/*****************************************************************************
 *
 * End game delay loop - Play end of game music and show game over
 *
 *****************************************************************************/
void end_game()
{
    int loop;
    SDL_Event event;

    for (loop = 0; loop < 100; loop++)
    {
        xprint("Game Over", bigfont, 175, 200);
        SDL_UpdateRect(screen, 0, 0, 0, 0);
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                }
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
            }
        }
        SDL_Delay(50);
    }
}

/**************************************************************************
 *
 * End of Game?
 *
 **************************************************************************/
int do_end_of_game()
{
    int loop;
    SDL_Event event;
    char str[100];
    int count;
    int mode;

    /* Endgame! */
    do_sounds(sounds[endgame]);
    end_game();

    mode = (coins > 0); /*
	                 * Hey look! I actually found a legit use for this C code
	                 * oddity.  Woohoo! C Rules! Take that PASCAL!
	                 */
    for (loop = 0; loop < 200; loop++)
    {
        count = 9 - (loop / 20);
        draw_screen(2);
        xprint("Continue?", bigfont, 170, 100);
        if (mode == 0)
        {
            xprint("Insert Coin", bigfont, 140, 200);
            sprintf(str, "%02d", count);
            xprint(str, bigfont, 275, 300);
        }
        else
        {
            xprint("Press P1 Start", bigfont, 105, 200);
            sprintf(str, "%02d", count);
            xprint(str, bigfont, 275, 300);
        }
        SDL_UpdateRect(screen, 0, 0, 0, 0);
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                    mode = 1;
                    loop = 0;
                }
                if ((event.key.keysym.sym == keys[STARTKEY]) && (coins > 0))
                {
                    coins--;
                    lives = 3;
                    p1score = 0;
                    newlife = 1;
                    return 1;           /* The player is going to keep going! */
                }
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
            }
        }
        SDL_Delay(50);
        tick = (tick + 1) % 2520;
    }
    return 0;
}                                       /* End death subroutine */

/**************************************************************************
 *
 * Do intermission - Called at the successful completion of each level
 *
 **************************************************************************/
void do_intermission(int level)
{
    int loop;
    SDL_Event event;
    int ltemp;

    /* Need an intermission sound! */
    do_sounds(sounds[intermission]);
    Mix_HaltMusic();
    for (loop = 0; loop < 120; loop++)
    {
        draw_screen(1);
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                }
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
            }
        }
        SDL_Delay(50);
        tick = (tick + 1) % 2520;
    }                                   /* End delay for end of screen */
    if ((level % 5 != 0) && (level != 31))
    {
        return;
    }
    if (level <= 30)
    {
        ltemp = level / 5;
    }
    else
    {
        ltemp = RAND % 6;
    }
    if (ltemp == 1)
    {
        show_intermission1();
    }
    if (ltemp == 2)
    {
        show_intermission2();
    }
    if (ltemp == 3)
    {
        show_intermission3();
    }
    if (ltemp == 4)
    {
        show_intermission4();
    }
    if (ltemp == 5)
    {
        show_intermission5();
    }
    if (ltemp == 6)
    {
        show_intermission6();
    }
}                                       /* End intermission */

/****************************************************************************
 *
 * pause_mode - Handle being in pause mode!  We wait here until they hit
 *              'P' again!
 *
 ****************************************************************************/
void pause_mode()
{
    int loop;
    SDL_Event event;
    SDL_Rect rect;
    SDL_Rect pos;

    while (1)
    {
        draw_screen(1);
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                    return;
                }
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
                if (event.key.keysym.sym == keys[PAUSEKEY])
                {
                    return;
                }
            }
        }
        tick = (tick + 1) % 2520;
        SDL_Delay(50);
    }
}

/****************************************************************************
 *
 * play_game
 *
 ****************************************************************************/
int play_game(int real)
{
    SDL_Event event;
    Uint8 *keystate;
    SDL_Surface *temp;
    int x, y;                           /*
	                                 * Temp variables for adding new item,
	                                 * and map copy
	                                 */
    int loop;
    int newone;                         /* I worked out the logic for the demo before I */

    /*
     * made some tweaks to the item drop.  The item
     * drop affects the random number usage, so we
     * have to go through this complicated process to
     * restore it for the demo (since coming up with
     * a new demo sequence would be a pain in the
     * behind.  This variable is used to get the
     * Random chance of a item drop, since it is needed
     * in two spots.
     */

    /*
     * #################################
     * WORKING HERE CHANGE ME BACK!!!!!
     * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
     */
    level = 1;
    cableleft = 10;
    /*
     * Variable initialization is slightly different depending on if this
     * is a real game, or the demo loop.
     */
    if (real == 1)
    {
        SRAND(time(NULL));
        lives = 3;
        p1score = 0;
        Mix_PlayMusic(musics[bg], -1);
    }
    else
    {
        SRAND(0);
        lives = 0;
        teaser_tick = 0;
        /* Need to UnRandomize */
    }
    /* Set TwigMans initial position */
    jump = 0;
    xpos = 230;
    ypos = 390;
    yvel = 0;
    xface = 0;

    /* Set Fabbris initial position */
    fstatus = 0;                        /* No Fabbri to start */
    fyvel = 0;
    fypos = 0;
    fxface = 0;
    fjump = 0;
    fxpos = 0;
    /* Init students */
    for (loop = 0; loop < 20; loop++)
    {
        students[loop].active = 0;
    }
    bogerstat = 0;                      /* No boger yet! */
    bogerx = 0;
    bogery = 0;

    /*
     * New life every 15,000 points.  This keeps track of which one we are going
     * to award next.  That way we can do a > check (instead of ==) and not
     * keep awarding the same extra life multiple times.
     */
    newlife = 1;
    /* Initialize the current map with this level. */
    for (x = 0; x < 20; x++)
    {
        for (y = 0; y < 15; y++)
        {
            map[y][x] = level1[y][x];
        }
    }
    /*
     * *************************************************************************
     * *
     * * MAIN LOOP START
     * *
     * *************************************************************************
     */
    while (1)
    {
        if (ypos < -45)
        {
            ypos = -45;    /* Can't jump off the top of the screen */
        }
        /* Handle TwigMan jump */
        if (jump == 1)
        {
            yvel += GRAV;
            if (yvel > TERM)
            {
                yvel = TERM;
            }
            ypos += yvel;
            if (ypos >= 390)
            {
                ypos = 390;
                jump = 0;
            }
        }
        if (yvel > 0)   /* Decending jump and land */
        {
            if ((map[(int)((ypos + 45) / 30)][(xpos + 15) / 30] == 1) || (map[(int)((ypos + 45) / 30)][(xpos + 14) /
                    30] == 1))
            {
                yvel = 0;
                jump = 0;
                ypos = 30 * (int)((ypos + 15) / 30);
            }
        }
        /* Falling */
        if (((map[(int)((ypos + 30) / 30)][(xpos + 15) / 30] != 1) && (map[(int)((ypos + 30) / 30)][(xpos + 14) / 30] !=
                1)) && (jump == 0))
        {
            jump = 1;
            yvel = 1;
        }
        /*
         **********************************************************************
         *
         * START OBJECT COLLECTION
         *
         **********************************************************************
         */
        /* Book collection */
        if ((ypos > 0) && (map[(int)((ypos) / 30)][(xpos + 15) / 30] == 2))
        {
            map[(int)((ypos) / 30)][(xpos + 15) / 30] = 0;
            if (real == 1)
            {
                p1score += 10;
            }
            do_sounds(sounds[get_book]);
        }
        /* Cola Collection */
        if ((ypos > 0) && ((map[(int)((ypos) / 30)][(xpos + 15) / 30] == 3)))
        {
            map[(int)((ypos) / 30)][(xpos + 15) / 30] = 0;
            if (real == 1)
            {
                p1score += 100;
            }
            twigpower = 200;
            do_sounds(sounds[can_open]);
        }
        /* Cable Tester Collection */
        if ((ypos > 0) && (map[(int)((ypos) / 30)][(xpos + 15) / 30] == 4))
        {
            map[(int)((ypos) / 30)][(xpos + 15) / 30] = 0;
            if (real == 1)
            {
                p1score += 300;
            }
            cableleft -= 1;
            do_sounds(sounds[get_tester]);
            if (cableleft == 0)   /* They completed this level! */
            {
                do_intermission(level);
                level++;
                Mix_PlayMusic(musics[bg], -1);
                /* Need to reset positions for the next level! */
                if (level > 31)
                {
                    level = 31;
                }
                cableleft = 10;
                jump = 0;
                xpos = 230;
                ypos = 390;
                yvel = 0;
                xface = 0;

                fstatus = 0;
                fyvel = 0;
                fypos = 0;
                fxface = 0;
                fjump = 0;
                fxpos = 0;
                for (loop = 0; loop < 20; loop++)
                {
                    students[loop].active = 0;
                }
                bogerstat = 0;
                bogerx = 0;
                bogery = 0;
                /* Load a new map */
                for (x = 0; x < 20; x++)
                {
                    for (y = 0; y < 15; y++)
                    {
                        if (level % 6 == 0)
                        {
                            map[y][x] = level6[y][x];
                        }
                        if (level % 6 == 1)
                        {
                            map[y][x] = level1[y][x];
                        }
                        if (level % 6 == 2)
                        {
                            map[y][x] = level2[y][x];
                        }
                        if (level % 6 == 3)
                        {
                            map[y][x] = level3[y][x];
                        }
                        if (level % 6 == 4)
                        {
                            map[y][x] = level4[y][x];
                        }
                        if (level % 6 == 5)
                        {
                            map[y][x] = level5[y][x];
                        }
                    }
                }
            }                           /* End of done with level - init new */
        }                               /* End if cable tester */

        /* Add a new cola or cable tester */
        newone = RAND % 200;
        /*
         * The if test is ugly because I added the tick%180 after working out the
         * demo sequence.  With that addition, the random number use changes, so
         * you don't get the same game as before.  It took too long to generate an
         * interesting sequence last time, and I didn't want to go through it again
         * so I kludged in this variable and the ugly if statement to make the demo
         * play the same as before.
         */
        if (((newone < 1) || ((tick % 360) == 0)) && (real == 1) || ((newone < 1) && (real == 0)))
        {
            /* Max 9 secs between drops! But only for real play! */
            x = RAND % 20;
            y = RAND % 12 + 1;
            if (RAND % 2 < 1)
            {
                /* Add a new tester */
                if (map[y][x] == 0)
                {
                    map[y][x] = 4;
                    do_sounds(sounds[appear]);
                }
            }
            else
            {
                /* Add a new cola */
                if (map[y][x] == 0)
                {
                    map[y][x] = 3;
                    do_sounds(sounds[appear]);
                }
            }
        }                               /* End add new cola or cable tester */
        /*
         **********************************************************************
         *
         *  END OBJECT COLLECTION
         *
         **********************************************************************
         **********************************************************************
         *
         *  START FABBRI
         *
         **********************************************************************
         */
        /* A new Fabbri? */
        if ((fstatus == 0) && (RAND % 100) < 1)
        {
            if ((RAND % 2) == 0)
            {
                fstatus = 1;
                fxface = 0;
                fxpos = WIDTH - 30;
                fyvel = 0;
                fypos = RAND % 13;
                while (map[fypos][19] == 1)
                {
                    fypos = RAND % 13;
                }
                fypos = fypos * 30 + OFFSET;
                do_sounds(sounds[drive]);
            }
            else
            {
                fstatus = 1;
                fxface = 1;
                fxpos = 0;
                fyvel = 0;
                fypos = RAND % 13;
                while (map[fypos][0] == 1)
                {
                    fypos = RAND % 13;
                }
                fypos = fypos * 30 + OFFSET;
                do_sounds(sounds[drive]);
            }
        }
        if (fstatus == 1)   /*  Update Fabbri if he exists! */
        {
            if ((twigpower == 0) && (testcollide(xpos + 15, ypos + 15, 14, fxpos + 15, fypos + 15, 15)))
            {
                do_death();
                /* Need to reset for level restart */
                lives--;
                if (lives == 0)
                    if (do_end_of_game() == 0)
                    {
                        return;
                    }
                Mix_PlayMusic(musics[bg], -1);
                jump = 0;
                xpos = 230;
                ypos = 390;
                yvel = 0;
                xface = 0;

                fstatus = 0;
                fyvel = 0;
                fypos = 0;
                fxface = 0;
                fjump = 0;
                fxpos = 0;
                for (loop = 0; loop < 20; loop++)
                {
                    students[loop].active = 0;
                }
                bogerstat = 0;
                bogerx = 0;
                bogery = 0;
            }                           /* End death by Fabbri */
            /* Fabbri falling */
            if (map[(int)((fypos + 30) / 30)][(fxpos + 15) / 30] != 1)
            {
                fyvel = 5;
            }
            /* Fabbri lands! */
            if (map[(int)((fypos + 30) / 30)][(fxpos + 15) / 30] == 1)
            {
                fyvel = 0;
                fypos = 30 * (int)((fypos) / 30);
            }
            /* Fabbri steals the books */
            if (map[(int)((fypos + 10) / 30)][(fxpos + 15) / 30] == 2)
            {
                map[(int)((fypos + 10) / 30)][(fxpos + 15) / 30] = 0;
                do_sounds(sounds[steal]);
            }
            /* Update his position */
            fypos += fyvel;
            if (fxface == 0)
            {
                fxpos -= 7;
            }
            else
            {
                fxpos += 7;
            }
            /* If he is off the screen */
            if ((fxpos < 0) || (fxpos > (WIDTH - 20)))
            {
                fstatus = 0;
            }
        }                               /* End update Fabbri */
        /*
         *********************************************************************
         *
         * END FABBRI
         *
         *********************************************************************
         *********************************************************************
         *
         * START STUDENT
         *
         *********************************************************************
         */
        if (RAND % 100 < 2)
        {
            for (loop = 0; loop < 20; loop++)
            {
                if (students[loop].active == 0)
                {
                    /* Add a new student! */

                    students[loop].active = 1;
                    students[loop].x = 10 + (RAND % (WIDTH - 30));
                    students[loop].y = 0;
                    students[loop].face = RAND % 2;
                    loop = 20;
                }
            }
        }
        /* Need to move each of them */
        for (loop = 0; loop < 20; loop++)
        {
            if (students[loop].active == 1)
            {
                if (map[(students[loop].y + 20) / 30][(students[loop].x + 10) / 30] == 1)
                {
                    if (RAND % 100 < 1)
                    {
                        students[loop].face = abs(students[loop].face - 1);
                    }
                    if (students[loop].face == 0)
                    {
                        students[loop].x += 2;
                    }
                    else
                    {
                        students[loop].x -= 2;
                    }
                }
                else
                {
                    students[loop].y += 5;
                }
                /* If the student gets to the edge of the screen */
                if ((students[loop].x < 0) || (students[loop].x > WIDTH - 15))
                {
                    students[loop].active = 0;
                }
                /* Students drop their books */
                if (((RAND % 500) < 1) && (map[(students[loop].y) / 30][(students[loop].x + 10) / 30] == 0))
                {
                    map[(students[loop].y) / 30][(students[loop].x + 10) / 30] = 2;
                    do_sounds(sounds[bookdrop]);
                }
                /* Students steal the cable tester */
                if (map[(students[loop].y) / 30][(students[loop].x + 10) / 30] == 4)
                {
                    do_sounds(sounds[evil_laugh]);
                    map[(students[loop].y) / 30][(students[loop].x + 10) / 30] = 0;
                }
                /* If we land on a student */
                if ((yvel > 0) && ((xpos + 25 > students[loop].x) && (xpos + 5 < students[loop].x + 10) && (ypos + 30 >
                                   students[loop].y - 4) && (ypos + 30 < students[loop].y + 5)))
                {
                    students[loop].active = 0;
                    if (real == 1)
                    {
                        p1score += 50;
                    }
                    do_sounds(sounds[squish]);
                }
            }
        }                               /* For each student */
        /*
         *********************************************************************
         *
         * END STUDENT
         *
         *********************************************************************
         *********************************************************************
         *
         * START BOGER
         *
         *********************************************************************
         */
        /* Does Boger appear? */
        if ((bogerstat == 0) && ((RAND % 100) < 2))
        {
            bogerstat = 1;
            bogerx = 10 + (RAND % (WIDTH - 30));
            bogerhouse = bogerx - 37;
            bogery = -28;
        }
        /* Boger is going up */
        if (bogerstat == 2)
        {
            bogery -= 2;
            if (bogery == -30)
            {
                bogerstat = 0;
            }
        }
        /* Boger sees us */
        if (((bogerstat == 2) || (bogerstat == 1)) && ((bogerx > xpos - 50) && (bogerx + 30 < xpos + 75)))
        {
            bogery += 2;
            bogerstat = 3;
            do_sounds(sounds[falling]);
        }
        /* Boger is going down */
        if (bogerstat == 1)
        {
            bogery += 2;
            if (bogery == 0)
            {
                bogerstat = 2;
            }
        }
        /* Boger is falling */
        if (bogerstat == 3)
        {
            if ((twigpower == 0) && (testcollide(xpos + 15, ypos + 15, 14, bogerx + 12, bogery + 15, 12)))
            {
                do_death();
                lives--;
                if (lives == 0)
                    if (do_end_of_game() == 0)
                    {
                        return;
                    }
                Mix_PlayMusic(musics[bg], -1);
                /* Reset level */
                jump = 0;
                xpos = 230;
                ypos = 390;
                yvel = 0;
                xface = 0;

                fstatus = 0;
                fyvel = 0;
                fypos = 0;
                fxface = 0;
                fjump = 0;
                fxpos = 0;
                for (loop = 0; loop < 20; loop++)
                {
                    students[loop].active = 0;
                }
                bogerstat = 0;
                bogerx = 0;
                bogery = 0;
            }                           /* End dead */
            /* Update position */
            bogery += 8;
            if (bogery > HEIGHT + OFFSET)
            {
                bogerstat = 0;
            }
            if (bogerx < xpos)
            {
                bogerx += 3;
            }
            if (bogerx > xpos)
            {
                bogerx -= 3;
            }
        } /* End Boger */
        /*
         *********************************************************************
         *
         * END BOGER
         *
         *********************************************************************
         */
        /* If you got a new life */
        if (p1score > 15000 * newlife)
        {
            lives++;
            newlife++;
            do_sounds(sounds[life]);
        }
        /* New hiscore */
        if (p1score > hiscore)
        {
            hiscore = p1score;
        }
        draw_screen(real);

        /* TwigPower decays over time.  Need more diet cola */
        twigpower -= 2;
        if (twigpower < 0)
        {
            twigpower = 0;
        }
        /*
         *********************************************************************
         *
         * START KEYSTATE
         *
         *********************************************************************
         */
        SDL_PumpEvents();
        keystate = SDL_GetKeyState(NULL);
        /* Since left and right could be held down, we use getkeystate for them. */
        if (((keystate[keys[LEFTKEY]]) && (real == 1)) || ((real == 0) && (demo_play[teaser_tick] & 1)))
        {
            xpos -= 5;
            if (xpos < -0)
            {
                xpos = 0;    /* Not too far past edge of screen */
            }
            xface = 0;
        }
        if (((keystate[keys[RIGHTKEY]]) && (real == 1)) || ((real == 0) && (demo_play[teaser_tick] & 2)))
        {
            xpos += 5;
            if (xpos > (WIDTH - 30))
            {
                xpos = WIDTH - 30;    /* Not too far past edge of screen */
            }
            xface = 1;
        }
        if ((real == 0) && (demo_play[teaser_tick] & 4))
        {
            do_sounds(sounds[jumpping]);
            jump = 1;
            yvel = JUMP;
        }
        /* These keys only trip once per press.  Can't be held down! */
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (((event.key.keysym.sym == keys[JUMPKEY]) && ((jump == 0) || (twigpower > 0))) && (real == 1))
                {
                    do_sounds(sounds[jumpping]);
                    jump = 1;
                    yvel = JUMP;
                }
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                    if (real == 0)
                    {
                        return;
                    }
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
                if (event.key.keysym.sym == keys[PAUSEKEY])
                {
                    pause_mode();
                }
            }
        }                               /* End Event poll */

        SDL_Delay(50); /* 20 Frames per second */

        tick = (tick + 1) % 2520;       /* Animation clock */
        teaser_tick++;
        if ((real == 0) && (teaser_tick == 360))
        {
            return;
        }
    }                                   /* End while */
}   /* End play_game */

/*****************************************************************************
 *
 * Insert_coin teaser loop
 *
 *****************************************************************************/
void insert_coin(int type)
{
    int loop;
    SDL_Event event;
    SDL_Rect rect;
    SDL_Rect pos;

    for (loop = 0; loop < 100; loop++)
    {
        if (type == 1)   /* Only show this for the second Insert Coin! */
        {
            rect.x = 60;
            rect.y = 45;
            rect.w = 30;
            rect.h = 380;
            SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 0, 0));

            pos.x = 60;
            rect.y = 0;
            rect.w = 30;
            rect.h = 30;

            rect.x = 0;
            pos.y = 45;
            SDL_BlitSurface(book, &rect, screen, &pos);

            rect.x = (tick % 6) * 30;
            pos.y = 85;
            SDL_BlitSurface(cola, &rect, screen, &pos);

            rect.x = (tick / 10) % 6 * 30;
            pos.y = 125;
            SDL_BlitSurface(cable, &rect, screen, &pos);

            rect.x = 20 * (tick % 8);
            rect.w = 20;
            rect.h = 20;
            pos.x = 65;
            pos.y = 165;
            SDL_BlitSurface(student, &rect, screen, &pos);

            rect.x = 30 * ((tick / 23) % 2);
            rect.w = 30;
            rect.h = 30;
            pos.x = 60;
            pos.y = 315;
            SDL_BlitSurface(player, &rect, screen, &pos);

            rect.x = 30 * ((tick / 29) % 2);
            pos.y = 355;
            SDL_BlitSurface(fabbri, &rect, screen, &pos);

            rect.x = 0;
            rect.w = 25;
            pos.x = 62;
            pos.y = 395;
            SDL_BlitSurface(boger, &rect, screen, &pos);
        }
        /* Start by blanking the screen */
        rect.x = 140;
        rect.y = 210;
        rect.w = 305;
        rect.h = 50;
        SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 0, 0));
        xprint("Insert Coin", bigfont, 140, 200);
        SDL_UpdateRect(screen, 0, 0, 0, 0);
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                    return;
                }
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
            }
        }
        SDL_Delay(50);
        tick = (tick + 1) % 2520;
    }
}

/****************************************************************************
 *
 * point_screen - Show what the game objects are and what they are for!
 *
 ****************************************************************************/
void point_screen()
{
    int loop;
    SDL_Event event;
    SDL_Color c;
    SDL_Rect rect;
    SDL_Rect pos;
    char str[100];

    c.r = 255;
    c.g = 255;
    c.b = 255;
    for (loop = 0; loop < 100; loop++)
    {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        FixedPrint(screen, 140, 50, " 10     CSCI 221 Textbook      Dropped by the students", &c);
        FixedPrint(screen, 140, 90, "100     Diet Cola              Gives TwigMan Super-Mega-Bonus-TwigPower", &c);
        FixedPrint(screen, 140, 130, "300     Cable Tester           Collect enough for each level", &c);
        FixedPrint(screen, 140, 170, " 50     Student                They steal the Cable Testers", &c);
        FixedPrint(screen, 140, 320, "        TwigMan                Hero of CSCI 221 Lab", &c);
        FixedPrint(screen, 140, 360, "        Dr. Fabbri             Steals books he didn't write", &c);
        FixedPrint(screen, 140, 400, "        Professor Boger        Looking for a soft place to land", &c);

        pos.x = 60;
        rect.y = 0;
        rect.w = 30;
        rect.h = 30;

        rect.x = 0;
        pos.y = 45;
        SDL_BlitSurface(book, &rect, screen, &pos);

        rect.x = (tick % 6) * 30;
        pos.y = 85;
        SDL_BlitSurface(cola, &rect, screen, &pos);

        rect.x = (tick / 10) % 6 * 30;
        pos.y = 125;
        SDL_BlitSurface(cable, &rect, screen, &pos);

        rect.x = 20 * (tick % 8);
        rect.w = 20;
        rect.h = 20;
        pos.x = 65;
        pos.y = 165;
        SDL_BlitSurface(student, &rect, screen, &pos);

        rect.x = 30 * ((tick / 23) % 2);
        rect.w = 30;
        rect.h = 30;
        pos.x = 60;
        pos.y = 315;
        SDL_BlitSurface(player, &rect, screen, &pos);

        rect.x = 30 * ((tick / 29) % 2);
        pos.y = 355;
        SDL_BlitSurface(fabbri, &rect, screen, &pos);

        rect.x = 0;
        rect.w = 25;
        pos.x = 62;
        pos.y = 395;
        SDL_BlitSurface(boger, &rect, screen, &pos);

        sprintf(str, "P1: %06d", p1score);
        xprint(str, font, 1, -3);

        sprintf(str, "High Score: %06d", hiscore);
        xprint(str, font, WIDTH / 3, -3);

        SDL_UpdateRect(screen, 0, 0, 0, 0);
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                    return;
                }
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
            }
        }
        SDL_Delay(50);
        tick = (tick + 1) % 2520;
    }
}

/****************************************************************************
 *
 * p1_start - Once they have entered a coin, but not pressed p1 start.  This
 * holds them until they do.
 *
 ****************************************************************************/
void p1_start()
{
    int loop;
    SDL_Event event;
    SDL_Rect rect;
    SDL_Rect pos;

    while (1)
    {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        xprint("Press", bigfont, 240, 170);
        xprint("Player 1", bigfont, 195, 250);
        xprint("Start", bigfont, 240, 330);

        xprint("New TwigMan each 15000 points", font, 165, 430);
        for (loop = 0; loop < 16; loop++)
        {
            rect.x = 20 * (tick % 8);
            rect.w = 20;
            rect.h = 20;
            rect.y = 0;
            pos.x = -20 + (loop * 40) + (tick % 40);
            pos.y = 160;
            SDL_BlitSurface(student, &rect, screen, &pos);
            pos.x = -20 + (loop * 40) + (40 - (tick % 40));
            rect.y = 20;
            pos.y = 240;
            SDL_BlitSurface(student, &rect, screen, &pos);
            pos.x = -20 + (loop * 40) + (tick % 40);
            rect.y = 0;
            pos.y = 320;
            SDL_BlitSurface(student, &rect, screen, &pos);
            pos.x = -20 + (loop * 40) + (40 - (tick % 40));
            rect.y = 20;
            pos.y = 400;
            SDL_BlitSurface(student, &rect, screen, &pos);
        }
        rect.x = 0;
        rect.y = 0;
        rect.w = 300;
        rect.h = 130;
        pos.x = 150;
        pos.y = 10;
        SDL_BlitSurface(logo, &rect, screen, &pos);
        SDL_UpdateRect(screen, 0, 0, 0, 0);
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                }
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (event.key.keysym.sym == keys[STARTKEY])
                {
                    return;
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
            }
        }
        SDL_Delay(50);
        tick = (tick + 1) % 2520;
    }
}

/****************************************************************************
 *
 * teaser - Main logo screen!
 *
 ****************************************************************************/
void teaser()
{
    int loop;
    SDL_Event event;
    SDL_Rect rect;
    SDL_Rect pos;
    char str[100];

    for (loop = 0; loop < 100; loop++)
    {
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        rect.x = 0;
        rect.y = 0;
        rect.w = 300;
        rect.h = 130;
        pos.x = 150;
        pos.y = 50;
        SDL_BlitSurface(logo, &rect, screen, &pos);
        xprint("Created by Dr. Evan L. Schemm", font, 155, 300);
        xprint("Copyright 2007", font, 228, 330);
        xprint("Department of Mathematics and Computer Science", font, 70, 360);
        xprint("Lake Superior State University", font, 150, 390);

        sprintf(str, "P1: %06d", p1score);
        xprint(str, font, 1, -3);

        sprintf(str, "High Score: %06d", hiscore);
        xprint(str, font, WIDTH / 3, -3);

        SDL_UpdateRect(screen, 0, 0, 0, 0);
        while (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                exit(0);
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == keys[COINKEY])
                {
                    coins++;
                    do_sounds(sounds[money]);
                    return;
                }
                if (event.key.keysym.sym == keys[QUITKEY])
                {
                    exit(0);
                }
                if (event.key.keysym.sym == keys[FSKEY])
                {
                    screen_flags = screen_flags ^ SDL_FULLSCREEN;
                    change_screen(WIDTH, HEIGHT + OFFSET);
                }
            }
        }
        SDL_Delay(50);
    }
}

/* Main!  Left to the student to figure out! */
int main(int argc, char *argv[])
{
    SDL_Surface *temp;

    init_SDL_stuff();                   /* Do the SDL stuff */

    /*
     * Still need to load the surface, since it's not from a file, XPM
     * uses a slightly different command!
     */
    temp = IMG_ReadXPMFromArray(NewerTwig3_xpm);
    player = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(player, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(book_xpm);
    book = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(book, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(fabbri_xpm);
    fabbri = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(fabbri, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(cola_xpm);
    cola = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(cola, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(cable_xpm);
    cable = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(cable, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(man_xpm);
    student = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(student, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(boger30_xpm);
    boger = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(boger, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(roof_xpm);
    roof = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(roof, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(twigman_logo_xpm);
    logo = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(logo, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(level_xpm);
    levelglyph = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(levelglyph, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(grider_xpm);
    brick = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(brick, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(BigBoger2_xpm);
    BigBoger = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(BigBoger, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(ibm360_2_xpm);
    ibm_360 = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(ibm_360, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(rock_xpm);
    rock = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(rock, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(crater_xpm);
    crater = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(crater, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(road_xpm);
    road = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(road, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(signs_xpm);
    signs = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(signs, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(house_xpm);
    house = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(house, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(trucks_xpm);
    trucks = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(trucks, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(misc_xpm);
    misc = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(misc, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(BigTwig2_xpm);
    BigTwig = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(BigTwig, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(BigSchemm2_xpm);
    BigSchemm = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(BigSchemm, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(mover_xpm);
    movers = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(movers, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(cats_xpm);
    cats = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(cats, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(BigFabbri2_xpm);
    BigFabbri = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(BigFabbri, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(caset_xpm);
    caset = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(caset, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(zombie_xpm);
    zombie = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(zombie, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(eng_xpm);
    eng = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(eng, SDL_SRCCOLORKEY, 0xFE00FE);

    /*
     * Dinosaur clipart in the public domain as certified by fundraw.com
     * Can be found at www.fundraw.com/clipart/categories/00000139
     * Modified by Dr. Evan L. Schemm for use in this program
     */
    temp = IMG_ReadXPMFromArray(dino1_xpm);
    dino1 = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(dino1, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(dino2_xpm);
    dino2 = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(dino2, SDL_SRCCOLORKEY, 0xFE00FE);

    temp = IMG_ReadXPMFromArray(dino3_xpm);
    dino3 = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_SetColorKey(dino3, SDL_SRCCOLORKEY, 0xFE00FE);

    /* Downloaded from flashkit.com */

    sounds[endgame] = Mix_LoadWAV("./audio/Terminated.wav"); /* Made by: Ilhan Bayrak */
    sounds[evil_laugh] = Mix_LoadWAV("./audio/evil_laugh.wav"); /* Made by: Wiretrip */
    sounds[money] = Mix_LoadWAV("./audio/money.wav"); /* Made by: Colin Dodd */
    sounds[death] = Mix_LoadWAV("./audio/death.wav");           /* Made by: Doron Add */
    sounds[can_open] = Mix_LoadWAV("./audio/can_open.wav"); /* Made by SodaBush */
    sounds[falling] = Mix_LoadWAV("./audio/falling.wav"); /* Made by Deflux Design */
    sounds[steal] = Mix_LoadWAV("./audio/steal.wav");           /* Made by Amit Patel */
    sounds[intermission] = Mix_LoadWAV("./audio/cheer.wav");    /* Made by A Algien */
    sounds[crash] = Mix_LoadWAV("./audio/crash.wav");           /* Made by Adrian Gallant */
    sounds[thud] = Mix_LoadWAV("./audio/thud.wav");             /* Made by Splinteh */
    sounds[meow] = Mix_LoadWAV("./audio/meow.wav");             /* Made by Starmanltd */
    sounds[purr] = Mix_LoadWAV("./audio/purr.wav");             /* Made by Richard Harding */
    sounds[gun] = Mix_LoadWAV("./audio/gun.wav");               /* Made by Maboroshi Tsukuru */
    sounds[life] = Mix_LoadWAV("./audio/life.wav");             /* Made by Crow xxx */
    /* The following sounds made by:  Sith Master */
    sounds[appear] = Mix_LoadWAV("./audio/appear.wav");
    sounds[get_tester] = Mix_LoadWAV("./audio/get_item.wav");
    sounds[get_book] = Mix_LoadWAV("./audio/swish.wav");

    /* Downloaded from www.grsites.com/sounds/cartoon008.shtml (cartoon148.wav) */
    sounds[flower] = Mix_LoadWAV("./audio/flower.wav");
    /* Downloaded from www.grsites.com/sounds/cartoon009.shtml (cartoon172.wav) */
    sounds[squish] = Mix_LoadWAV("./audio/squish.wav");
    /* Downloaded from www.grsites.com/sounds/horror001.html (horror004.wav) */
    sounds[roar] = Mix_LoadWAV("./audio/roar.wav");
    /* Downloaded from simplythebest.net/sounds/WAV/cartoon_wavs.html */
    sounds[jumpping] = Mix_LoadWAV("./audio/boing_3.wav");
    /* Downloaded from www.acoustica.com/mp3-audio-mixer */
    sounds[drive] = Mix_LoadWAV("./audio/fabbri.wav");
    /* Downloaded from freesound.iua.upf.edu */
    sounds[bookdrop] = Mix_LoadWAV("./audio/oops.wav");

    /* Downloaded from flashkit.com  Made by: Nemo */
    musics[bg] = Mix_LoadMUS("./audio/Happ-Nemo-5891.wav");
    musics[inter1] = Mix_LoadMUS("./audio/Boss_1_T-miqal-8855.wav");

    tick = 0;                           /* Start animation clock */
    coins = 0;                          /* No coins at power on */
    /* Main game loop */
    while (1)
    {
        /* Teaser screen */
        if (coins == 0)
        {
            teaser();
        }
        /* Insert coin */
        if (coins == 0)
        {
            insert_coin(0);
        }
        /* Scoring screen */
        if (coins == 0)
        {
            point_screen();
        }
        /* Insert coin */
        if (coins == 0)
        {
            insert_coin(1);
        }
        /* Play game demo */
        if (coins == 0)
        {
            play_game(0);
        }
        /* If there is a coin */
        while (coins > 0)
        {
            /* Wait for player 1 start */
            p1_start();
            /* Play */
            coins--;
            play_game(1);
        }
    }                                   /* End main game while */
}   /* End main */
