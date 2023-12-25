/*jetpac.cpp
 * Object framework code for Jetpac
 * .
 * Copyright (C) 2001  James Blewitt
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "jetpac.h"
#include <iostream>
#include <stdlib.h>

// ********************class entity methods************************

void entity::Set_Image(SDL_Surface *NewImage, unsigned int Anim_Frame)
{
    Image[Anim_Frame] = NewImage;
}

void entity::Set_Anim_Frame(unsigned int New_Anim_Frame)
{
    Anim_Frame = New_Anim_Frame;
}

void entity::Set_Num_Frames(unsigned int New_Num_Frames)
{
    Num_Frames = New_Num_Frames;
}

void entity::Set_Xpos(unsigned int New_Xpos)
{
    Xpos = New_Xpos;
}

void entity::Set_Ypos(unsigned int New_Ypos)
{
    Ypos = New_Ypos;
}

void entity::Kill()
{
    Alive = 0;
}

void entity::Ressurect()
{
    Alive = 1;
}

SDL_Surface *entity::Get_Image(unsigned int Anim_Frame)
{
    return Image[Anim_Frame];
}

unsigned int entity::Get_Anim_Frame()
{
    return Anim_Frame;
}

unsigned int entity::Get_Num_Frames()
{
    return Num_Frames;
}

unsigned int entity::Get_Xpos()
{
    return Xpos;
}

unsigned int entity::Get_Ypos()
{
    return Ypos;
}

unsigned char entity::Is_Alive()
{
    return Alive;
}

void entity::Draw(SDL_Surface *screen)
{
    SDL_Rect dest;

    /* Blit onto the screen surface.
     * The surfaces should not be locked at this point.
     */
    dest.x = Xpos;
    dest.y = Ypos;
    dest.w = Image[Anim_Frame]->w;
    dest.h = Image[Anim_Frame]->h;
    SDL_BlitSurface(Image[Anim_Frame], NULL, screen, &dest);

    /* Update the changed portion of the screen */
    SDL_UpdateRects(screen, 1, &dest);
}

void entity::Erase(SDL_Surface *screen, SDL_Surface *background)
{
// Draws the area of the background from under the sprite to the screen.
    SDL_Rect src;

    src.x = Xpos;
    src.y = Ypos;
    src.w = Image[Anim_Frame]->w;
    src.h = Image[Anim_Frame]->h;

    SDL_BlitSurface(background, &src, screen, &src);
}

void entity::Update(SDL_Surface *screen)
{
    SDL_Rect rect;

    rect.x = Xpos;
    rect.y = Ypos;
    rect.w = Image[Anim_Frame]->w;
    rect.h = Image[Anim_Frame]->h;
// Don't try to update stuff that is off the screen.
    if (rect.x + rect.w > screen->w)
    {
        rect.w = screen->w - rect.x;
    }
    if (rect.y + rect.h > screen->h)
    {
        rect.h = screen->h - rect.y;
    }
    SDL_UpdateRects(screen, 1, &rect);
}

entity::entity(unsigned int New_Xpos, unsigned int New_Ypos, SDL_Surface *New_Image)
{
    Set_Xpos(New_Xpos);
    Set_Ypos(New_Ypos);
    Set_Num_Frames(0);
    Set_Anim_Frame(0);
    Set_Image(New_Image, 0);
    Ressurect();
}

// **************************class booty methods*************************

void booty::Set_Type(unsigned int New_Type)
{
    Type = New_Type;
}

void booty::Set_State(unsigned int New_State)
{
    State = New_State;
}

unsigned int booty::Get_Type()
{
    return Type;
}

unsigned int booty::Get_State()
{
    return State;
}

// *************************class actor methods**************************

void actor::Set_Direction(unsigned char New_Direction)
{
    Direction = New_Direction;
}

void actor::Set_Reload_Speed(unsigned int New_Reload_Speed)
{
    Reload_Speed = New_Reload_Speed;
}

void actor::Set_Reload_Step(unsigned int New_Reload_Step)
{
    Reload_Step = New_Reload_Step;
}

unsigned char actor::Get_Direction()
{
    return Direction;
}

unsigned int actor::Get_Reload_Speed()
{
    return Reload_Speed;
}

unsigned int actor::Get_Reload_Step()
{
    return Reload_Step;
}

actor::actor(unsigned int New_Xpos, unsigned int New_Ypos, SDL_Surface *New_Image)
{
    // Call the base constructor
actor:
    entity(New_Xpos, New_Ypos, New_Image);

    Set_Direction(RIGHT);
    Set_Reload_Speed(100);
    Set_Reload_Step(0);
}

// *************************class player methods***************************

void player::Add_Score(unsigned int Amount)
{
    Score += Amount;
}

void player::Reset_Score()
{
    Score = 0;
}

