/*jetpac.h
 * Header file for jetpac game.
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

#include <SDL/SDL.h>
// #include <SDL/SDL_image.h>
// #include "SDL/SDL_endian.h"

// General macros
#define MAX_BULLETS	7
#define MAX_PLATFORMS	20
#define MAX_MONSTERS	10
#define MAX_BOOTY	5
#define N_DEF		9999

// Booty type macros
#define FUEL		0
#define GOLD		1
#define TIME_BONUS	2
#define WEAPON_UPGRADE	3

// Rocket type macros:
#define STANDARD_ROCKET 0

// Rocket segment state macros:
#define UNTOUCHED	0
#define BEING_CARRIED	1
#define FALLING		2
#define ON_ROCKET	3
#define TAKING_OFF	4

// Monster AI string macros:
#define STATIC		0
#define ASTEROID	1
#define BEZERK		2
#define HOMING		3

// Direction macros:
#define LEFT		0
#define RIGHT		1

class entity
{
    unsigned int Xpos;
    unsigned int Ypos;
    unsigned int Num_Frames;
    unsigned int Anim_Frame;
    SDL_Surface *Image[9];
    unsigned char Alive;

public:
    void Set_Image(SDL_Surface *NewImage, unsigned int Anim_Frame);
    void Set_Anim_Frame(unsigned int New_Anim_Frame);
    void Set_Num_Frames(unsigned int New_Num_Frames);
    void Set_Xpos(unsigned int New_Xpos);
    void Set_Ypos(unsigned int New_Ypos);
    void Kill();
    void Ressurect();

    SDL_Surface *Get_Image(unsigned int Anim_Frame);
    unsigned int Get_Anim_Frame();
    unsigned int Get_Num_Frames();
    unsigned int Get_Xpos();
    unsigned int Get_Ypos();
    unsigned char Is_Alive();

    void Draw(SDL_Surface *screen);
    void Erase(SDL_Surface *screen, SDL_Surface *background);
    void Update(SDL_Surface *screen);

    // Constructors/Destructor
    entity(unsigned int New_Xpos = 1, unsigned int New_Ypos = 1, SDL_Surface *New_Image = NULL);
};

class booty: public entity
// Things the player can collect, i.e. rocket fuel, gold etc.
{
    unsigned int Type;                  // FUEL
    // GOLD
    // WEAPON_UPGRADE
    unsigned int State;                 // UNTOUCHED

    // FALLING etc.

public:
    void Set_Type(unsigned int New_Type);
    void Set_State(unsigned int New_State);

    unsigned int Get_Type();
    unsigned int Get_State();
};

class actor: public entity
// entities that are alive (i.e. players, monsters etc.)
{
    unsigned char Direction;
    unsigned int Reload_Speed;
    unsigned int Reload_Step;

public:
    void Set_Direction(unsigned char New_Direction);
    void Set_Reload_Speed(unsigned int New_Reload_Speed);
    void Set_Reload_Step(unsigned int New_Reload_Step);

    unsigned char Get_Direction();
    unsigned int Get_Reload_Speed();
    unsigned int Get_Reload_Step();

    // Constructors/Destructor
    actor(unsigned int New_Xpos = 1, unsigned int New_Ypos = 1, SDL_Surface *New_Image = NULL);
};

class player: public actor
{
    unsigned int Score;
    bool Standing;
    bool Falling;
    unsigned int Super_Weapon_Count;
    unsigned int Lives;
    unsigned int Thrust;
    bool Carrying;                      // Is player carrying anything, rocket parts, fuel etc?
    bool In_Rocket;

public:
    void Add_Score(unsigned int Amount);
    void Reset_Score();
    void Set_Lives(unsigned int New_Lives);
    void Set_Thrust(unsigned int New_Thrust);
    void Set_Standing(bool New_Standing);
    void Set_Falling(bool New_Falling);
    void Set_Carrying(bool New_Carrying);
    void Set_In_Rocket(bool New_In_Rocket);
    void Set_Super_Weapon_Count(unsigned int New_Super_Weapon_Count);

    unsigned int Get_Score();
    unsigned int Get_Lives();
    unsigned int Get_Thrust();

    bool Get_Standing();
    bool Get_Falling();
    bool Get_Carrying();
    bool Get_In_Rocket();
    bool Has_Super_Weapon();
    void Dec_Super_Weapon_Count();
};

class monster: public actor
{
    unsigned int Behaviour;             // 0 = static
    // 1 = asteroids
    // 2 = bezerk!!!
    // 3 = homing quickly
    unsigned int Speed;
    unsigned int Hit_Pts;
    unsigned int Max_Hit_Pts;
    bool Can_Spore;
    unsigned int Spore_Counter;
    bool Spore;
    bool Invincible;

public:
    void Set_Behaviour(unsigned int New_Behaviour);
    void Set_Speed(unsigned int New_Speed);
    void Set_Hit_Pts(unsigned int New_Hit_Pts);
    void Set_Max_Hit_Pts(unsigned int New_Max_Hit_Pts);
    void Set_Can_Spore(bool New_Can_Spore);
    void Dec_Spore_Counter();
    void Reset_Spore_Counter();
    void Set_Spore(bool New_Spore);
    void Set_Invincible(bool New_Invincible);

    unsigned int Get_Behaviour();
    unsigned int Get_Speed();
    unsigned int Get_Hit_Pts();
    unsigned int Get_Max_Hit_Pts();

    bool Get_Can_Spore();
    bool Spore_Ready();
    bool Get_Spore();
    bool Get_Invincible();

    // Constructors/Destructor
    monster(unsigned int New_Xpos = 1, unsigned int New_Ypos = 1, SDL_Surface *New_Image = NULL);
};

class rocket_segment: public entity
{
    unsigned char State;                // UNTOUCHED

    // BEING_CARRIED
    // FALLING
    // TAKING_OFF (once assembled)
    // ON_ROCKET

public:
    void Set_State(unsigned char New_State);

    unsigned char Get_State();
};

class rocket
{
    unsigned char Rocket_Type;
    unsigned char Num_Segments;
    rocket_segment *Rocket_Segment[5];
    unsigned int Max_Fuel;
    unsigned int Current_Fuel;
    bool Fuel_Full;
    bool Complete;

public:
    void Set_Rocket_Type(unsigned char New_Rocket_Type);
    void Set_Num_Segments(unsigned char New_Num_Segments);
    void Set_Rocket_Segment(rocket_segment *New_Rocket_Segment, unsigned char Index);
    void Set_Max_Fuel(unsigned int New_Max_Fuel);
    void Add_Fuel(unsigned int Num_Fuel);
    void Reset_Fuel();
    void Set_Complete(bool New_Complete);

    unsigned char Get_Rocket_Type();
    unsigned char Get_Num_Segments();
    unsigned int Get_Fuel();
    unsigned int Get_Max_Fuel();
    rocket_segment *Get_Rocket_Segment(unsigned char Index);

    bool Check_Fuel_Full();
    bool Is_Fuel_Full();
    bool Check_Complete();
    bool Is_Complete();
};

class bullet: public entity
{
    unsigned int Speed;
    unsigned int Direction;

public:
    void Set_Speed(unsigned int New_Speed);
    void Set_Direction(unsigned int New_Direction);

    unsigned int Get_Speed();
    unsigned int Get_Direction();
};

class level
{
    unsigned int Num_Platforms;
    entity *Platform[MAX_PLATFORMS];
    unsigned int Num_Monsters;
    monster *Monster[MAX_MONSTERS];
    unsigned int Monster_Freeze;
    unsigned int Num_Booty;
    booty *Booty[MAX_BOOTY];
    bullet *Bullet[MAX_BULLETS];
    bool Rocket_Level;                  // Does this level have a buildable rocket?
    rocket *Rocket;
    bool Complete;
    unsigned int Stage;
    SDL_Surface *Background;

public:
    void Set_Num_Platforms(unsigned int New_Num_Platforms);
    void Set_Platform(unsigned int Index, entity *New_Platform);
    void Set_Num_Monsters(unsigned int New_Num_Monsters);
    void Set_Background(SDL_Surface *New_Background);
    void Set_Monster(unsigned int Index, monster *New_Monster);
    void Set_Num_Booty(unsigned int New_Num_Booty);
    void Set_Booty(unsigned int Index, booty *New_Booty);
    void Set_Bullet(unsigned int Index, bullet *New_Bullet);
    void Set_Rocket_Level(bool New_Rocket_Level);
    void Set_Rocket(rocket *New_Rocket);
    void Set_Complete(bool New_Complete);
    void Set_Stage(unsigned int New_Stage);
    void Set_Monster_Freeze(unsigned int New_Monster_Freeze);

    unsigned int Get_Num_Platforms();
    entity *Get_Platform(unsigned int Index);
    monster *Get_Monster(unsigned int Monster_Num);
    unsigned int Get_Num_Booty();
    booty *Get_Booty(unsigned int Booty_Num);
    bullet *Get_Bullet(unsigned int Bullet_Num);
    unsigned int Get_Num_Monsters();
    SDL_Surface *Get_Background();

    bool Is_Rocket_Level();

    rocket *Get_Rocket();

    bool Is_Complete();

    unsigned int Get_Stage();

    bool Monsters_Frozen();
    void Dec_Monster_Freeze();

    void Draw_Background(SDL_Surface *screen);
};
