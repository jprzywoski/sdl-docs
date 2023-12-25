/*game.cpp
 * Main game code.
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
#include <stdlib.h>
#include <time.h>

using namespace std;

#define TICK_INTERVAL 12

player Player;
level Level;
SDL_Joystick *Joystick;
unsigned int videoflags;
bool ScreenModeChanged = false;         // true = Screen mode has been altered.
bool FullScreenMode = false;
unsigned int Initial_Stage = 0;         // When this is assigned n>=1 game must have been started
SDL_Surface *LifeSprite;                // Sprite used to show the number of lives
SDL_Surface *FuelFullSprite;
SDL_Surface *FuelEmptySprite;

// Score sprites
SDL_Surface *Score1Sprite;
SDL_Surface *Score2Sprite;
SDL_Surface *Score3Sprite;
SDL_Surface *Score4Sprite;

// Booty sprites
SDL_Surface *FuelBooty;
SDL_Surface *GoldBooty;
SDL_Surface *TimeBooty;
SDL_Surface *WeaponBooty;
extern SDL_Surface *LoadBMP(char *file);
extern void Draw(unsigned int Xpos, unsigned int Ypos, unsigned int Width, unsigned int Height, SDL_Surface *image,
                 SDL_Surface *screen);
extern void Erase(unsigned int Xpos, unsigned int Ypos, unsigned int Width, unsigned int Height, SDL_Surface *screen,
                  SDL_Surface *background);
extern void Update(unsigned int Xpos, unsigned int Ypos, unsigned int Width, unsigned int Height, SDL_Surface *screen);

Uint32 TimeLeft()
{
    static Uint32 next_time = 0;
    Uint32 now;

    now = SDL_GetTicks();
    if (next_time <= now)
    {
        next_time = now + TICK_INTERVAL;
        return 0;
    }
    return next_time - now;
}

extern void Init_Stage_Basic();
extern void Init_Stage1();
extern void Init_Stage2();
extern void Init_Stage3();
extern void Init_Stage4();
extern void Init_Stage5();
extern void Init_Stage6();

void Init()
{
    switch (Level.Get_Stage())
    {
    case 1:
        Init_Stage1();
        break;
    case 2:
        Init_Stage2();
        break;
    case 3:
        Init_Stage3();
        break;
    case 4:
        Init_Stage4();
        break;
    case 5:
        Init_Stage5();
        break;
    case 6:
        Init_Stage6();
        break;
    default:
        Init_Stage1();
        break;
    }
}

void UnInit()
{
    unsigned int n;                     // Counter variable

    SDL_FreeSurface(LifeSprite);
    SDL_FreeSurface(FuelFullSprite);
    SDL_FreeSurface(FuelEmptySprite);

    SDL_FreeSurface(Score1Sprite);
    SDL_FreeSurface(Score2Sprite);
    SDL_FreeSurface(Score3Sprite);
    SDL_FreeSurface(Score4Sprite);

    SDL_FreeSurface(FuelBooty);
    SDL_FreeSurface(GoldBooty);
// Now do the individual rocket segments.
    for (n = 0; n < Level.Get_Rocket()->Get_Num_Segments(); n++)
    {
// Deallocate the rocket segments
        rocket_segment *C_Segment = Level.Get_Rocket()->Get_Rocket_Segment(n);

        SDL_FreeSurface(C_Segment->Get_Image(0));
        delete C_Segment;
    }
// Booty
    for (n = 0; n < Level.Get_Num_Booty(); n++)
    {
// Deallocate the platforms
        booty *C_Booty = Level.Get_Booty(n);

        delete C_Booty;
    }
// Platforms
    for (n = 0; n < Level.Get_Num_Platforms(); n++)
    {
// Deallocate the platforms
        entity *C_Platform;

        C_Platform = Level.Get_Platform(n);
        SDL_FreeSurface(C_Platform->Get_Image(0));
        delete C_Platform;
    }
// Monsters
    for (n = 0; n < Level.Get_Num_Monsters(); n++)
    {
// Deallocate memory for all the monsters in the game.
        monster *C_Monster;

        C_Monster = Level.Get_Monster(n);

        SDL_FreeSurface(C_Monster->Get_Image(0));
        SDL_FreeSurface(C_Monster->Get_Image(1));
        delete C_Monster;
    }
// Bullets.
    for (n = 0; n < MAX_BULLETS; n++)
    {
// Deallocate memory for all the bullets in the game.
        bullet *C_Bullet;

        C_Bullet = Level.Get_Bullet(n);

        SDL_FreeSurface(C_Bullet->Get_Image(0));
        delete C_Bullet;
    }
// Deallocate the player
    SDL_FreeSurface(Player.Get_Image(0));
    SDL_FreeSurface(Player.Get_Image(1));
    SDL_FreeSurface(Player.Get_Image(2));
    SDL_FreeSurface(Player.Get_Image(3));
    SDL_FreeSurface(Player.Get_Image(4));
    SDL_FreeSurface(Player.Get_Image(5));
    SDL_FreeSurface(Player.Get_Image(6));
    SDL_FreeSurface(Player.Get_Image(7));

// Setup the rocket!
    rocket *C_Rocket = Level.Get_Rocket();

    delete C_Rocket;

// Deallocate the current level
    SDL_FreeSurface(Level.Get_Background());

// Finished deallocation.
}

void Quit()
{
    UnInit();

    cout << "\nGame Over" << endl;
    cout << "\n\nScore: " << Player.Get_Score() << endl;
    if (SDL_JoystickOpened(0))
    {
        SDL_JoystickClose(Joystick);    // close joystick
    }

// Display random quit message.
    unsigned int Message_Num = rand() % 10;

    switch (Message_Num)
    {
    case 0:
        cout << "\nJetpac again soon!!!" << endl;
        break;
    case 1:
        cout << "\nDon't you like shooting asteroids???" << endl;
        break;
    case 2:
        cout << "\nFormatting hard disk.... nah, kidding!" << endl;
        break;
    case 3:
        cout << "\nCheck out the web site for the latest version of Jetpac" << endl;
        break;
    case 4:
        cout << "\nEmail me to let me know what you think: james.blewitt@ukonline.co.uk" << endl;
        break;
    case 5:
        cout << "\nYou'll be back...The craving will start to kick in very soon." << endl;
        break;
    case 6:
        cout << "\nLinux rocks!  Just incase you haven't already upgraded your OS." << endl;
        break;
    case 7:
        cout << "\nDon't you want to develop a healthy gaming addiction???" << endl;
        break;
    case 8:
        cout << "\nWARNING: PLAYING JETPAC MAY HAVE WASTED YOUR TIME" << endl;
        break;
    case 9:
        cout << "\nFATAL ERROR: General user behaviour error.  Please play Jetpac again to rectify!" << endl;
        break;
    }
    exit(2);
}

void Draw_Score(SDL_Surface *screen)
{
    int Score = Player.Get_Score();
    unsigned int Xpos = 350;

// Level 4
    if (Score >= 1600)
    {
        while (Score > 0)
        {
            Score -= 1600;
            if (Score >= 0)
            {
                Draw(Xpos, 0, 20, 20, Score4Sprite, screen);
                Xpos += 20;
            }
        }
        if (Score < 0)
        {
            Score += 1600;
        }
    }
// Level 3
    if (Score >= 400)
    {
        while (Score > 0)
        {
            Score -= 400;
            if (Score >= 0)
            {
                Draw(Xpos, 0, 20, 20, Score3Sprite, screen);
                Xpos += 20;
            }
        }
        if (Score < 0)
        {
            Score += 400;
        }
    }
// Level 2
    if (Score >= 100)
    {
        while (Score > 0)
        {
            Score -= 100;
            if (Score >= 0)
            {
                Draw(Xpos, 0, 20, 20, Score2Sprite, screen);
                Xpos += 20;
            }
        }
        if (Score < 0)
        {
            Score += 100;
        }
    }
// Level 1
    if (Score >= 20)
    {
        while (Score > 0)
        {
            Score -= 20;
            if (Score >= 0)
            {
                Draw(Xpos, 0, 20, 20, Score1Sprite, screen);
                Xpos += 20;
            }
        }
    }
}

void Update_Score(SDL_Surface *screen)
{
    Erase(350, 0, 290, 20, screen, Level.Get_Background());
    Update(350, 0, 290, 20, screen);
}

void Draw_Screen(SDL_Surface *screen)
{
    unsigned int n;

    for (n = 0; n < MAX_BULLETS; n++)
        if (Level.Get_Bullet(n)->Is_Alive())
        {
            Level.Get_Bullet(n)->Erase(screen, Level.Get_Background());
        }
    for (n = 0; n < Level.Get_Num_Monsters(); n++)
    {
        Level.Get_Monster(n)->Erase(screen, Level.Get_Background());
    }
// Clear the platforms
    for (n = 0; n < Level.Get_Num_Platforms(); n++)
        if (Level.Get_Platform(n)->Is_Alive())
        {
            Level.Get_Platform(n)->Erase(screen, Level.Get_Background());
        }
// Clear the rocket parts if the level has a rocket in it.
    if (Level.Is_Rocket_Level())
    {
        for (n = 0; n < Level.Get_Rocket()->Get_Num_Segments(); n++)
        {
            if (Level.Get_Rocket()->Get_Rocket_Segment(n)->Get_State() != BEING_CARRIED)
            {
                Level.Get_Rocket()->Get_Rocket_Segment(n)->Erase(screen, Level.Get_Background());
            }
        }
    }
// Clear the booty.
    for (n = 0; n < Level.Get_Num_Booty(); n++)
    {
        if (Level.Get_Booty(n)->Is_Alive())
        {
            if (Level.Get_Booty(n)->Get_State() != BEING_CARRIED)
            {
                Level.Get_Booty(n)->Erase(screen, Level.Get_Background());
            }
        }
    }
// Draw the player.
    if (!Player.Get_In_Rocket())
    {
        Player.Erase(screen, Level.Get_Background());
// Draw the player.
        Player.Draw(screen);
    }
// Draw the booty.
    for (n = 0; n < Level.Get_Num_Booty(); n++)
    {
        if (Level.Get_Booty(n)->Is_Alive())
        {
            if (Level.Get_Booty(n)->Get_State() != BEING_CARRIED)
            {
                Level.Get_Booty(n)->Draw(screen);
            }
        }
    }
// Draw the rocket parts if the level has a rocket in it.
    if (Level.Is_Rocket_Level())
    {
        for (n = 0; n < Level.Get_Rocket()->Get_Num_Segments(); n++)
        {
            if (Level.Get_Rocket()->Get_Rocket_Segment(n)->Get_State() != BEING_CARRIED)
            {
                Level.Get_Rocket()->Get_Rocket_Segment(n)->Draw(screen);
            }
        }
    }
// Draw the platforms
    for (n = 0; n < Level.Get_Num_Platforms(); n++)
        if (Level.Get_Platform(n)->Is_Alive())
        {
            Level.Get_Platform(n)->Draw(screen);
        }
// Draw_Aliens
    for (n = 0; n < Level.Get_Num_Monsters(); n++)
    {
        if (Level.Get_Monster(n)->Is_Alive())
        {
            Level.Get_Monster(n)->Draw(screen);
        }
    }
// Draw Bullets
    for (n = 0; n < MAX_BULLETS; n++)
        if (Level.Get_Bullet(n)->Is_Alive())
        {
            Level.Get_Bullet(n)->Draw(screen);
        }
// Do score
    Draw_Score(screen);
// Draw life icons.
    for (n = 0; n < Player.Get_Lives(); n++)
    {
        Draw(n * 20, 0, 20, 20, LifeSprite, screen);
    }
    for (n = 0; n < Level.Get_Rocket()->Get_Max_Fuel(); n++)
    {
        if (n < Level.Get_Rocket()->Get_Fuel())
        {
            Draw(200 + (n * 20), 0, 20, 20, FuelFullSprite, screen);
        }
        else
        {
            Draw(200 + (n * 20), 0, 20, 20, FuelEmptySprite, screen);
        }
    }
}

void Move_Bullets(SDL_Surface *screen)
{
    unsigned int n;
    bullet *C_Bullet;

    for (n = 0; n < MAX_BULLETS; n++)
    {
        C_Bullet = Level.Get_Bullet(n);
        if (C_Bullet->Is_Alive())
        {
            if (C_Bullet->Get_Direction() == LEFT)
            {
                C_Bullet->Set_Xpos(C_Bullet->Get_Xpos() - C_Bullet->Get_Speed());
            }
            if (C_Bullet->Get_Direction() == RIGHT)
            {
                C_Bullet->Set_Xpos(C_Bullet->Get_Xpos() + C_Bullet->Get_Speed());
            }
// Destroy them if they leave the game arena.
            if (C_Bullet->Get_Xpos() < 10)
            {
                C_Bullet->Kill();
                C_Bullet->Erase(screen, Level.Get_Background());
                C_Bullet->Update(screen);
            }
            if (C_Bullet->Get_Xpos() > 580)
            {
                C_Bullet->Kill();
                C_Bullet->Erase(screen, Level.Get_Background());
                C_Bullet->Update(screen);
            }
        }
    }
}

void Spawn_Booty()
{
// Only spawn booty occasionally.
    if (rand() % 500 != 2)
    {
        return;
    }
    if (!Level.Get_Rocket()->Is_Complete())
    {
        return;    // Don't drop fuel for an unfinished rocket.
    }

    unsigned int n;
    unsigned int Spare_Slot = N_DEF;
    booty *C_Booty;
    unsigned int Booty_Type;

    for (n = 0; n < Level.Get_Num_Booty(); n++)
        if (Level.Get_Booty(n)->Is_Alive() == false)
        {
            Spare_Slot = n;
        }
    if (Spare_Slot != N_DEF)
    {
// Check for what kind of booty.
        Booty_Type = rand() % 10;
        if (Booty_Type < 5)
        {
            C_Booty = new booty;        // Instantiate this element.
            C_Booty->Set_Type(FUEL);
            C_Booty->Set_Num_Frames(1);
            C_Booty->Set_Anim_Frame(0);
            C_Booty->Set_Image(FuelBooty, 0);
            C_Booty->Set_Xpos(rand() % (Level.Get_Background()->w - 50));
            C_Booty->Set_Ypos(0);       // Top of screen, ready to drop!
            C_Booty->Set_State(FALLING);
            C_Booty->Ressurect();
        }
        else if (Booty_Type < 8)
        {
            C_Booty = new booty;        // Instantiate this element.
            C_Booty->Set_Type(GOLD);
            C_Booty->Set_Num_Frames(1);
            C_Booty->Set_Anim_Frame(0);
            C_Booty->Set_Image(GoldBooty, 0);
            C_Booty->Set_Xpos(rand() % (Level.Get_Background()->w - 50));
            C_Booty->Set_Ypos(0);       // Top of screen, ready to drop!
            C_Booty->Set_State(FALLING);
            C_Booty->Ressurect();
        }
        else if (Booty_Type < 9)
        {
            C_Booty = new booty;        // Instantiate this element.
            C_Booty->Set_Type(TIME_BONUS);
            C_Booty->Set_Num_Frames(1);
            C_Booty->Set_Anim_Frame(0);
            C_Booty->Set_Image(TimeBooty, 0);
            C_Booty->Set_Xpos(rand() % (Level.Get_Background()->w - 50));
            C_Booty->Set_Ypos(0);       // Top of screen, ready to drop!
            C_Booty->Set_State(FALLING);
            C_Booty->Ressurect();
        }
        else if (Booty_Type < 10)
        {
            C_Booty = new booty;        // Instantiate this element.
            C_Booty->Set_Type(WEAPON_UPGRADE);
            C_Booty->Set_Num_Frames(1);
            C_Booty->Set_Anim_Frame(0);
            C_Booty->Set_Image(WeaponBooty, 0);
            C_Booty->Set_Xpos(rand() % (Level.Get_Background()->w - 50));
            C_Booty->Set_Ypos(0);       // Top of screen, ready to drop!
            C_Booty->Set_State(FALLING);
            C_Booty->Ressurect();
        }
        Level.Set_Booty(Spare_Slot, C_Booty);   // Update the DB
    }
}

int Collide(entity *Object1, entity *Object2)
{
    if ((Object1->Get_Ypos() >= (Object2->Get_Ypos() + Object2->Get_Image(Object2->Get_Anim_Frame())->h)) ||
            (Object1->Get_Xpos() >= (Object2->Get_Xpos() + Object2->Get_Image(Object2->Get_Anim_Frame())->w)) ||
            (Object2->Get_Ypos() >= (Object1->Get_Ypos() + Object1->Get_Image(Object1->Get_Anim_Frame())->h)) ||
            (Object2->Get_Xpos() >= (Object1->Get_Xpos() + Object1->Get_Image(Object1->Get_Anim_Frame())->w)))
    {
        return 0;
    }
    return 1;
}

void Drop_Booty(SDL_Surface *screen)
{
    unsigned int n, m;

    for (n = 0; n < Level.Get_Num_Booty(); n++)
    {
        if (Level.Get_Booty(n)->Is_Alive() == true && Level.Get_Booty(n)->Get_State() == FALLING)
        {
            Level.Get_Booty(n)->Set_Ypos(Level.Get_Booty(n)->Get_Ypos() + 1);
            // Has it gone into the rocket?
            if (Collide(Level.Get_Booty(n), Level.Get_Rocket()->Get_Rocket_Segment(
                            Level.Get_Rocket()->Get_Num_Segments() - 1)))
            {
                Level.Get_Booty(n)->Kill();
                Level.Get_Booty(n)->Erase(screen, Level.Get_Background());
                Level.Get_Booty(n)->Update(screen);
                switch (Level.Get_Booty(n)->Get_Type())
                {
                case FUEL:
                    Player.Add_Score(10);
                    Update_Score(screen);

                    // Increase the fuel in the rocket!
                    Level.Get_Rocket()->Add_Fuel(1);
                    Level.Get_Rocket()->Check_Fuel_Full();      // Is the rocket full yet?
                    break;
                case GOLD:
                    Player.Add_Score(50);
                    Update_Score(screen);
                    break;
                case TIME_BONUS:
                    Level.Set_Monster_Freeze(500);
                    break;
                case WEAPON_UPGRADE:
                    Player.Set_Super_Weapon_Count(1000);
                    // Activate alternate laser beam.
                    for (m = 0; m < MAX_BULLETS; m++)
                    {
                        Level.Get_Bullet(m)->Set_Anim_Frame(1);
                    }
                    break;
                }
            }
            // Has it landed?
            for (m = 0; m < Level.Get_Num_Platforms(); m++)
            {
                if (Collide(Level.Get_Platform(m), Level.Get_Booty(n)))
                {
                    Level.Get_Booty(n)->Set_State(UNTOUCHED);
                }
            }
            if (Level.Get_Booty(n)->Get_Ypos() + Level.Get_Booty(n)->Get_Image(0)->h >= (unsigned
                    int)Level.Get_Background()->h - 2)
            {
                Level.Get_Booty(n)->Set_State(UNTOUCHED);
            }
        }
    }
}

int Collide_From_Above(entity *Object1, entity *Object2)
{
    unsigned int YTL1 = Object1->Get_Ypos() + Object1->Get_Image(Object1->Get_Anim_Frame())->h - 5;
    unsigned int XTL1 = Object1->Get_Xpos();
    unsigned int YBR1 = Object1->Get_Ypos() + Object1->Get_Image(Object1->Get_Anim_Frame())->h;
    unsigned int XBR1 = Object1->Get_Xpos() + Object1->Get_Image(Object1->Get_Anim_Frame())->w;
    unsigned int YTL2 = Object2->Get_Ypos();
    unsigned int XTL2 = Object2->Get_Xpos();
    unsigned int YBR2 = Object2->Get_Ypos() + 5;
    unsigned int XBR2 = Object2->Get_Xpos() + Object2->Get_Image(Object2->Get_Anim_Frame())->w;

// Returns true if Object1 lands on top of Object2.
    if ((YTL1 >= YBR2) || (XTL1 >= XBR2) || (YTL2 >= YBR1) || (XTL2 >= XBR1))
    {
        return 0;
    }
    return 1;
    cout << "COLLIDED FROM ABOVE!" << endl;
}

int Weak_Collide(entity *Object1, entity *Object2)
{
    if ((Object1->Get_Ypos() >= (Object2->Get_Ypos() + Object2->Get_Image(Object2->Get_Anim_Frame())->h - 20)) ||
            (Object1->Get_Xpos() >= (Object2->Get_Xpos() + Object2->Get_Image(Object2->Get_Anim_Frame())->w - 25)) ||
            (Object2->Get_Ypos() >= (Object1->Get_Ypos() + Object1->Get_Image(Object1->Get_Anim_Frame())->h - 20)) ||
            (Object2->Get_Xpos() >= (Object1->Get_Xpos() + Object1->Get_Image(Object1->Get_Anim_Frame())->w - 25)))
    {
        return 0;
    }
    return 1;
}

int Bullet_Collide(entity *Object1, bullet *The_Bullet)
{
    if ((Object1->Get_Ypos() >= (The_Bullet->Get_Ypos() + The_Bullet->Get_Image(The_Bullet->Get_Anim_Frame())->h -
                                 20)) || (Object1->Get_Xpos() >= (The_Bullet->Get_Xpos() + The_Bullet->Get_Image(
                                         The_Bullet->Get_Anim_Frame())->w - 25)) || (The_Bullet->Get_Ypos() >= (Object1->Get_Ypos() + Object1->Get_Image(
                                                 Object1->Get_Anim_Frame())->h - 20)) || (The_Bullet->Get_Xpos() >= (Object1->Get_Xpos() + Object1->Get_Image(
                                                         Object1->Get_Anim_Frame())->w - 25)))
    {
        return 0;
    }
    // Confirmed hit
    return 1;
}

void Do_Lift_Off()
{
// Makes the rocket take off once it has been fueled and the player is inside.
    if (!Player.Get_In_Rocket())
    {
        return;    // Don't do this unless player is inside rocket.
    }

    unsigned int n;

// Move the rocket segments upwards.
    for (n = 0; n < Level.Get_Rocket()->Get_Num_Segments(); n++)
    {
        if (Level.Get_Rocket()->Get_Rocket_Segment(n)->Get_Ypos() >= 2)
        {
            Level.Get_Rocket()->Get_Rocket_Segment(n)->Set_Ypos(Level.Get_Rocket()->Get_Rocket_Segment(n)->Get_Ypos() -
                    1);
        }
        else
        {
            Player.Add_Score(50);
            Level.Set_Complete(true);   // Finish the level.
        }
    }
}

bool Check_Rocket_Collect(SDL_Surface *screen)
{
// Check to see if the player has collected a rocket part, or got into finished rocket.
    unsigned int n;

    if (Player.Get_In_Rocket())
    {
        return 0;    // Exit if the player is already in the rocket.
    }
    if (Level.Get_Rocket()->Is_Fuel_Full())
    {
        for (n = 0; n < Level.Get_Rocket()->Get_Num_Segments(); n++)
        {
            if (Collide(&Player, Level.Get_Rocket()->Get_Rocket_Segment(n)))
            {
                Player.Set_In_Rocket(true);
            }
        }
        if (Player.Get_In_Rocket())
        {
            Player.Erase(screen, Level.Get_Background());
            Player.Update(screen);
            // Make the rocket take off!
            for (n = 0; n < Level.Get_Rocket()->Get_Num_Segments(); n++)
            {
                Level.Get_Rocket()->Get_Rocket_Segment(n)->Set_State(TAKING_OFF);
            }
            // Kill all the monsters;
            for (n = 0; n < Level.Get_Num_Monsters(); n++)
            {
                Level.Get_Monster(n)->Kill();
                Level.Get_Monster(n)->Set_Behaviour(STATIC);
                // Level.Get_Monster(n)->Erase(
            }
        }
    }
    if (Player.Get_Carrying())
    {
        return false;    // Can't collect stuff if already
    }
    // carrying.
// n is initially 1 because the base unit is never collected!
    for (n = 1; n < Level.Get_Rocket()->Get_Num_Segments(); n++)
    {
        if (Level.Get_Rocket()->Get_Rocket_Segment(n)->Get_State() == UNTOUCHED)
        {
            if (Level.Get_Rocket()->Get_Rocket_Segment(n - 1)->Get_State() == ON_ROCKET)
            {
                if (Collide(&Player, Level.Get_Rocket()->Get_Rocket_Segment(n)))
                {
                    Level.Get_Rocket()->Get_Rocket_Segment(n)->Set_State(BEING_CARRIED);
                    Level.Get_Rocket()->Get_Rocket_Segment(n)->Erase(screen, Level.Get_Background());
                    Level.Get_Rocket()->Get_Rocket_Segment(n)->Update(screen);
                    // Set the player to carry the pack.
                    Player.Set_Carrying(true);
                    return true;
                }
            }
        }
    }
    return false;
}

bool Check_Booty_Collect(SDL_Surface *screen)
{
// Check to see if the player has collected some booty.
    unsigned int n, m;

    if (Player.Get_Carrying())
    {
        return false;    // Can't collect stuff if already
    }
    // carrying.
    for (n = 0; n < Level.Get_Num_Booty(); n++)
    {
        if (Level.Get_Booty(n)->Is_Alive())
        {
            if (Level.Get_Booty(n)->Get_State() == UNTOUCHED)
            {
                if (Collide(&Player, Level.Get_Booty(n)))
                {
                    switch (Level.Get_Booty(n)->Get_Type())
                    {
                    case FUEL:
                        Level.Get_Booty(n)->Set_State(BEING_CARRIED);
                        Level.Get_Booty(n)->Erase(screen, Level.Get_Background());
                        Level.Get_Booty(n)->Update(screen);
                        // Set the player to carry the pack.
                        Player.Set_Carrying(true);
                        return true;
                        break;
                    case GOLD:
                        Level.Get_Booty(n)->Kill();
                        Level.Get_Booty(n)->Erase(screen, Level.Get_Background());
                        Level.Get_Booty(n)->Update(screen);
                        Player.Add_Score(50);
                        Update_Score(screen);
                        return true;
                        break;
                    case TIME_BONUS:
                        Level.Get_Booty(n)->Kill();
                        Level.Get_Booty(n)->Erase(screen, Level.Get_Background());
                        Level.Get_Booty(n)->Update(screen);
                        Level.Set_Monster_Freeze(500);
                        return true;
                        break;
                    case WEAPON_UPGRADE:
                        Level.Get_Booty(n)->Kill();
                        Level.Get_Booty(n)->Erase(screen, Level.Get_Background());
                        Level.Get_Booty(n)->Update(screen);
                        Player.Set_Super_Weapon_Count(1000);
                        // Activate alternate laser beam.
                        for (m = 0; m < MAX_BULLETS; m++)
                        {
                            Level.Get_Bullet(m)->Set_Anim_Frame(1);
                        }
                        return true;
                        break;
                    }
                }
            }
        }
    }
    return false;
}

void Check_Rocket_Drop()
{
    unsigned int n;
    rocket_segment *C_Segment;

    if (!Player.Get_Carrying())
    {
        return;    // Can't drop anything if not carrying
    }
    for (n = 1; n < Level.Get_Rocket()->Get_Num_Segments(); n++)
    {
        C_Segment = Level.Get_Rocket()->Get_Rocket_Segment(n);
        if (C_Segment->Get_State() == BEING_CARRIED)
        {
            if (Player.Get_Xpos() == Level.Get_Rocket()->Get_Rocket_Segment(0)->Get_Xpos())
            {
                if (Player.Get_Ypos() <= Level.Get_Rocket()->Get_Rocket_Segment(0)->Get_Ypos() - ((50 * n) + 50))
                {
                    C_Segment->Set_State(FALLING);
                    C_Segment->Set_Xpos(Player.Get_Xpos());
                    C_Segment->Set_Ypos(Player.Get_Ypos() + 50);
                    Player.Set_Carrying(false);
                }
            }
        }
    }
}

void Check_Booty_Drop()
{
    unsigned int n;
    booty *C_Booty;

    if (!Player.Get_Carrying())
    {
        return;    // Can't drop anything if not carrying
    }
    for (n = 0; n < Level.Get_Num_Booty(); n++)
    {
        C_Booty = Level.Get_Booty(n);
        if (C_Booty->Get_State() == BEING_CARRIED)
        {
            if (Player.Get_Xpos() == Level.Get_Rocket()->Get_Rocket_Segment(0)->Get_Xpos())
            {
                if (Player.Get_Ypos() <= Level.Get_Rocket()->Get_Rocket_Segment(0)->Get_Ypos() - 200)
                {
                    C_Booty->Set_State(FALLING);
                    C_Booty->Set_Xpos(Player.Get_Xpos());
                    C_Booty->Set_Ypos(Player.Get_Ypos() + 50);
                    Player.Set_Carrying(false);
                }
            }
        }
    }
}

bool Check_Land()
{
// Checks to see if player has landed on a platform.
    unsigned int n;

// Make sure player is not on a platform.
// if (!Player.Get_Standing()) {
    for (n = 0; n < Level.Get_Num_Platforms(); n++)
    {
        if (Collide_From_Above(&Player, Level.Get_Platform(n)))
        {
            Player.Set_Standing(true);
        }
        // cout << "STANDING" << endl;
        return true;
    }
// }
// cout << "NOT STANDING " << endl;
    return false;
}

bool Check_Platform_Collide()
{
    unsigned int n;

// Make sure player is not on a platform.
    for (n = 0; n < Level.Get_Num_Platforms(); n++)
    {
        if (Collide(&Player, Level.Get_Platform(n)))
        {
            return true;
        }
    }
    return false;
}

void Drop_Rocket_Segments(SDL_Surface *screen)
{
    unsigned int n;
    rocket_segment *C_Segment;

    for (n = 1; n < Level.Get_Rocket()->Get_Num_Segments(); n++)
    {
        C_Segment = Level.Get_Rocket()->Get_Rocket_Segment(n);
        if (C_Segment->Get_State() == FALLING)
        {
            if (C_Segment->Get_Ypos() <= Level.Get_Rocket()->Get_Rocket_Segment(0)->Get_Ypos() - (50 * n))
            {
                C_Segment->Erase(screen, Level.Get_Background());
                // Update the screen immediately above this segment.  This gets
                // rid of screen corruption.
                Update(C_Segment->Get_Xpos(), C_Segment->Get_Ypos() - 1, C_Segment->Get_Image(0)->w, 1, screen);
                C_Segment->Set_Ypos(C_Segment->Get_Ypos() + 1);
                C_Segment->Draw(screen);
            }
            else
            {
                C_Segment->Set_State(ON_ROCKET);
                Level.Get_Rocket()->Check_Complete();   // is the rocket now finished?
                Player.Add_Score(10);
                Update_Score(screen);
            }
        }
    }
}

void Move_Baddies(SDL_Surface *screen)
{
    unsigned int rnd;
    unsigned int n, m;
    monster *C_Monster;
    monster *D_Monster;
    unsigned int M_Speed;
    bool move = true;

    for (n = 0; n < Level.Get_Num_Monsters(); n++)
    {
        C_Monster = Level.Get_Monster(n);       // Find the current monster.
        M_Speed = C_Monster->Get_Speed();
        if (Level.Monsters_Frozen())
        {
            return;    // don't move monsters when frozen.
        }
// Allow monsters to spawn spores
        if (C_Monster->Get_Can_Spore())
        {
            C_Monster->Dec_Spore_Counter();
            if (C_Monster->Spore_Ready() && C_Monster->Is_Alive())
            {
                for (m = n; m < Level.Get_Num_Monsters(); m++)
                {
                    if (Level.Get_Monster(m)->Get_Spore() && !Level.Get_Monster(m)->Is_Alive())
                    {
                        Level.Get_Monster(m)->Ressurect();
                        Level.Get_Monster(m)->Set_Xpos(C_Monster->Get_Xpos() + 20);
                        Level.Get_Monster(m)->Set_Ypos(C_Monster->Get_Ypos() + 30);
                        Level.Get_Monster(m)->Set_Hit_Pts(Level.Get_Monster(m)->Get_Max_Hit_Pts());
                        C_Monster->Reset_Spore_Counter();
                    }
                }
            }
        }
        switch (C_Monster->Get_Behaviour())
        {
        case STATIC:
            // For static monsters, no movement is required.
            break;

        case ASTEROID:
            move = true;
            for (m = 0; m < n; m++)
            {
                D_Monster = Level.Get_Monster(m);
                if (Collide(C_Monster, D_Monster) && n != m & D_Monster->Is_Alive() && D_Monster->Get_Behaviour() ==
                        ASTEROID)
                {
                    // Prevent this asteroid from moving.
                    move = false;
                }
            }
            if (!move)
            {
                break;
            }
            if (C_Monster->Get_Xpos() > 0)
            {
                C_Monster->Set_Xpos(C_Monster->Get_Xpos() - C_Monster->Get_Speed());
            }
            if (C_Monster->Get_Xpos() < 3)
            {
                C_Monster->Erase(screen, Level.Get_Background());
                C_Monster->Update(screen);
                C_Monster->Set_Xpos(Level.Get_Background()->w);
                C_Monster->Set_Ypos(rand() % Level.Get_Background()->h);
                // Re-init health
                C_Monster->Set_Hit_Pts(C_Monster->Get_Max_Hit_Pts());
                C_Monster->Set_Anim_Frame(0);
                C_Monster->Ressurect();
            }
            rnd = rand() % 4;
            if (rnd == 0)
            {
                C_Monster->Set_Ypos(C_Monster->Get_Ypos() + 1);
            }
            if (C_Monster->Get_Ypos() > (unsigned int)Level.Get_Background()->h - 20)
            {
                C_Monster->Erase(screen, Level.Get_Background());
                C_Monster->Update(screen);
                C_Monster->Set_Xpos(Level.Get_Background()->w);
                C_Monster->Set_Ypos(rand() % Level.Get_Background()->h);
            }
            break;

        case BEZERK:
            if (!C_Monster->Is_Alive())
            {
                rnd = rand() % 270;
                if (rnd == 0 & !C_Monster->Get_Spore())         // then respawn (don't respawn spores!)
                {
                    // Respawn at the bottom of the screen and at the side!
                    rnd = rand() % 2;
                    if (rnd == 0)
                    {
                        C_Monster->Set_Ypos(rand() % (Level.Get_Background()->h + 5));
                        C_Monster->Set_Xpos(640);
                        C_Monster->Set_Hit_Pts(C_Monster->Get_Max_Hit_Pts());
                        C_Monster->Set_Anim_Frame(0);
                        C_Monster->Ressurect();
                    }
                    else
                    {
                        C_Monster->Set_Ypos(480);
                        C_Monster->Set_Xpos(rand() % (Level.Get_Background()->w));
                        C_Monster->Set_Hit_Pts(C_Monster->Get_Max_Hit_Pts());
                        C_Monster->Set_Anim_Frame(0);
                        C_Monster->Ressurect();
                    }
                }
            }
            else
            {
                rnd = rand() % 8;
                switch (rnd)
                {
                case 0:
                    if (C_Monster->Get_Xpos() > 0)
                    {
                        C_Monster->Set_Xpos(C_Monster->Get_Xpos() - M_Speed);
                        for (m = 0; m < Level.Get_Num_Monsters(); m++)
                        {
                            D_Monster = Level.Get_Monster(m);
                            if (!C_Monster->Get_Spore())
                            {
                                if (Collide(C_Monster, D_Monster) & n != m & D_Monster->Is_Alive())
                                {
                                    C_Monster->Set_Xpos(C_Monster->Get_Xpos() + M_Speed);    // undo this move.
                                }
                            }
                        }
                    }
                    break;
                case 1:
                    if (C_Monster->Get_Xpos() < 590)
                    {
                        C_Monster->Set_Xpos(C_Monster->Get_Xpos() + M_Speed);
                        for (m = 0; m < Level.Get_Num_Monsters(); m++)
                        {
                            D_Monster = Level.Get_Monster(m);
                            if (!C_Monster->Get_Spore())
                            {
                                if (Collide(C_Monster, D_Monster) & n != m && D_Monster->Is_Alive())
                                {
                                    C_Monster->Set_Xpos(C_Monster->Get_Xpos() - M_Speed);    // undo this move.
                                }
                            }
                        }
                    }
                    break;
                case 2:
                    if (C_Monster->Get_Ypos() > 0)
                    {
                        C_Monster->Set_Ypos(C_Monster->Get_Ypos() - M_Speed);
                        for (m = 0; m < Level.Get_Num_Monsters(); m++)
                        {
                            D_Monster = Level.Get_Monster(m);
                            if (!C_Monster->Get_Spore())
                            {
                                if (Collide(C_Monster, D_Monster) & n != m && D_Monster->Is_Alive())
                                {
                                    C_Monster->Set_Ypos(C_Monster->Get_Ypos() + M_Speed);    // undo this move.
                                }
                            }
                        }
                    }
                    break;
                case 3:
                    if (C_Monster->Get_Ypos() < 430)
                    {
                        C_Monster->Set_Ypos(C_Monster->Get_Ypos() + M_Speed);
                        for (m = 0; m < Level.Get_Num_Monsters(); m++)
                        {
                            D_Monster = Level.Get_Monster(m);
                            if (!C_Monster->Get_Spore())
                            {
                                if (Collide(C_Monster, D_Monster) && n != m & D_Monster->Is_Alive())
                                {
                                    C_Monster->Set_Ypos(C_Monster->Get_Ypos() - M_Speed);    // undo this move.
                                }
                            }
                        }
                    }
                    break;
                case 4:
                case 5:
                case 6:
                    if (C_Monster->Get_Ypos() > Player.Get_Ypos())
                    {
                        C_Monster->Set_Ypos(C_Monster->Get_Ypos() - M_Speed);
                        for (m = 0; m < Level.Get_Num_Monsters(); m++)
                        {
                            D_Monster = Level.Get_Monster(m);
                            if (!C_Monster->Get_Spore())
                            {
                                if (Collide(C_Monster, D_Monster) && n != m & D_Monster->Is_Alive())
                                {
                                    C_Monster->Set_Ypos(C_Monster->Get_Ypos() + M_Speed);    // undo this move.
                                }
                            }
                        }
                    }
                    if (C_Monster->Get_Ypos() < Player.Get_Ypos())
                    {
                        C_Monster->Set_Ypos(C_Monster->Get_Ypos() + M_Speed);
                        for (m = 0; m < Level.Get_Num_Monsters(); m++)
                        {
                            if (!C_Monster->Get_Spore())
                            {
                                D_Monster = Level.Get_Monster(m);
                                if (Collide(C_Monster, D_Monster) && n != m & D_Monster->Is_Alive())
                                {
                                    C_Monster->Set_Ypos(C_Monster->Get_Ypos() - M_Speed);    // undo this move.
                                }
                            }
                        }
                    }
                    if (C_Monster->Get_Xpos() > Player.Get_Xpos())
                    {
                        C_Monster->Set_Xpos(C_Monster->Get_Xpos() - M_Speed);
                        for (m = 0; m < Level.Get_Num_Monsters(); m++)
                        {
                            D_Monster = Level.Get_Monster(m);
                            if (!C_Monster->Get_Spore())
                            {
                                if (Collide(C_Monster, D_Monster) & n != m & D_Monster->Is_Alive())
                                {
                                    C_Monster->Set_Xpos(C_Monster->Get_Xpos() + M_Speed);    // undo this move.
                                }
                            }
                        }
                    }
                    if (C_Monster->Get_Xpos() < Player.Get_Xpos())
                    {
                        C_Monster->Set_Xpos(C_Monster->Get_Xpos() + M_Speed);
                        for (m = 0; m < Level.Get_Num_Monsters(); m++)
                        {
                            D_Monster = Level.Get_Monster(m);
                            if (!C_Monster->Get_Spore())
                            {
                                if (Collide(C_Monster, D_Monster) & n != m && D_Monster->Is_Alive())
                                {
                                    C_Monster->Set_Xpos(C_Monster->Get_Xpos() - M_Speed);    // undo this move.
                                }
                            }
                        }
                    }
                    break;
                }
            }
            break;

        case HOMING:
            rnd = rand() % 3;
            if (rnd == 0)
            {
                if (C_Monster->Get_Ypos() > Player.Get_Ypos())
                {
                    C_Monster->Set_Ypos(C_Monster->Get_Ypos() - M_Speed);
                    for (m = 0; m < Level.Get_Num_Monsters(); m++)
                    {
                        D_Monster = Level.Get_Monster(m);
                        if (!C_Monster->Get_Spore())
                        {
                            if (Collide(C_Monster, D_Monster) && n != m & D_Monster->Is_Alive())
                            {
                                C_Monster->Set_Ypos(C_Monster->Get_Ypos() + M_Speed);    // undo this move.
                            }
                        }
                    }
                }
                if (C_Monster->Get_Ypos() < Player.Get_Ypos())
                {
                    C_Monster->Set_Ypos(C_Monster->Get_Ypos() + M_Speed);
                    for (m = 0; m < Level.Get_Num_Monsters(); m++)
                    {
                        D_Monster = Level.Get_Monster(m);
                        if (!C_Monster->Get_Spore())
                        {
                            if (Collide(C_Monster, D_Monster) & n != m && D_Monster->Is_Alive())
                            {
                                C_Monster->Set_Ypos(C_Monster->Get_Ypos() - M_Speed);    // undo this move.
                            }
                        }
                    }
                }
                if (C_Monster->Get_Xpos() > Player.Get_Xpos())
                {
                    C_Monster->Set_Xpos(C_Monster->Get_Xpos() - M_Speed);
                    for (m = 0; m < Level.Get_Num_Monsters(); m++)
                    {
                        D_Monster = Level.Get_Monster(m);
                        if (!C_Monster->Get_Spore())
                        {
                            if (Collide(C_Monster, D_Monster) & n != m && D_Monster->Is_Alive())
                            {
                                C_Monster->Set_Xpos(C_Monster->Get_Xpos() + M_Speed);    // undo this move.
                            }
                        }
                    }
                }
                if (C_Monster->Get_Xpos() < Player.Get_Xpos())
                {
                    C_Monster->Set_Xpos(C_Monster->Get_Xpos() + M_Speed);
                    for (m = 0; m < Level.Get_Num_Monsters(); m++)
                    {
                        D_Monster = Level.Get_Monster(m);
                        if (!C_Monster->Get_Spore())
                        {
                            if (Collide(C_Monster, D_Monster) & n != m && D_Monster->Is_Alive())
                            {
                                C_Monster->Set_Xpos(C_Monster->Get_Xpos() - M_Speed);    // undo this move.
                            }
                        }
                    }
                }
            }
            break;
        }
    }
}

void Fire_Bullet(unsigned int Xpos, unsigned int Ypos, unsigned char Direction)
{
    if (Player.Get_Carrying())
    {
        return;    // cant fire if carrying stuff.
    }
    if (Player.Get_Reload_Step() < Player.Get_Reload_Speed())
    {
        return;
    }

    unsigned int m;

// De-activate super laser if needed.
    if (!Player.Has_Super_Weapon())
    {
        for (m = 0; m < MAX_BULLETS; m++)
        {
            Level.Get_Bullet(m)->Set_Anim_Frame(0);
        }
    }
// Make sure player isn't too close to the edge of the screen.
    if (Player.Get_Xpos() <= 50 && Player.Get_Direction() == LEFT)
    {
        return;
    }
    if (Player.Get_Xpos() >= (unsigned int)Level.Get_Background()->w - 50 && Player.Get_Direction() == RIGHT)
    {
        return;
    }

// Find an inactive bullet.
    unsigned int n;
    unsigned int Current_Bullet = 0;
    unsigned char Found = 0;

    for (n = 0; n < MAX_BULLETS; n++)
    {
        if (!Level.Get_Bullet(n)->Is_Alive())
        {
            Current_Bullet = n;
            Found = 1;
        }
    }
    if (Found)
    {
        bullet *C_Bullet = Level.Get_Bullet(Current_Bullet);

        C_Bullet->Set_Xpos(Xpos);
        C_Bullet->Set_Ypos(Ypos);
        C_Bullet->Set_Direction(Direction);
        C_Bullet->Ressurect();
        Level.Set_Bullet(Current_Bullet, C_Bullet);
        if (Level.Get_Bullet(Current_Bullet)->Is_Alive())
// Make the player wait for reload
        {
            Player.Set_Reload_Step(0);
        }
    }
}

void Check_Kill(SDL_Surface *screen)
{
// Check to see if an alien has hit the good guy.
    unsigned int n;

    for (n = 0; n < Level.Get_Num_Monsters(); n++)
    {
        if (Weak_Collide(Level.Get_Monster(n), &Player) && !Player.Get_Falling() && Level.Get_Monster(n)->Is_Alive())
        {
            if (Player.Get_Lives() >= 1)
            {
                // Destroy monster
                Level.Get_Monster(n)->Kill();
                Level.Get_Monster(n)->Erase(screen, Level.Get_Background());
                Level.Get_Monster(n)->Update(screen);
                Player.Set_Lives(Player.Get_Lives() - 1);
                Erase(0, 0, 100, 20, screen, Level.Get_Background());
                Update(0, 0, 100, 20, screen);
                Player.Set_Falling(true);       // Make player fall down.
            }
            else
            {
                Player.Kill();
            }
        }
    }
}

void Check_Bullet_Hits(SDL_Surface *screen)
{
// Check to see if a bullet has hit an alien, if so, damage.
    unsigned int n, m;
    monster *C_Monster;
    bullet *C_Bullet;

    for (n = 0; n < Level.Get_Num_Monsters(); n++)
    {
        C_Monster = Level.Get_Monster(n);
        for (m = 0; m < MAX_BULLETS; m++)
        {
            C_Bullet = Level.Get_Bullet(m);
            if (Collide(C_Monster, C_Bullet) && C_Bullet->Is_Alive() && C_Monster->Is_Alive())
            {
                if (Player.Has_Super_Weapon())
                {
                    C_Monster->Set_Anim_Frame(1);
                    C_Monster->Kill();
                    C_Monster->Erase(screen, Level.Get_Background());
                    C_Monster->Update(screen);
                    Player.Add_Score(5);
                    Update_Score(screen);
                }
                // Destroy alien.
                if (C_Monster->Get_Hit_Pts() > 0)
                {
                    C_Monster->Set_Hit_Pts(C_Monster->Get_Hit_Pts() - 1);
                    Player.Add_Score(1);
                    Update_Score(screen);
                }
                else
                {
                    C_Monster->Set_Anim_Frame(1);
                    C_Monster->Kill();
                    C_Monster->Erase(screen, Level.Get_Background());
                    C_Monster->Update(screen);
                    Player.Add_Score(5);
                    Update_Score(screen);
                }
                // Now destroy the bullet.
                C_Bullet->Kill();
                C_Bullet->Erase(screen, Level.Get_Background());
                C_Bullet->Update(screen);
            }
        }
    }
}

void Game_Loop(SDL_Surface *screen)
{
    Level.Draw_Background(screen);

    SDL_Event event;
    Uint8 *keys;                        // Saves the keys.

    while (Player.Is_Alive() && !Level.Is_Complete())
    {
        SDL_PollEvent(&event);
        keys = SDL_GetKeyState(NULL);
        if (Player.Get_Direction() == LEFT)
        {
            if (!Player.Get_Carrying())
            {
                Player.Set_Anim_Frame(1);
            }
            else
            {
                Player.Set_Anim_Frame(5);
            }
        }
        if (Player.Get_Direction() == RIGHT)
        {
            if (!Player.Get_Carrying())
            {
                Player.Set_Anim_Frame(0);
            }
            else
            {
                Player.Set_Anim_Frame(4);
            }
        }
// Check_Land();
// Gravity.
        if (Player.Get_Ypos() < 430)
        {
            Player.Set_Ypos(Player.Get_Ypos() + 1);
            if (Check_Platform_Collide())
            {
                Player.Set_Ypos(Player.Get_Ypos() - 1);
                Player.Set_Standing(true);
                Player.Set_Falling(false);
            }
        }
        if (Player.Get_Ypos() >= 430)
        {
            Player.Set_Falling(false);
        }
        if ((keys[SDLK_DOWN] || SDL_JoystickGetAxis(Joystick, 1) > 3000) && (Player.Get_Ypos() < 430) &&
                !Player.Get_Falling())
        {
            Player.Set_Ypos(Player.Get_Ypos() + 1);
            if (Check_Platform_Collide())
            {
                Player.Set_Ypos(Player.Get_Ypos() - 1);
                Player.Set_Standing(true);
            }
        }
        if ((keys[SDLK_RIGHT] || SDL_JoystickGetAxis(Joystick, 0) > 3000) && (Player.Get_Xpos() < 590) &&
                !Player.Get_Falling())
        {
            Player.Set_Direction(RIGHT);
            Player.Set_Xpos(Player.Get_Xpos() + 1);
            // Player.Set_Standing(false);
            if (Check_Platform_Collide())
            {
                Player.Set_Xpos(Player.Get_Xpos() - 1);
            }
        }
        if ((keys[SDLK_LEFT] || SDL_JoystickGetAxis(Joystick, 0) < -3000) && (Player.Get_Xpos() > 0) &&
                !Player.Get_Falling())
        {
            Player.Set_Direction(LEFT);
            Player.Set_Xpos(Player.Get_Xpos() - 1);
            // Player.Set_Standing(false);
            if (Check_Platform_Collide())
            {
                Player.Set_Xpos(Player.Get_Xpos() + 1);
            }
        }
        if ((keys[SDLK_UP] || SDL_JoystickGetAxis(Joystick, 1) < -3000) && (Player.Get_Ypos() > 20) &&
                !Player.Get_Falling())
        {
            // Do rocket pack.
            if (Player.Get_Direction() == LEFT)
            {
                if (!Player.Get_Carrying())
                {
                    Player.Set_Anim_Frame(3);
                }
                else
                {
                    Player.Set_Anim_Frame(7);
                }
            }
            if (Player.Get_Direction() == RIGHT)
            {
                if (!Player.Get_Carrying())
                {
                    Player.Set_Anim_Frame(2);
                }
                else
                {
                    Player.Set_Anim_Frame(6);
                }
            }
            Player.Set_Standing(false);
            Player.Set_Ypos(Player.Get_Ypos() - 2);
            if (Check_Platform_Collide())
            {
                Player.Set_Ypos(Player.Get_Ypos() + 2);
            }
        }
        if ((keys[SDLK_SPACE] || SDL_JoystickGetButton(Joystick, 0)) && !Player.Get_Falling())
        {
            if (Player.Get_Direction() == RIGHT)
            {
                Fire_Bullet(Player.Get_Xpos() + 20, Player.Get_Ypos() + 32, RIGHT);
            }
            else
            {
                Fire_Bullet(Player.Get_Xpos() - 20, Player.Get_Ypos() + 32, LEFT);
            }
        }
        /*if (keys[SDLK_c]) {
         *      SDL_SaveBMP(screen, "screenshot.bmp");
         *      }*/
        if (keys[SDLK_ESCAPE])
        {
            Player.Kill();
        }
        if (keys[SDLK_t])
        {
            SDL_WM_ToggleFullScreen(screen);
        }
        if (keys[SDLK_9])
        {
            if (!FullScreenMode)
            {
                videoflags = SDL_HWSURFACE | SDL_HWPALETTE | SDL_FULLSCREEN;
                ScreenModeChanged = true;
                FullScreenMode = true;
            }
        }
        if (keys[SDLK_0])
        {
            if (FullScreenMode)
            {
                videoflags = SDL_HWSURFACE | SDL_HWPALETTE;
                ScreenModeChanged = true;
                FullScreenMode = false;
            }
        }
        if (Player.Get_Falling())
        {
            Player.Set_Anim_Frame(8);
        }
        if (ScreenModeChanged)
        {
            screen = SDL_SetVideoMode(640, 480, 16, videoflags);
            ScreenModeChanged = false;
            Level.Draw_Background(screen);
        }
        Draw_Screen(screen);            // Draw the screen.
        Move_Baddies(screen);
        Move_Bullets(screen);
        Do_Lift_Off();
        Check_Bullet_Hits(screen);
        Check_Kill(screen);
        Check_Rocket_Collect(screen);
        Check_Booty_Collect(screen);
        Check_Rocket_Drop();
        Check_Booty_Drop();
        Drop_Booty(screen);
        Spawn_Booty();
        Drop_Rocket_Segments(screen);
        Level.Dec_Monster_Freeze();             // Start to unfreeze monsters.
        Player.Dec_Super_Weapon_Count();        // Make super weapon wear off eventually.