void player::Set_Lives(unsigned int New_Lives)
{
    Lives = New_Lives;
}

void player::Set_Thrust(unsigned int New_Thrust)
{
    Thrust = New_Thrust;
}

void player::Set_Standing(bool New_Standing)
{
    Standing = New_Standing;
}

void player::Set_Falling(bool New_Falling)
{
    Falling = New_Falling;
}

void player::Set_Carrying(bool New_Carrying)
{
    Carrying = New_Carrying;
}

void player::Set_In_Rocket(bool New_In_Rocket)
{
    In_Rocket = New_In_Rocket;
}

void player::Set_Super_Weapon_Count(unsigned int New_Super_Weapon_Count)
{
    Super_Weapon_Count = New_Super_Weapon_Count;
}

unsigned int player::Get_Score()
{
    return Score;
}

unsigned int player::Get_Lives()
{
    return Lives;
}

unsigned int player::Get_Thrust()
{
    return Thrust;
}

bool player::Get_Standing()
{
    return Standing;
}

bool player::Get_Falling()
{
    return Falling;
}

bool player::Get_Carrying()
{
    return Carrying;
}

bool player::Get_In_Rocket()
{
    return In_Rocket;
}

bool player::Has_Super_Weapon()
{
    if (Super_Weapon_Count == 0)
    {
        return false;
    }
    return true;
}

void player::Dec_Super_Weapon_Count()
{
    if (Super_Weapon_Count > 0)
    {
        Super_Weapon_Count--;
    }
}

// ***********************class monster methods**************************

void monster::Set_Behaviour(unsigned int New_Behaviour)
{
    Behaviour = New_Behaviour;
}

void monster::Set_Speed(unsigned int New_Speed)
{
    Speed = New_Speed;
}

void monster::Set_Hit_Pts(unsigned int New_Hit_Pts)
{
    Hit_Pts = New_Hit_Pts;
}

void monster::Set_Max_Hit_Pts(unsigned int New_Max_Hit_Pts)
{
    Max_Hit_Pts = New_Max_Hit_Pts;
}

void monster::Set_Can_Spore(bool New_Can_Spore)
{
    Can_Spore = New_Can_Spore;
}

void monster::Dec_Spore_Counter()
{
    if (Spore_Counter > 0 & rand() % 5 == 0)
    {
        Spore_Counter--;
    }
}

void monster::Reset_Spore_Counter()
{
    Spore_Counter = 150;
}

void monster::Set_Spore(bool New_Spore)
{
    Spore = New_Spore;
}

void monster::Set_Invincible(bool New_Invincible)
{
    Invincible = New_Invincible;
}

unsigned int monster::Get_Behaviour()
{
    return Behaviour;
}

unsigned int monster::Get_Speed()
{
    return Speed;
}

unsigned int monster::Get_Hit_Pts()
{
    return Hit_Pts;
}

unsigned int monster::Get_Max_Hit_Pts()
{
    return Max_Hit_Pts;
}

bool monster::Get_Can_Spore()
{
    return Can_Spore;
}

bool monster::Spore_Ready()
{
    if (Spore_Counter == 0)
    {
        return true;
    }
    return false;
}

bool monster::Get_Spore()
{
    return Spore;
}

bool monster::Get_Invincible()
{
    return Invincible;
}

monster::monster(unsigned int New_Xpos, unsigned int New_Ypos, SDL_Surface *New_Image)
{
    // Call the base constructor
monster:
    actor(New_Xpos, New_Ypos, New_Image);

    Set_Behaviour(BEZERK);
    Set_Speed(1);
    Set_Can_Spore(false);
    Set_Spore(false);
    Spore_Counter = 400;
    Invincible = false;
}

// ************************class rocket_segment methods******************

void rocket_segment::Set_State(unsigned char New_State)
{
    State = New_State;
}

unsigned char rocket_segment::Get_State()
{
    return State;
}

// ***************************class rocket methods***********************

void rocket::Set_Rocket_Type(unsigned char New_Rocket_Type)
{
    Rocket_Type = New_Rocket_Type;
}

void rocket::Set_Num_Segments(unsigned char New_Num_Segments)
{
    Num_Segments = New_Num_Segments;
}

void rocket::Set_Rocket_Segment(rocket_segment *New_Rocket_Segment, unsigned char Index)
{
    Rocket_Segment[Index] = New_Rocket_Segment;
}

void rocket::Set_Max_Fuel(unsigned int New_Max_Fuel)
{
    Max_Fuel = New_Max_Fuel;
}

void rocket::Add_Fuel(unsigned int Num_Fuel)
{
    Current_Fuel += Num_Fuel;
}

void rocket::Reset_Fuel()
{
    Current_Fuel = 0;
    Fuel_Full = false;
}

void rocket::Set_Complete(bool New_Complete)
{
    Complete = New_Complete;
}

