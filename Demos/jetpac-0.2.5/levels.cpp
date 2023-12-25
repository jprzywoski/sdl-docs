/*levels.cpp
 * Contains functions for initialising each level.
 *
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

extern SDL_Surface *LifeSprite;
extern SDL_Surface *FuelFullSprite;
extern SDL_Surface *FuelEmptySprite;

// Score sprites
extern SDL_Surface *Score1Sprite;
extern SDL_Surface *Score2Sprite;
extern SDL_Surface *Score3Sprite;
extern SDL_Surface *Score4Sprite;

// Booty sprites
extern SDL_Surface *FuelBooty;
extern SDL_Surface *GoldBooty;
extern SDL_Surface *TimeBooty;
extern SDL_Surface *WeaponBooty;
extern player Player;
extern level Level;
extern SDL_Surface *LoadBMP(char *file);
extern void Draw(unsigned int Xpos, unsigned int Ypos, unsigned int Width, unsigned int Height, SDL_Surface *image,
                 SDL_Surface *screen);
extern void Erase(unsigned int Xpos, unsigned int Ypos, unsigned int Width, unsigned int Height, SDL_Surface *screen,
                  SDL_Surface *background);
extern void Update(unsigned int Xpos, unsigned int Ypos, unsigned int Width, unsigned int Height, SDL_Surface *screen);

void Init_Stage_Basic()
{
// Initialises the things common to all levels, i.e. laser gun etc.
    unsigned int n;                     // Counter variable

    LifeSprite = LoadBMP("pixmaps/lifeicon.bmp");
    SDL_SetColorKey(LifeSprite, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(LifeSprite->format, 255, 255, 255));
    FuelFullSprite = LoadBMP("pixmaps/fuelfullicon.bmp");
    SDL_SetColorKey(FuelFullSprite, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(FuelFullSprite->format, 255, 255,
                    255));
    FuelEmptySprite = LoadBMP("pixmaps/fuelemptyicon.bmp");
    SDL_SetColorKey(FuelEmptySprite, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(FuelEmptySprite->format, 255, 255,
                    255));

    Score1Sprite = LoadBMP("pixmaps/score1icon.bmp");
    SDL_SetColorKey(Score1Sprite, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(Score1Sprite->format, 255, 255, 255));
    Score2Sprite = LoadBMP("pixmaps/score2icon.bmp");
    SDL_SetColorKey(Score2Sprite, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(Score2Sprite->format, 255, 255, 255));
    Score3Sprite = LoadBMP("pixmaps/score3icon.bmp");
    SDL_SetColorKey(Score3Sprite, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(Score3Sprite->format, 255, 255, 255));
    Score4Sprite = LoadBMP("pixmaps/score4icon.bmp");
    SDL_SetColorKey(Score4Sprite, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(Score4Sprite->format, 255, 255, 255));

    FuelBooty = LoadBMP("pixmaps/fueldrum.bmp");
    SDL_SetColorKey(FuelBooty, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(FuelBooty->format, 255, 255, 255));
    GoldBooty = LoadBMP("pixmaps/gold1.bmp");
    SDL_SetColorKey(GoldBooty, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(GoldBooty->format, 255, 255, 255));
    TimeBooty = LoadBMP("pixmaps/timepod.bmp");
    SDL_SetColorKey(TimeBooty, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(TimeBooty->format, 255, 255, 255));
    WeaponBooty = LoadBMP("pixmaps/weaponupgrade.bmp");
    SDL_SetColorKey(WeaponBooty, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(WeaponBooty->format, 255, 255, 255));
// Initialise bullets.
    for (n = 0; n < MAX_BULLETS; n++)
    {
        bullet *C_Bullet;

        C_Bullet = new bullet;
        C_Bullet->Set_Image(LoadBMP("pixmaps/laserbolt.bmp"), 0);
        C_Bullet->Set_Image(LoadBMP("pixmaps/laserbolt2.bmp"), 1);
        C_Bullet->Kill();               // Bullets are initially non active.
        C_Bullet->Set_Num_Frames(2);
        C_Bullet->Set_Anim_Frame(0);
        C_Bullet->Set_Speed(6);
        C_Bullet->Set_Direction(RIGHT);
        SDL_SetColorKey(C_Bullet->Get_Image(0), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Bullet->Get_Image(
                            0)->format, 255, 255, 255));
        SDL_SetColorKey(C_Bullet->Get_Image(1), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Bullet->Get_Image(
                            1)->format, 255, 255, 255));
        Level.Set_Bullet(n, C_Bullet);
    }
// No frozen entities at startup.
    Level.Set_Monster_Freeze(0);
    Player.Set_Super_Weapon_Count(0);
// Initialise the booty
    for (n = 0; n < Level.Get_Num_Booty(); n++)
    {
        booty *C_Booty = new booty;

        C_Booty->Kill();
        Level.Set_Booty(n, C_Booty);
    }
// Setup the player
    Player.Set_Num_Frames(9);
    Player.Set_Image(LoadBMP("pixmaps/jetmanb.bmp"), 0);
    Player.Set_Image(LoadBMP("pixmaps/jetmanbleft.bmp"), 1);
    Player.Set_Image(LoadBMP("pixmaps/jetmanb2.bmp"), 2);
    Player.Set_Image(LoadBMP("pixmaps/jetmanb2left.bmp"), 3);
// With cargo
    Player.Set_Image(LoadBMP("pixmaps/jetmanbcarry.bmp"), 4);
    Player.Set_Image(LoadBMP("pixmaps/jetmanbcarryleft.bmp"), 5);
    Player.Set_Image(LoadBMP("pixmaps/jetmanbcarry2.bmp"), 6);
    Player.Set_Image(LoadBMP("pixmaps/jetmanbcarryleft2.bmp"), 7);
    Player.Set_Image(LoadBMP("pixmaps/jetmandead.bmp"), 8);

    Player.Set_Anim_Frame(0);
    Player.Set_Xpos(10);
    Player.Set_Ypos(200);
    Player.Set_Direction(RIGHT);
    Player.Set_Reload_Speed(30);
    Player.Set_Reload_Step(0);
    Player.Set_Standing(false);
    Player.Set_Falling(false);
    Player.Set_Carrying(false);
    Player.Set_In_Rocket(false);
    Player.Ressurect();
    SDL_SetColorKey(Player.Get_Image(0), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(Player.Get_Image(0)->format, 255,
                    255, 255));
    SDL_SetColorKey(Player.Get_Image(1), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(Player.Get_Image(1)->format, 255,
                    255, 255));
    SDL_SetColorKey(Player.Get_Image(2), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(Player.Get_Image(2)->format, 255,
                    255, 255));
    SDL_SetColorKey(Player.Get_Image(3), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(Player.Get_Image(3)->format, 255,
                    255, 255));
    SDL_SetColorKey(Player.Get_Image(4), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(Player.Get_Image(4)->format, 255,
                    255, 255));
    SDL_SetColorKey(Player.Get_Image(5), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(Player.Get_Image(5)->format, 255,
                    255, 255));
    SDL_SetColorKey(Player.Get_Image(6), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(Player.Get_Image(6)->format, 255,
                    255, 255));
    SDL_SetColorKey(Player.Get_Image(7), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(Player.Get_Image(7)->format, 255,
                    255, 255));
    SDL_SetColorKey(Player.Get_Image(8), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(Player.Get_Image(8)->format, 255,
                    255, 255));
}

void Init_Stage1()
{
    unsigned int n;                     // Counter variable

// Setup the current level
    Level.Set_Background(LoadBMP("pixmaps/spacem.bmp"));
    Level.Set_Num_Monsters(4);
    Level.Set_Num_Platforms(5);
    Level.Set_Rocket_Level(true);
    Level.Set_Num_Booty(2);
    Level.Set_Complete(false);

// Setup the rocket!
    rocket *C_Rocket = new rocket;

    C_Rocket->Set_Rocket_Type(STANDARD_ROCKET);
    C_Rocket->Set_Num_Segments(3);
    C_Rocket->Set_Max_Fuel(4);
    C_Rocket->Reset_Fuel();
    C_Rocket->Set_Complete(false);

// Commit the changes to the DB
    Level.Set_Rocket(C_Rocket);

// Setup the basic level from this information
    Init_Stage_Basic();
// Now do the individual rocket segments.
    for (n = 0; n < Level.Get_Rocket()->Get_Num_Segments(); n++)
    {
        rocket_segment *C_Segment = new rocket_segment;

        C_Segment->Set_State(UNTOUCHED);
        if (n == 0)
        {
            C_Segment->Set_Image(LoadBMP("pixmaps/rocket_base.bmp"), 0);
            C_Segment->Set_Xpos(400);
            C_Segment->Set_Ypos(430);
            C_Segment->Set_State(ON_ROCKET);    // Base unit is already there.
        }
        if (n == 1)
        {
            C_Segment->Set_Image(LoadBMP("pixmaps/rocket_tube.bmp"), 0);
            C_Segment->Set_Xpos(230);
            C_Segment->Set_Ypos(30);
        }
        if (n == 2)
        {
            C_Segment->Set_Image(LoadBMP("pixmaps/rocket_head.bmp"), 0);
            C_Segment->Set_Xpos(300);
            C_Segment->Set_Ypos(350);
        }
        C_Segment->Set_Anim_Frame(0);
        C_Segment->Set_Num_Frames(1);
        C_Segment->Ressurect();

        SDL_SetColorKey(C_Segment->Get_Image(0), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Segment->Get_Image(
                            0)->format, 255, 255, 255));

// Commit this rocket segment.
        Level.Get_Rocket()->Set_Rocket_Segment(C_Segment, n);
    }
// Platforms
    for (n = 0; n < Level.Get_Num_Platforms(); n++)
    {
        entity *C_Platform;

        C_Platform = new entity;
        C_Platform->Set_Image(LoadBMP("pixmaps/platform1m.bmp"), 0);
        C_Platform->Set_Anim_Frame(0);
        C_Platform->Set_Num_Frames(1);
        if (n == 0)
        {
            C_Platform->Set_Xpos(50);
            C_Platform->Set_Ypos(300);
        }
        if (n == 1)
        {
            C_Platform->Set_Xpos(200);
            C_Platform->Set_Ypos(80);
        }
        if (n == 2)
        {
            C_Platform->Set_Xpos(250);
            C_Platform->Set_Ypos(80);
        }
        if (n == 3)
        {
            C_Platform->Set_Xpos(300);
            C_Platform->Set_Ypos(400);
        }
        if (n == 4)
        {
            C_Platform->Set_Xpos(350);
            C_Platform->Set_Ypos(400);
        }
        C_Platform->Ressurect();

        SDL_SetColorKey(C_Platform->Get_Image(0), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Platform->Get_Image(
                            0)->format, 255, 255, 255));

        Level.Set_Platform(n, C_Platform);
    }
// Monsters
    for (n = 0; n < Level.Get_Num_Monsters(); n++)
    {
        monster *C_Monster;

        C_Monster = new monster(100, 60 * n, LoadBMP("pixmaps/alien1.bmp"));    // Instantiate object.

        C_Monster->Set_Image(LoadBMP("pixmaps/asteroid.bmp"), 0);
        C_Monster->Set_Image(LoadBMP("pixmaps/asteroid.bmp"), 1);

        C_Monster->Set_Behaviour(ASTEROID);
        C_Monster->Set_Speed(3);
        C_Monster->Set_Hit_Pts(2);
        C_Monster->Set_Max_Hit_Pts(2);

        C_Monster->Set_Xpos(500);
        C_Monster->Set_Ypos(10 + (60 * n));
        C_Monster->Set_Num_Frames(1);
        C_Monster->Set_Anim_Frame(0);
        C_Monster->Set_Direction(LEFT);
        C_Monster->Kill();              // They aren't here initially

        SDL_SetColorKey(C_Monster->Get_Image(0), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Monster->Get_Image(
                            0)->format, 255, 255, 255));

        SDL_SetColorKey(C_Monster->Get_Image(1), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Monster->Get_Image(
                            1)->format, 255, 255, 255));
// Load this into the actual database.
        Level.Set_Monster(n, C_Monster);
    }
}

void Init_Stage2()
{
    unsigned int n;                     // Counter variable

// Setup the current level
    Level.Set_Background(LoadBMP("pixmaps/spacem.bmp"));
    Level.Set_Num_Monsters(5);
    Level.Set_Num_Platforms(5);
    Level.Set_Rocket_Level(true);
    Level.Set_Num_Booty(2);
    Level.Set_Complete(false);

// Setup the rocket!
    rocket *C_Rocket = new rocket;

    C_Rocket->Set_Rocket_Type(STANDARD_ROCKET);
    C_Rocket->Set_Num_Segments(3);
    C_Rocket->Set_Max_Fuel(4);
    C_Rocket->Reset_Fuel();
    C_Rocket->Set_Complete(false);

// Commit the changes to the DB
    Level.Set_Rocket(C_Rocket);

// Setup the basic level from this information
    Init_Stage_Basic();
// Now do the individual rocket segments.
    for (n = 0; n < Level.Get_Rocket()->Get_Num_Segments(); n++)
    {
        rocket_segment *C_Segment = new rocket_segment;

        C_Segment->Set_State(UNTOUCHED);
        if (n == 0)
        {
            C_Segment->Set_Image(LoadBMP("pixmaps/rocket_base.bmp"), 0);
            C_Segment->Set_Xpos(400);
            C_Segment->Set_Ypos(430);
            C_Segment->Set_State(ON_ROCKET);    // Base unit is already there.
        }
        if (n == 1)
        {
            C_Segment->Set_Image(LoadBMP("pixmaps/rocket_tube.bmp"), 0);
            C_Segment->Set_Xpos(10);
            C_Segment->Set_Ypos(50);
        }
        if (n == 2)
        {
            C_Segment->Set_Image(LoadBMP("pixmaps/rocket_head.bmp"), 0);
            C_Segment->Set_Xpos(500);
            C_Segment->Set_Ypos(430);
        }
        C_Segment->Set_Anim_Frame(0);
        C_Segment->Set_Num_Frames(1);
        C_Segment->Ressurect();

        SDL_SetColorKey(C_Segment->Get_Image(0), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Segment->Get_Image(
                            0)->format, 255, 255, 255));

// Commit this rocket segment.
        Level.Get_Rocket()->Set_Rocket_Segment(C_Segment, n);
    }
// Platforms
    for (n = 0; n < Level.Get_Num_Platforms(); n++)
    {
        entity *C_Platform;

        C_Platform = new entity;
        C_Platform->Set_Image(LoadBMP("pixmaps/platform2m.bmp"), 0);
        C_Platform->Set_Anim_Frame(0);
        C_Platform->Set_Num_Frames(1);
        if (n == 0)
        {
            C_Platform->Set_Xpos(0);
            C_Platform->Set_Ypos(100);
        }
        if (n == 1)
        {
            C_Platform->Set_Xpos(50);
            C_Platform->Set_Ypos(100);
        }
        if (n == 2)
        {
            C_Platform->Set_Xpos(250);
            C_Platform->Set_Ypos(350);
        }
        if (n == 3)
        {
            C_Platform->Set_Xpos(300);
            C_Platform->Set_Ypos(350);
        }
        if (n == 4)
        {
            C_Platform->Set_Xpos(250);
            C_Platform->Set_Ypos(250);
        }
        C_Platform->Ressurect();

        SDL_SetColorKey(C_Platform->Get_Image(0), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Platform->Get_Image(
                            0)->format, 255, 255, 255));

        Level.Set_Platform(n, C_Platform);
    }
// Monsters
    for (n = 0; n < Level.Get_Num_Monsters(); n++)
    {
        monster *C_Monster;

        C_Monster = new monster(100, 60 * n, LoadBMP("pixmaps/alien1.bmp"));    // Instantiate object.

        C_Monster->Set_Image(LoadBMP("pixmaps/alien3.bmp"), 0);
        C_Monster->Set_Image(LoadBMP("pixmaps/alien3.bmp"), 1);

        C_Monster->Set_Behaviour(BEZERK);
        C_Monster->Set_Speed(1);
        C_Monster->Set_Hit_Pts(3);
        C_Monster->Set_Max_Hit_Pts(3);

        C_Monster->Set_Xpos(500);
        C_Monster->Set_Ypos(10 + (60 * n));
        C_Monster->Set_Num_Frames(1);
        C_Monster->Set_Anim_Frame(0);
        C_Monster->Set_Direction(LEFT);
        C_Monster->Kill();

        SDL_SetColorKey(C_Monster->Get_Image(0), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Monster->Get_Image(
                            0)->format, 255, 255, 255));

        SDL_SetColorKey(C_Monster->Get_Image(1), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Monster->Get_Image(
                            1)->format, 255, 255, 255));
// Load this into the actual database.
        Level.Set_Monster(n, C_Monster);
    }
}

void Init_Stage3()
{
    unsigned int n;                     // Counter variable

// Setup the current level
    Level.Set_Background(LoadBMP("pixmaps/spacem.bmp"));
    Level.Set_Num_Monsters(4);
    Level.Set_Num_Platforms(8);
    Level.Set_Rocket_Level(true);
    Level.Set_Num_Booty(2);
    Level.Set_Complete(false);

// Setup the rocket!
    rocket *C_Rocket = new rocket;

    C_Rocket->Set_Rocket_Type(STANDARD_ROCKET);
    C_Rocket->Set_Num_Segments(3);
    C_Rocket->Set_Max_Fuel(4);
    C_Rocket->Reset_Fuel();
    C_Rocket->Set_Complete(false);

// Commit the changes to the DB
    Level.Set_Rocket(C_Rocket);

// Setup the basic level from this information
    Init_Stage_Basic();
// Now do the individual rocket segments.
    for (n = 0; n < Level.Get_Rocket()->Get_Num_Segments(); n++)
    {
        rocket_segment *C_Segment = new rocket_segment;

        C_Segment->Set_State(UNTOUCHED);
        if (n == 0)
        {
            C_Segment->Set_Image(LoadBMP("pixmaps/rocket_base.bmp"), 0);
            C_Segment->Set_Xpos(350);
            C_Segment->Set_Ypos(430);
            C_Segment->Set_State(ON_ROCKET);    // Base unit is already there.
        }
        if (n == 1)
        {
            C_Segment->Set_Image(LoadBMP("pixmaps/rocket_tube.bmp"), 0);
            C_Segment->Set_Xpos(270);
            C_Segment->Set_Ypos(50);
        }
        if (n == 2)
        {
            C_Segment->Set_Image(LoadBMP("pixmaps/rocket_head.bmp"), 0);
            C_Segment->Set_Xpos(200);
            C_Segment->Set_Ypos(50);
        }
        C_Segment->Set_Anim_Frame(0);
        C_Segment->Set_Num_Frames(1);
        C_Segment->Ressurect();

        SDL_SetColorKey(C_Segment->Get_Image(0), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Segment->Get_Image(
                            0)->format, 255, 255, 255));

// Commit this rocket segment.
        Level.Get_Rocket()->Set_Rocket_Segment(C_Segment, n);
    }
// Platforms
    for (n = 0; n < Level.Get_Num_Platforms(); n++)
    {
        entity *C_Platform;

        C_Platform = new entity;
        C_Platform->Set_Image(LoadBMP("pixmaps/platform1m.bmp"), 0);
        C_Platform->Set_Anim_Frame(0);
        C_Platform->Set_Num_Frames(1);
        if (n == 0)
        {
            C_Platform->Set_Xpos(60);
            C_Platform->Set_Ypos(100);
        }
        if (n == 1)
        {
            C_Platform->Set_Xpos(110);
            C_Platform->Set_Ypos(100);
            C_Platform->Set_Image(LoadBMP("pixmaps/platform3m.bmp"), 0);
        }
        if (n == 2)
        {
            C_Platform->Set_Xpos(160);
            C_Platform->Set_Ypos(100);
        }
        if (n == 3)
        {
            C_Platform->Set_Xpos(210);
            C_Platform->Set_Ypos(100);
            C_Platform->Set_Image(LoadBMP("pixmaps/platform3m.bmp"), 0);
        }
        if (n == 4)
        {
            C_Platform->Set_Xpos(260);
            C_Platform->Set_Ypos(100);
        }
        if (n == 5)
        {
            C_Platform->Set_Xpos(300);
            C_Platform->Set_Ypos(350);
        }
        if (n == 6)
        {
            C_Platform->Set_Xpos(400);
            C_Platform->Set_Ypos(350);
        }
        if (n == 7)
        {
            C_Platform->Set_Xpos(150);
            C_Platform->Set_Ypos(200);
        }
        C_Platform->Ressurect();

        SDL_SetColorKey(C_Platform->Get_Image(0), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Platform->Get_Image(
                            0)->format, 255, 255, 255));

        Level.Set_Platform(n, C_Platform);
    }
// Monsters
    for (n = 0; n < Level.Get_Num_Monsters(); n++)
    {
        monster *C_Monster;

        C_Monster = new monster(100, 60 * n, LoadBMP("pixmaps/alien1.bmp"));    // Instantiate object.

        C_Monster->Set_Image(LoadBMP("pixmaps/alien1.bmp"), 0);
        C_Monster->Set_Image(LoadBMP("pixmaps/alien1dead.bmp"), 1);

        C_Monster->Set_Behaviour(BEZERK);
        C_Monster->Set_Speed(1);
        C_Monster->Set_Hit_Pts(3);
        C_Monster->Set_Max_Hit_Pts(3);
        if (n == 0)
        {
            C_Monster->Set_Behaviour(ASTEROID);
            C_Monster->Set_Speed(3);
            C_Monster->Set_Image(LoadBMP("pixmaps/asteroid.bmp"), 0);
            C_Monster->Set_Image(LoadBMP("pixmaps/asteroid.bmp"), 1);
        }
        C_Monster->Set_Xpos(500);
        C_Monster->Set_Ypos(10 + (60 * n));
        C_Monster->Set_Num_Frames(1);
        C_Monster->Set_Anim_Frame(0);
        C_Monster->Set_Direction(LEFT);
        C_Monster->Kill();

        SDL_SetColorKey(C_Monster->Get_Image(0), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Monster->Get_Image(
                            0)->format, 255, 255, 255));

        SDL_SetColorKey(C_Monster->Get_Image(1), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Monster->Get_Image(
                            1)->format, 255, 255, 255));
// Load this into the actual database.
        Level.Set_Monster(n, C_Monster);
    }
}

void Init_Stage4()
{
    unsigned int n;                     // Counter variable

// Setup the current level
    Level.Set_Background(LoadBMP("pixmaps/spacem.bmp"));
    Level.Set_Num_Monsters(6);
    Level.Set_Num_Platforms(6);
    Level.Set_Rocket_Level(true);
    Level.Set_Num_Booty(2);
    Level.Set_Complete(false);

// Setup the rocket!
    rocket *C_Rocket = new rocket;

    C_Rocket->Set_Rocket_Type(STANDARD_ROCKET);
    C_Rocket->Set_Num_Segments(3);
    C_Rocket->Set_Max_Fuel(4);
    C_Rocket->Reset_Fuel();
    C_Rocket->Set_Complete(false);

// Commit the changes to the DB
    Level.Set_Rocket(C_Rocket);

// Setup the basic level from this information
    Init_Stage_Basic();
// Now do the individual rocket segments.
    for (n = 0; n < Level.Get_Rocket()->Get_Num_Segments(); n++)
    {
        rocket_segment *C_Segment = new rocket_segment;

        C_Segment->Set_State(UNTOUCHED);
        if (n == 0)
        {
            C_Segment->Set_Image(LoadBMP("pixmaps/rocket_base.bmp"), 0);
            C_Segment->Set_Xpos(350);
            C_Segment->Set_Ypos(430);
            C_Segment->Set_State(ON_ROCKET);    // Base unit is already there.
        }
        if (n == 1)
        {
            C_Segment->Set_Image(LoadBMP("pixmaps/rocket_tube.bmp"), 0);
            C_Segment->Set_Xpos(270);
            C_Segment->Set_Ypos(50);
        }
        if (n == 2)
        {
            C_Segment->Set_Image(LoadBMP("pixmaps/rocket_head.bmp"), 0);
            C_Segment->Set_Xpos(200);
            C_Segment->Set_Ypos(50);
        }
        C_Segment->Set_Anim_Frame(0);
        C_Segment->Set_Num_Frames(1);
        C_Segment->Ressurect();

        SDL_SetColorKey(C_Segment->Get_Image(0), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Segment->Get_Image(
                            0)->format, 255, 255, 255));

// Commit this rocket segment.
        Level.Get_Rocket()->Set_Rocket_Segment(C_Segment, n);
    }
// Platforms
    for (n = 0; n < Level.Get_Num_Platforms(); n++)
    {
        entity *C_Platform;

        C_Platform = new entity;
        C_Platform->Set_Image(LoadBMP("pixmaps/platform1m.bmp"), 0);
        C_Platform->Set_Anim_Frame(0);
        C_Platform->Set_Num_Frames(1);
        if (n == 0)
        {
            C_Platform->Set_Xpos(60);
            C_Platform->Set_Ypos(100);
        }
        if (n == 1)
        {
            C_Platform->Set_Xpos(110);
            C_Platform->Set_Ypos(100);
            C_Platform->Set_Image(LoadBMP("pixmaps/platform3m.bmp"), 0);
        }
        if (n == 2)
        {
            C_Platform->Set_Xpos(160);
            C_Platform->Set_Ypos(100);
        }
        if (n == 3)
        {
            C_Platform->Set_Xpos(210);
            C_Platform->Set_Ypos(100);
            C_Platform->Set_Image(LoadBMP("pixmaps/platform3m.bmp"), 0);
        }
        if (n == 4)
        {
            C_Platform->Set_Xpos(260);
            C_Platform->Set_Ypos(100);
        }
        if (n == 5)
        {
            C_Platform->Set_Xpos(300);
            C_Platform->Set_Ypos(350);
        }
        if (n == 6)
        {
            C_Platform->Set_Xpos(400);
            C_Platform->Set_Ypos(350);
        }
        if (n == 7)
        {
            C_Platform->Set_Xpos(150);
            C_Platform->Set_Ypos(200);
        }
        C_Platform->Ressurect();

        SDL_SetColorKey(C_Platform->Get_Image(0), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Platform->Get_Image(
                            0)->format, 255, 255, 255));

        Level.Set_Platform(n, C_Platform);
    }
// Monsters
    for (n = 0; n < Level.Get_Num_Monsters(); n++)
    {
        monster *C_Monster;

        C_Monster = new monster(100, 60 * n, LoadBMP("pixmaps/alien3.bmp"));    // Instantiate object.

        C_Monster->Set_Image(LoadBMP("pixmaps/asteroid.bmp"), 0);
        C_Monster->Set_Image(LoadBMP("pixmaps/asteroid.bmp"), 1);

        C_Monster->Set_Behaviour(ASTEROID);
        C_Monster->Set_Speed(3);
        C_Monster->Set_Hit_Pts(3);
        C_Monster->Set_Max_Hit_Pts(3);
        if (n <= 3)
        {
            C_Monster->Set_Behaviour(BEZERK);
            C_Monster->Set_Speed(1);
            C_Monster->Set_Image(LoadBMP("pixmaps/alien3.bmp"), 0);
            C_Monster->Set_Image(LoadBMP("pixmaps/alien3.bmp"), 1);
            C_Monster->Set_Max_Hit_Pts(3);
        }
        C_Monster->Set_Xpos(500);
        C_Monster->Set_Ypos(10 + (60 * n));
        C_Monster->Set_Num_Frames(1);
        C_Monster->Set_Anim_Frame(0);
        C_Monster->Set_Direction(LEFT);
        C_Monster->Kill();

        SDL_SetColorKey(C_Monster->Get_Image(0), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Monster->Get_Image(
                            0)->format, 255, 255, 255));

        SDL_SetColorKey(C_Monster->Get_Image(1), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Monster->Get_Image(
                            1)->format, 255, 255, 255));
// Load this into the actual database.
        Level.Set_Monster(n, C_Monster);
    }
}

void Init_Stage5()
{
    unsigned int n;                     // Counter variable

// Setup the current level
    Level.Set_Background(LoadBMP("pixmaps/spacem.bmp"));
    Level.Set_Num_Monsters(6);
    Level.Set_Num_Platforms(5);
    Level.Set_Rocket_Level(true);
    Level.Set_Num_Booty(2);
    Level.Set_Complete(false);

// Setup the rocket!
    rocket *C_Rocket = new rocket;

    C_Rocket->Set_Rocket_Type(STANDARD_ROCKET);
    C_Rocket->Set_Num_Segments(3);
    C_Rocket->Set_Max_Fuel(4);
    C_Rocket->Reset_Fuel();
    C_Rocket->Set_Complete(false);

// Commit the changes to the DB
    Level.Set_Rocket(C_Rocket);

// Setup the basic level from this information
    Init_Stage_Basic();
// Now do the individual rocket segments.
    for (n = 0; n < Level.Get_Rocket()->Get_Num_Segments(); n++)
    {
        rocket_segment *C_Segment = new rocket_segment;

        C_Segment->Set_State(UNTOUCHED);
        if (n == 0)
        {
            C_Segment->Set_Image(LoadBMP("pixmaps/rocket_base.bmp"), 0);
            C_Segment->Set_Xpos(400);
            C_Segment->Set_Ypos(430);
            C_Segment->Set_State(ON_ROCKET);    // Base unit is already there.
        }
        if (n == 1)
        {
            C_Segment->Set_Image(LoadBMP("pixmaps/rocket_tube.bmp"), 0);
            C_Segment->Set_Xpos(10);
            C_Segment->Set_Ypos(50);
        }
        if (n == 2)
        {
            C_Segment->Set_Image(LoadBMP("pixmaps/rocket_head.bmp"), 0);
            C_Segment->Set_Xpos(500);
            C_Segment->Set_Ypos(430);
        }
        C_Segment->Set_Anim_Frame(0);
        C_Segment->Set_Num_Frames(1);
        C_Segment->Ressurect();

        SDL_SetColorKey(C_Segment->Get_Image(0), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Segment->Get_Image(
                            0)->format, 255, 255, 255));

// Commit this rocket segment.
        Level.Get_Rocket()->Set_Rocket_Segment(C_Segment, n);
    }
// Platforms
    for (n = 0; n < Level.Get_Num_Platforms(); n++)
    {
        entity *C_Platform;

        C_Platform = new entity;
        C_Platform->Set_Image(LoadBMP("pixmaps/platform3m.bmp"), 0);
        C_Platform->Set_Anim_Frame(0);
        C_Platform->Set_Num_Frames(1);
        if (n == 0)
        {
            C_Platform->Set_Xpos(0);
            C_Platform->Set_Ypos(100);
        }
        if (n == 1)
        {
            C_Platform->Set_Xpos(50);
            C_Platform->Set_Ypos(100);
        }
        if (n == 2)
        {
            C_Platform->Set_Xpos(250);
            C_Platform->Set_Ypos(350);
        }
        if (n == 3)
        {
            C_Platform->Set_Xpos(300);
            C_Platform->Set_Ypos(350);
        }
        if (n == 4)
        {
            C_Platform->Set_Xpos(250);
            C_Platform->Set_Ypos(250);
        }
        C_Platform->Ressurect();

        SDL_SetColorKey(C_Platform->Get_Image(0), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Platform->Get_Image(
                            0)->format, 255, 255, 255));

        Level.Set_Platform(n, C_Platform);
    }
// Monsters
    for (n = 0; n < Level.Get_Num_Monsters(); n++)
    {
        monster *C_Monster;

        C_Monster = new monster(100, 80 * n, LoadBMP("pixmaps/alien1.bmp"));    // Instantiate object.

        C_Monster->Set_Image(LoadBMP("pixmaps/blob.bmp"), 0);
        C_Monster->Set_Image(LoadBMP("pixmaps/blob.bmp"), 1);

        C_Monster->Set_Behaviour(HOMING);
        C_Monster->Set_Spore(true);
        C_Monster->Set_Speed(2);
        C_Monster->Set_Hit_Pts(0);
        C_Monster->Set_Max_Hit_Pts(0);
        if (n == 0)
        {
            C_Monster->Set_Image(LoadBMP("pixmaps/alien2.bmp"), 0);
            C_Monster->Set_Image(LoadBMP("pixmaps/alien2.bmp"), 1);
            C_Monster->Set_Ypos(200);
            C_Monster->Set_Behaviour(BEZERK);
            C_Monster->Set_Can_Spore(true);
            C_Monster->Set_Spore(false);
            C_Monster->Set_Speed(1);
            C_Monster->Set_Hit_Pts(10);
            C_Monster->Set_Max_Hit_Pts(10);
        }
        C_Monster->Set_Xpos(500);
        C_Monster->Set_Ypos(10 + (60 * n));
        C_Monster->Set_Num_Frames(1);
        C_Monster->Set_Anim_Frame(0);
        C_Monster->Set_Direction(LEFT);
        C_Monster->Kill();

        SDL_SetColorKey(C_Monster->Get_Image(0), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Monster->Get_Image(
                            0)->format, 255, 255, 255));

        SDL_SetColorKey(C_Monster->Get_Image(1), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Monster->Get_Image(
                            1)->format, 255, 255, 255));
// Load this into the actual database.
        Level.Set_Monster(n, C_Monster);
    }
}

void Init_Stage6()
{
    unsigned int n;                     // Counter variable

// Setup the current level
    Level.Set_Background(LoadBMP("pixmaps/nebulae.bmp"));
    Level.Set_Num_Monsters(6);
    Level.Set_Num_Platforms(5);
    Level.Set_Rocket_Level(true);
    Level.Set_Num_Booty(2);
    Level.Set_Complete(false);

// Setup the rocket!
    rocket *C_Rocket = new rocket;

    C_Rocket->Set_Rocket_Type(STANDARD_ROCKET);
    C_Rocket->Set_Num_Segments(4);
    C_Rocket->Set_Max_Fuel(5);
    C_Rocket->Reset_Fuel();
    C_Rocket->Set_Complete(false);

// Commit the changes to the DB
    Level.Set_Rocket(C_Rocket);

// Setup the basic level from this information
    Init_Stage_Basic();
// Now do the individual rocket segments.
    for (n = 0; n < Level.Get_Rocket()->Get_Num_Segments(); n++)
    {
        rocket_segment *C_Segment = new rocket_segment;

        C_Segment->Set_State(UNTOUCHED);
        if (n == 0)
        {
            C_Segment->Set_Image(LoadBMP("pixmaps/rocket_base.bmp"), 0);
            C_Segment->Set_Xpos(350);
            C_Segment->Set_Ypos(430);
            C_Segment->Set_State(ON_ROCKET);    // Base unit is already there.
        }
        if (n == 1)
        {
            C_Segment->Set_Image(LoadBMP("pixmaps/rocket_tube.bmp"), 0);
            C_Segment->Set_Xpos(160);
            C_Segment->Set_Ypos(100);
        }
        if (n == 2)
        {
            C_Segment->Set_Image(LoadBMP("pixmaps/rocket_tubetop.bmp"), 0);
            C_Segment->Set_Xpos(310);
            C_Segment->Set_Ypos(100);
        }
        if (n == 3)
        {
            C_Segment->Set_Image(LoadBMP("pixmaps/rocket_head.bmp"), 0);
            C_Segment->Set_Xpos(10);
            C_Segment->Set_Ypos(100);
        }
        C_Segment->Set_Anim_Frame(0);
        C_Segment->Set_Num_Frames(1);
        C_Segment->Ressurect();

        SDL_SetColorKey(C_Segment->Get_Image(0), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Segment->Get_Image(
                            0)->format, 255, 255, 255));

// Commit this rocket segment.
        Level.Get_Rocket()->Set_Rocket_Segment(C_Segment, n);
    }
// Platforms
    for (n = 0; n < Level.Get_Num_Platforms(); n++)
    {
        entity *C_Platform;

        C_Platform = new entity;
        C_Platform->Set_Image(LoadBMP("pixmaps/platform3m.bmp"), 0);
        C_Platform->Set_Anim_Frame(0);
        C_Platform->Set_Num_Frames(1);
        if (n == 0)
        {
            C_Platform->Set_Xpos(0);
            C_Platform->Set_Ypos(150);
        }
        if (n == 1)
        {
            C_Platform->Set_Xpos(150);
            C_Platform->Set_Ypos(150);
        }
        if (n == 2)
        {
            C_Platform->Set_Xpos(300);
            C_Platform->Set_Ypos(150);
        }
        if (n == 3)
        {
            C_Platform->Set_Xpos(450);
            C_Platform->Set_Ypos(150);
        }
        if (n == 4)
        {
            C_Platform->Set_Xpos(600);
            C_Platform->Set_Ypos(150);
        }
        C_Platform->Ressurect();

        SDL_SetColorKey(C_Platform->Get_Image(0), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Platform->Get_Image(
                            0)->format, 255, 255, 255));

        Level.Set_Platform(n, C_Platform);
    }
// Monsters
    for (n = 0; n < Level.Get_Num_Monsters(); n++)
    {
        monster *C_Monster;

        C_Monster = new monster(100, 80 * n, LoadBMP("pixmaps/alien1.bmp"));    // Instantiate object.

        C_Monster->Set_Image(LoadBMP("pixmaps/alien3.bmp"), 0);
        C_Monster->Set_Image(LoadBMP("pixmaps/alien3.bmp"), 1);

        C_Monster->Set_Behaviour(ASTEROID);
        C_Monster->Set_Speed(2);
        C_Monster->Set_Hit_Pts(2);
        C_Monster->Set_Max_Hit_Pts(2);

        C_Monster->Set_Xpos(500);
        C_Monster->Set_Ypos(10 + (60 * n));
        C_Monster->Set_Num_Frames(1);
        C_Monster->Set_Anim_Frame(0);
        C_Monster->Set_Direction(LEFT);
        C_Monster->Kill();

        SDL_SetColorKey(C_Monster->Get_Image(0), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Monster->Get_Image(
                            0)->format, 255, 255, 255));

        SDL_SetColorKey(C_Monster->Get_Image(1), (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(C_Monster->Get_Image(
                            1)->format, 255, 255, 255));
// Load this into the actual database.
        Level.Set_Monster(n, C_Monster);
    }
}