// Increment reload times
        if (Player.Get_Reload_Step() < Player.Get_Reload_Speed() + 10)
        {
            Player.Set_Reload_Step(Player.Get_Reload_Step() + 1);
        }
        SDL_Delay(TimeLeft());          // Use timer to slow things down a little.
    }
}

void Draw_Messages(SDL_Surface *screen)
{
    int stage;
    SDL_Surface *Stage_Box;

    stage = Level.Get_Stage();
    if (stage == 1)
    {
        return;    // Don't tell them that it is stage 1.
    }
// Scale it down so that it is 1 to 5.
    switch (stage)
    {
    case 6:
        Stage_Box = LoadBMP("pixmaps/aa2.dat");
        Draw(250, 150, Stage_Box->w, Stage_Box->h, Stage_Box, screen);
        break;
    case 11:
        Stage_Box = LoadBMP("pixmaps/aa3.dat");
        Draw(250, 150, Stage_Box->w, Stage_Box->h, Stage_Box, screen);
        break;
    case 16:
        Stage_Box = LoadBMP("pixmaps/aa4.dat");
        Draw(250, 150, Stage_Box->w, Stage_Box->h, Stage_Box, screen);
        break;
    case 21:
        Stage_Box = LoadBMP("pixmaps/aa5.dat");
        Draw(250, 150, Stage_Box->w, Stage_Box->h, Stage_Box, screen);
        break;
    }
    while (stage > 5)
    {
        stage -= 5;
    }
    switch (stage)
    {
    case 1:
        Stage_Box = LoadBMP("pixmaps/stage1.bmp");
        Draw(250, 200, Stage_Box->w, Stage_Box->h, Stage_Box, screen);
        break;
    case 2:
        Stage_Box = LoadBMP("pixmaps/stage2.bmp");
        Draw(250, 200, Stage_Box->w, Stage_Box->h, Stage_Box, screen);
        break;
    case 3:
        Stage_Box = LoadBMP("pixmaps/stage3.bmp");
        Draw(250, 200, Stage_Box->w, Stage_Box->h, Stage_Box, screen);
        break;
    case 4:
        Stage_Box = LoadBMP("pixmaps/stage4.bmp");
        Draw(250, 200, Stage_Box->w, Stage_Box->h, Stage_Box, screen);
        break;
    case 5:
        Stage_Box = LoadBMP("pixmaps/stage5.bmp");
        Draw(250, 200, Stage_Box->w, Stage_Box->h, Stage_Box, screen);
        break;
    default:
        break;
    }

// Wait for a key press!
    SDL_Event event;
    Uint8 *keys;

    while (1)
    {
        SDL_WaitEvent(&event);
        keys = SDL_GetKeyState(NULL);
        if (!keys[SDLK_SPACE] || !keys[SDLK_TAB] || !keys[SDLK_ESCAPE])
        {
            return;
        }
    }
}