unsigned char rocket::Get_Rocket_Type()
{
    return Rocket_Type;
}

unsigned char rocket::Get_Num_Segments()
{
    return Num_Segments;
}

unsigned int rocket::Get_Fuel()
{
    return Current_Fuel;
}

unsigned int rocket::Get_Max_Fuel()
{
    return Max_Fuel;
}

rocket_segment *rocket::Get_Rocket_Segment(unsigned char Index)
{
    return Rocket_Segment[Index];
}

bool rocket::Check_Fuel_Full()
{
    if (Current_Fuel == Max_Fuel)
    {
        Fuel_Full = true;
        return true;
    }
    return false;
}

bool rocket::Is_Fuel_Full()
{
    return Fuel_Full;
}

bool rocket::Check_Complete()
{
    unsigned int n;

    for (n = 0; n < this->Get_Num_Segments(); n++)
        if (this->Get_Rocket_Segment(n)->Get_State() != ON_ROCKET)
        {
            return false;
        }
    Complete = true;
    return true;
}

bool rocket::Is_Complete()
{
    return Complete;
}

// *************************class bullet methods*************************

void bullet::Set_Speed(unsigned int New_Speed)
{
    Speed = New_Speed;
}

void bullet::Set_Direction(unsigned int New_Direction)
{
    Direction = New_Direction;
}

unsigned int bullet::Get_Speed()
{
    return Speed;
}

unsigned int bullet::Get_Direction()
{
    return Direction;
}

// **************************class level methods****************************

void level::Set_Num_Platforms(unsigned int New_Num_Platforms)
{
    Num_Platforms = New_Num_Platforms;
}

void level::Set_Platform(unsigned int Index, entity *New_Platform)
{
    Platform[Index] = New_Platform;
}

void level::Set_Num_Monsters(unsigned int New_Num_Monsters)
{
    Num_Monsters = New_Num_Monsters;
}

void level::Set_Monster(unsigned int Index, monster *New_Monster)
{
    Monster[Index] = New_Monster;
}

void level::Set_Num_Booty(unsigned int New_Num_Booty)
{
    Num_Booty = New_Num_Booty;
}

void level::Set_Booty(unsigned int Index, booty *New_Booty)
{
    Booty[Index] = New_Booty;
}

void level::Set_Bullet(unsigned int Index, bullet *New_Bullet)
{
    Bullet[Index] = New_Bullet;
}

void level::Set_Background(SDL_Surface *New_Background)
{
    Background = New_Background;
}

void level::Set_Rocket_Level(bool New_Rocket_Level)
{
    Rocket_Level = New_Rocket_Level;
}

void level::Set_Rocket(rocket *New_Rocket)
{
    Rocket = New_Rocket;
}

void level::Set_Complete(bool New_Complete)
{
    Complete = New_Complete;
}

void level::Set_Stage(unsigned int New_Stage)
{
    Stage = New_Stage;
}

void level::Set_Monster_Freeze(unsigned int New_Monster_Freeze)
{
    Monster_Freeze = New_Monster_Freeze;
}

entity *level::Get_Platform(unsigned int Index)
{
    return Platform[Index];
}

unsigned int level::Get_Num_Platforms()
{
    return Num_Platforms;
}

monster *level::Get_Monster(unsigned int Monster_Num)
{
    return Monster[Monster_Num];
}

unsigned int level::Get_Num_Booty()
{
    return Num_Booty;
}

booty *level::Get_Booty(unsigned int Booty_Num)
{
    return Booty[Booty_Num];
}

bullet *level::Get_Bullet(unsigned int Bullet_Num)
{
    return Bullet[Bullet_Num];
}

unsigned int level::Get_Num_Monsters()
{
    return Num_Monsters;
}

SDL_Surface *level::Get_Background()
{
    return Background;
}

bool level::Is_Rocket_Level()
{
    return Rocket_Level;
}

rocket *level::Get_Rocket()
{
    return Rocket;
}

bool level::Is_Complete()
{
    return Complete;
}

unsigned int level::Get_Stage()
{
    return Stage;
}

bool level::Monsters_Frozen()
{
    if (Monster_Freeze == 0)
    {
        return false;
    }
    return true;
}

void level::Dec_Monster_Freeze()
{
    if (Monster_Freeze > 0)
    {
        Monster_Freeze--;
    }
}

void level::Draw_Background(SDL_Surface *screen)
{
    SDL_Rect dest;

    /* Blit onto the screen surface.
     * The surfaces should not be locked at this point.
     */
    dest.x = 0;
    dest.y = 0;
    dest.w = Background->w;
    dest.h = Background->h;
    SDL_BlitSurface(Background, NULL, screen, &dest);

    /* Update the changed portion of the screen */
    SDL_UpdateRects(screen, 1, &dest);
}
