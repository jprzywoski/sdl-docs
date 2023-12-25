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

extern SDL_Surface *alien_top[];
extern SDL_Surface *invader;
extern SDL_Surface *invader2;
extern int wavenumber;
extern int extrahard;
extern int speed_bonus;
int max = 0;
int min = X_RESOLUTION;
int invader_dir = 0;                    /* 0 = right, 1 = down, 2 = left, 3 = down */

/* When a new alien appears on the screen, call this function for it.
 * alien_type: 0 for normal aliens, 1 for aliens on top
 * x, y: the starting coordinates of the alien
 * health: the starting life energy for the alien
 * id: unique identification of the alien
 * Surface: The SDL_Surface of the alien
 */
int init_galien(int alien_type, int x, int y, int health, int frame, int id, SDL_Surface *Surface)
{
    galiens[id].alien_type = alien_type;
    galiens[id].x = x;
    galiens[id].y = y;
    galiens[id].health = health;
    galiens[id].frame = frame;
    galiens[id].Surface = Surface;

    return 1;
}

/* At the start of each wave, this function is called to initialise
 * the alien wave and determine how many of each type are in this wave.
 */
void init_invaders()
{
    int i, j, k;

    invader_dir = 0;
    /* Set the start position of all invaders.
     * Make them alive. */
    for (i = 0; i < NUM_INVADER_ROWS; i++)
    {
        for (j = 0; j < NUM_INVADERS_PER_ROW; j++)
        {
            k = rand() % (60);
            /* The normal (green) aliens */
            if (k >= wavenumber)
            {
                init_galien(0, (INVADER_DISTANCE + (j * (INVADER_SIZE + INVADER_DISTANCE))), ((i * INVADER_SIZE) + (2 *
                            INVADER_SIZE)), 10 + (extrahard * (wavenumber - 1) * 10), 1, ((i * NUM_INVADERS_PER_ROW) + j),
                            invader);
            }
            /* The meaner (blue) aliens */
            else
            {
                init_galien(0, (INVADER_DISTANCE + (j * (INVADER_SIZE + INVADER_DISTANCE))), ((i * INVADER_SIZE) + (2 *
                            INVADER_SIZE)), 20 + (extrahard * (wavenumber - 1) * 20), 1, ((i * NUM_INVADERS_PER_ROW) + j),
                            invader2);
            }
        }
    }
}

/* A generic alien is moved */
int move_galien(int id)
{
    /* normal aliens */
    if (galiens[id].alien_type == 0)
    {
        switch (invader_dir)
        {
        case 0:
            galiens[id].x += ((STEPSIZE) + speed_bonus);
            break;
        case 1:
            galiens[id].y += STEPSIZE * 2;
            break;
        case 2:
            galiens[id].x -= ((STEPSIZE) + speed_bonus);
            break;
        case 3:
            galiens[id].y += STEPSIZE * 2;
            break;
        }
        return 0;
    }
    /* special alien on top */
    if (galiens[id].alien_type == 1)
    {
        galiens[id].x -= STEPSIZE;
        if (galiens[id].x < (0 - INVADER_SIZE))
        {
            if (galiens[id].health > 0)
            {
                galiens[id].health = 0;
                the_fighter.health -= 1;
            }
        }
        galiens[id].frame -= 1;
        if (galiens[id].frame < 0)
        {
            galiens[id].frame = 5;
        }
        galiens[id].Surface = alien_top[galiens[id].frame];
    }
    return 0;
}

/* Checks if any aliens need to be moved and calls move_galien for those */
void check_move_galiens()
{
    int i, min = X_RESOLUTION, max = 0;

    switch (invader_dir)
    {
    case 0:
        for (i = 0; i < NUM_INVADERS; i++)
        {
            if ((galiens[i].health > 0) && (max < galiens[i].x))
            {
                max = galiens[i].x;
            }
        }
        if (max + INVADER_SIZE >= X_RESOLUTION)
        {
            invader_dir = 1;
        }
        break;
    case 1:
        invader_dir = 2;
        break;
    case 2:
        for (i = 0; i < NUM_INVADERS; i++)
        {
            if ((galiens[i].health > 0) && (min > galiens[i].x))
            {
                min = galiens[i].x;
            }
        }
        if (min <= 0)
        {
            invader_dir = 3;
        }
        break;
    case 3:
        invader_dir = 0;
        break;
    }
    for (i = 0; i < MAX_GALIENS; i++)
    {
        move_galien(i);
    }
}