extern void Menu_Screen(SDL_Surface *screen);

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
    {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);
    SDL_WM_SetCaption("Jetpac - The Return", NULL);

    Uint8 iconmask[128] = { 0x00, 0x7f, 0x80, 0x00, 0x00, 0xff, 0xe0, 0x00, 0x01, 0xff, 0xf0, 0x00, 0x03, 0xff, 0xf8,
                            0x00, 0x03, 0xff, 0xfc, 0x00, 0x07, 0xff, 0xfc, 0x00, 0x07, 0xff, 0xfe, 0x00, 0x0f, 0xff,
                            0xfe, 0x00, 0x0f,
                            0xff, 0xfe, 0x00, 0x0f, 0xff, 0xfe, 0x00, 0x0f, 0xff, 0xfe, 0x00, 0x0f, 0xff, 0xfc, 0x00,
                            0x0f, 0xff, 0xfc,
                            0x00, 0x0f, 0xff, 0xfc, 0x00, 0x07, 0xff, 0xfc, 0x00, 0x0f, 0xff, 0xf8, 0x00, 0x0f, 0xff,
                            0xf0, 0x00, 0x0f,
                            0xff, 0xf0, 0x00, 0x0f, 0xff, 0xf0, 0x00, 0x0f, 0xff, 0xff, 0xc0, 0x0f, 0xff, 0xff, 0xe0,
                            0x0f, 0xff, 0xfe,
                            0x00, 0x0f, 0xff, 0xfc, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x00, 0x3f,
                            0xf8, 0x00, 0x00,
                            0x7f, 0xfc, 0x00, 0x00, 0x7f, 0xfc, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x1f, 0xfc, 0x00,
                            0x00, 0x3f, 0xf8,
                            0x00, 0x00, 0x1f, 0xe0, 0x00
                          };

    SDL_WM_SetIcon(SDL_LoadBMP("icon.bmp"), iconmask);  // Setup Icon

    SDL_Surface *screen;

    videoflags = SDL_HWSURFACE | SDL_HWPALETTE;
    screen = SDL_SetVideoMode(640, 480, 16, videoflags);        // |SDL_FULLSCREEN);
    if (screen == NULL)
    {
        fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_ShowCursor(false);              // Hide the cursor.
    if (SDL_NumJoysticks() > 0)         // open joystick
    {
        Joystick = SDL_JoystickOpen(0);
        // SDL_JoystickEventState(SDL_IGNORE);
    }

    time_t MyTime = time(NULL);

    srand(int(MyTime));

Menu:
    Menu_Screen(screen);
    Level.Set_Stage(Initial_Stage);

    Player.Reset_Score();
    Player.Set_Lives(2);

Init:
    Init();

    Draw_Messages(screen);

    Game_Loop(screen);
    if (!Player.Is_Alive())
    {
        goto Menu;
    }
    if (Level.Is_Complete())
    {
        UnInit();
        Level.Set_Stage(Level.Get_Stage() + 1);
        goto Init;
    }
    return 0;
}
