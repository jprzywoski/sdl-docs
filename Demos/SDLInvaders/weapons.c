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
#include "graphics.h"
#include "music.h"
#include "common.h"

extern int wavenumber;
extern int game_points;

int miracle_repair(miracle_p this, fighter_p fighter, int *shot_slots)
{
    if (fighter->health >= 100)
    {
        return 1;
    }
    fighter->health += 10;
    if (fighter->health > 100)
    {
        fighter->health = 100;
    }
    return 0;
}

int miracle_hotshot(miracle_p this, fighter_p fighter, int *shot_slots)
{
    if (*shot_slots >= MAX_SHOTS)
    {
        return 1;
    }
    *shot_slots += 6;
    if (*shot_slots > MAX_SHOTS)
    {
        *shot_slots = MAX_SHOTS;
    }
    return 0;
}

int miracle_nextWeapon(miracle_p this, fighter_p fighter, int *shot_slots)
{
    if (fighter->have_weapon >= 1)      /* FIXME: hardcoded limit */
    {
        return 1;
    }
    fighter->have_weapon++;
    fighter->active_weapon++;

    *shot_slots += 2;
    if (*shot_slots > MAX_SHOTS)
    {
        *shot_slots = MAX_SHOTS;
    }
    return 0;
}

/* return 0 means: special weapon is still active */
int specialWeapon_detect_hit(struct specialWeapon_s *this, fighter_p fighter, struct shot_s *shots, struct
                             shot2_s *ashots)
{
    int i, j;

    /* collision with alien shot */
    for (i = 0; i < MAX_SHOTS2; i++)
    {
        if (ashots[i].active && (this->x > ashots[i].x) && (this->x < ashots[i].x + 10) && (this->y > ashots[i].y) &&
                (this->y < ashots[i].y + 30))
        {
            /* all special weapons eat foreign shots */
            ashots[i].active = 0;
            game_points += (2 + wavenumber);
            /* since alien shots can overlap with aliens, we can't return yet */
        }
    }
    /* collision with blockade */
    for (i = 0; i < NUM_BLOCKADES; i++)
    {
        if (blockades[i].health > 0)
        {
            if ((this->x > blockades[i].x) && (this->x < blockades[i].x + INVADER_SIZE) && (this->y > blockades[i].y) &&
                    (this->y < blockades[i].y + INVADER_SIZE))
            {
                blockades[i].health -= 1;
            }
        }
    }
    /* collision with galien */
    for (i = 0; i < MAX_GALIENS; i++)
    {
        if (galiens[i].health > 0 && galiens[i].alien_type == 0 && (this->x + 10 > galiens[i].x) && (this->x - 10 <
                galiens[i].x + INVADER_SIZE) && (this->y + 10 > galiens[i].y) && (this->y - 10 < galiens[i].y +
                        INVADER_SIZE))
        {
            switch (this->type)
            {
            case BOMB:
                galiens[i].health = 0;
#ifndef NOSOUND
                play_sample(smp_explosion2);
#endif
                draw_explosion(galiens[i].x, galiens[i].y);
                /* look for neighbour aliens */
                for (j = 0; j < MAX_GALIENS; j++)
                {
                    if (j != i && galiens[j].health > 0 && sqrt(pow(galiens[j].x - this->x, 2) + pow(galiens[j].y -
                            this->y, 2)) < 90)
                    {
                        galiens[j].health = 0;
                        game_points += (9 + wavenumber);
#ifndef NOSOUND
                        play_sample(smp_explosion2);
#endif
                        draw_explosion(galiens[j].x, galiens[j].y);
                    }
                }
                this->active = 0;
                return 1;
                break;
            case ROCKET:
                galiens[i].health = 0;
#ifndef NOSOUND
                play_sample(smp_explosion2);
#endif
                draw_explosion(galiens[i].x, galiens[i].y);
                game_points += (9 + wavenumber);
                break;
            case SEEKROCKET:
                galiens[i].health = 0;
#ifndef NOSOUND
                play_sample(smp_explosion2);
#endif
                draw_explosion(galiens[i].x, galiens[i].y);
                game_points += (9 + wavenumber);
                this->active = 0;
                return 1;
                break;
            default:
                printf("warning: unknown weapon type [2]\n");
                break;
            }
        }
    }
    return 0;                           /* still active */
}

/* return 0 means: special weapon is still active */
int specialWeapon_move(struct specialWeapon_s *this, fighter_p fighter, struct shot_s *shots, struct shot2_s *ashots)
{
    const float slow_min = 2;
    const float slow_max = 200;
    int target_x, target_y;
    int mov_x, mov_y;
    float slow_x, slow_y;

    if (!this || !this->active)
    {
        return 1;
    }
    switch (this->type)
    {
    case BOMB:
    case ROCKET:
        this->y -= 7;
        break;
    case SEEKROCKET:
        if (this->target == NULL || this->target->health <= 0)
        {
            target_x = this->x;         /* up, up and away... */
            target_y = -100;
        }
        else
        {
            target_x = this->target->x;
            target_y = this->target->y;
        }
        mov_x = target_x - this->x;
        mov_y = target_y - this->y;
        slow_x = abs(mov_x) > slow_max ? 10 : abs(mov_x) * (10 - slow_min) / slow_max;
        slow_y = abs(mov_y) > slow_max ? 10 : abs(mov_y) * (10 - slow_min) / slow_max;
        if (mov_x > 10)
        {
            mov_x = slow_x;
        }
        else if (mov_x < -10)
        {
            mov_x = -slow_x;
        }
        if (mov_y > 10)
        {
            mov_y = slow_y;
        }
        else if (mov_y < -10)
        {
            mov_y = -slow_y;
        }
        this->y += mov_y;
        this->x += mov_x;
        break;
    default:
        printf("warning: unknown weapon type [3]\n");
        break;
    }
    /* check playfield boundaries */
    if (this->y < 0 || this->y > Y_RESOLUTION || this->x < 0 || this->x > X_RESOLUTION)
    {
        this->active = 0;
        return 1;
    }
    return specialWeapon_detect_hit(this, fighter, shots, ashots);
}

int specialWeapon_launch(struct specialWeapon_s *this, fighter_p fighter)
{
    int c, i, r;

#ifndef NOSOUND
    play_sample(smp_shot1);
#endif
    for (i = 0; i < MAX_SPECIAL_SHOTS; i++)
    {
        if (special_shots[i] == NULL)
        {
            if (this->type == SEEKROCKET)
            {
                /* choose a target */
                r = rand() % MAX_GALIENS;
                c = 0;
                while ((!galiens[r].health > 0 || galiens[r].alien_type != 0) && c <= MAX_GALIENS)
                {
                    r = (r + 1) % MAX_GALIENS;
                    c++;
                }
                if (c > MAX_GALIENS)
                {
                    /* no targets */
                    this->target = NULL;
                }
                else
                {
                    this->target = &galiens[r];
                }
            }
            this->x = fighter->x;
            this->y = fighter->y;
            this->active = 1;
            special_shots[i] = this;
            return 0;
        }
    }
    return 1;
}

int miracle_specialWeapon(miracle_p this, fighter_p fighter, int *shot_slots)
{
    int i;
    specialWeapon_p weapon;

    for (i = 0; i < MAX_FIGHTER_SPECIAL_SHOTS; i++)
    {
        if (fighter->bombs[i] == NULL)
        {
            weapon = malloc(sizeof (specialWeapon_t));
            fighter->bombs[i] = weapon;
            weapon->type = rand() % MAX_SPECIAL_WEAPON_TYPE;
            weapon->move = specialWeapon_move;
            weapon->launch = specialWeapon_launch;
            weapon->active = 0;
            return 0;
        }
    }
    return 1;
}
