// ==============================================================================
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// ==============================================================================

// ==============================================================================
// File: Main.cpp
// Author: Jarmo Hekkanen <jarski@2ndpoint.fi>
// Copyright (c) Jarmo Hekkanen 2003 <jarski@2ndpoint.fi>
// ------------------------------------------------------------------------------
// Includes
#include <iostream>
#include <stdexcept>
#include <string>
#include <list>
#include <fstream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdlib.h>
#include <time.h>
#include "cParticleSystem.hpp"
#include "Config.hpp"
#include "Main.hpp"
#ifdef SOUND
#include <SDL/SDL_mixer.h>
#endif
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
// ==============================================================================
using namespace std;

// Surfaces
SDL_Surface *pScreen = NULL;
SDL_Surface *pBackground = NULL;
SDL_Surface *pBitmaps = NULL;

// Marine animation
Uint32 MarineLastFrame = 0;
Uint8 MarineFrame = 0;

// Tank animation
Uint32 TankLastFrame = 0;
Uint8 TankFrame = 0;

#ifdef SOUND

Mix_Chunk *pGunSound = NULL;
int GunChannel = 1;
Mix_Chunk *pRicochetSound = NULL;
Mix_Chunk *pExplosionSound = NULL;
Mix_Chunk *pDeathSound = NULL;

#endif

list<UnitData> Tanks;
list<UnitData> Marines;
list<Grenade> Grenades;
list<cParticleSystem> ParticleSystems;

// Weapons
Uint8 GrenadeAmmo = 3;

// Ammo box
bool AmmoBoxVisible = false;
Uint32 AmmoBoxTime;
UnitData AmmoBox;

// Score
Uint16 Kills = 0;
Uint16 Score = 0;

// Highscore
Uint16 HighscoreKills = 0;
Uint16 HighscoreScore = 0;
bool Running = true;
bool ShowGameOver = false;

#define VERSION		"1.0"
#define DATA_DIR	"."
#define SCORE_DIR	"."

// ==============================================================================
// Main function
// ------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    try
    {
        // Check command line
        if (argc > 1)
        {
            for (int i = 1; i <= argc; i++)
            {
                // Version
                if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
                {
                    cout << "OilWar version " << VERSION;
#ifdef SOUND
                    cout << " compiled with sound support";
#else
                    cout << " compiled without sound support";
#endif
                    cout << endl;
                    return 0;
                }
                // Help
                if (strcmp(argv[i], "--help") == 0)
                {
                    cout << "Usage: oilwar [OPTION]\n\n";
                    cout << "Options\n";
                    cout << " -v, --version      Display version information and exit.\n";
                    cout << " --help             Display this help and exit.\n";
                    cout << "\nData and score file locations\n";
                    cout << " Data dir: " << DATA_DIR << '\n';
                    cout << " Score file: " << SCORE_DIR << "oilwar.scores";
                    cout << endl;
                    return 0;
                }
                cerr << "Unknown command line option \"" << argv[i] << '\"' << endl;
                return 1;
            }
        }
        // Init SDL
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            cerr << "Unable to init SDL: " << SDL_GetError() << endl;
            return 1;
        }
        atexit(CleanUp);

        // Set caption
        SDL_WM_SetCaption("OilWar", NULL);

        // Set video mode
        pScreen = SDL_SetVideoMode(640, 480, 16, SDL_HWSURFACE | SDL_DOUBLEBUF);
        if (pScreen == NULL)
        {
            cerr << "Unable to set video mode: " << SDL_GetError() << endl;
            return 1;
        }
#ifdef SOUND
        // Open audio
        if (Mix_OpenAudio(22050, AUDIO_S16, 2, 4096))
        {
            cerr << "Unable to open audio: " << Mix_GetError() << endl;
            return 1;
        }
#endif

        // Random seed
        srand(time(NULL));

        // Load images
        pBackground = LoadImage("background.png");
        pBitmaps = LoadImage("bitmaps.png", true);

#ifdef SOUND
        // Load sounds
        pGunSound = LoadSound("singlegunshot.wav");
        pRicochetSound = LoadSound("bulletricochet.wav");
        pExplosionSound = LoadSound("explode.wav");
        pDeathSound = LoadSound("gasp.wav");
#endif

        // Load highscores
        LoadHighscores();

        // Hide cursor
        SDL_ShowCursor(false);

        // Blit background image
        SDL_BlitSurface(pBackground, NULL, pScreen, NULL);
        SDL_Flip(pScreen);

        // Timing
        Uint32 now, deltaTime, lastTime = SDL_GetTicks(), lastMove = SDL_GetTicks(), endTime = 0, explosionTime = 0;
        float time;

        list<UnitData>::iterator marine;
        list<UnitData>::iterator tank;
        list<Grenade>::iterator grenade;
        list<cParticleSystem>::iterator system, tempSystem;

        SpawnUnits();

        // Main loop
        SDL_Event event;
        SDL_Rect srcRect, dstRect;

        while (Running)
        {
            // Poll the event queue
            while (SDL_PollEvent(&event) != 0)
            {
                switch (event.type)
                {
                case SDL_QUIT:
                    Running = false;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_ESCAPE:
                        Running = false;
                        break;
                    case SDLK_F10:
                        cout << "Saving screenshot to shot.bmp" << endl;
                        SDL_SaveBMP(pScreen, "shot.bmp");
                        break;
                    case SDLK_F11:
                        SDL_WM_ToggleFullScreen(pScreen);
                        break;
                    default:
                        break;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (ShowGameOver)
                    {
                        break;
                    }
                    switch (event.button.button)
                    {
                    case SDL_BUTTON_LEFT:
                        FireGun(event.button.x, event.button.y);
                        break;
                    case SDL_BUTTON_RIGHT:
                        FireGrenade(event.button.x, event.button.y);
                        break;
                    default:
                        break;
                    }
                    break;
                default:
                    break;
                }
            }
            // Calculate delta time
            now = SDL_GetTicks();
            deltaTime = now - lastTime;
            lastTime = now;
            time = deltaTime * 0.01f;

            // Blit background image
            SDL_BlitSurface(pBackground, NULL, pScreen, NULL);
            // Animate tanks
            if (now > TankLastFrame + TankDelay)
            {
                TankFrame++;
                if (TankFrame >= NumberOfTankFrames)
                {
                    TankFrame = 0;
                }
                TankLastFrame = now;
            }
            // Render tanks
            tank = Tanks.begin();
            while (tank != Tanks.end())
            {
                srcRect = MakeRect(TankRect.x + TankFrame * TankRect.w, TankRect.y, TankRect.w, TankRect.h);
                dstRect = MakeRect(tank->x, tank->y, 0, 0);

                SDL_BlitSurface(pBitmaps, &srcRect, pScreen, &dstRect);
                tank++;
            }
            // Animate marines
            if (now > MarineLastFrame + MarineDelay)
            {
                MarineFrame++;
                if (MarineFrame >= NumberOfMarineFrames)
                {
                    MarineFrame = 0;
                }
                MarineLastFrame = now;
            }
            // Render marines
            marine = Marines.begin();
            while (marine != Marines.end())
            {
                srcRect = MakeRect(MarineRect.x + MarineFrame * MarineRect.w, MarineRect.y, MarineRect.w, MarineRect.h);
                dstRect = MakeRect(marine->x, marine->y, 0, 0);

                SDL_BlitSurface(pBitmaps, &srcRect, pScreen, &dstRect);
                marine++;
            }
            // Render ammo box
            if (AmmoBoxVisible)
            {
                dstRect = MakeRect(AmmoBox.x, AmmoBox.y, 0, 0);
                SDL_BlitSurface(pBitmaps, &AmmoBoxRect, pScreen, &dstRect);
            }
            grenade = Grenades.begin();
            while (grenade != Grenades.end())
            {
                grenade->velY += 9.81f * time;
                grenade->y += grenade->velY * time;
                grenade->x += grenade->velX * time;
                if (grenade->y >= grenade->ground)
                {
                    grenade->y = grenade->ground;
                    // Explode
                    MakeExplosion(int(grenade->x + 0.5f), int(grenade->y + 0.5f));
                    GrenadeDecal(int(grenade->x + 0.5f), int(grenade->y + 0.5f));
                    Explosion(int(grenade->x + 0.5f), int(grenade->y + 0.5f));

#ifdef SOUND
                    Mix_PlayChannel(-1, pExplosionSound, 0);
#endif

                    // Remove grenade from the list
                    list<Grenade>::iterator temp = grenade;
                    temp++;
                    Grenades.erase(grenade);
                    grenade = temp;
                }
                else
                {
                    // Render grenade
                    dstRect = MakeRect(int(grenade->x + 0.5f), int(grenade->y + 0.5f), 0, 0);
                    SDL_BlitSurface(pBitmaps, &GrenadeRect, pScreen, &dstRect);
                }
                grenade++;
            }
            // Render particle systems
            system = ParticleSystems.begin();
            while (system != ParticleSystems.end())
            {
                system->Render(pScreen);
                system++;
            }
            // Print kills & score
            Print(490, 40, Kills);
            Print(490, 100, Score);

            // Print highscores
            Print(20, 40, HighscoreKills);
            Print(20, 100, HighscoreScore);
            // Render weapon icons
            for (int i = 0; i < GrenadeAmmo; i++)
            {
                dstRect = MakeRect(610 - i * (GrenadeIconRect.w + 3), 450, 0, 0);
                SDL_BlitSurface(pBitmaps, &GrenadeIconRect, pScreen, &dstRect);
            }
            // Render crosshair
            RenderCrosshair();

            // Update screen
            SDL_Flip(pScreen);
            if (lastMove + 100 < now)
            {
                list<UnitData>::iterator tank = Tanks.begin();
                while (tank != Tanks.end())
                {
                    tank->x++;
                    CheckPosition(tank->x, tank->y);
                    tank++;
                }
                list<UnitData>::iterator marine = Marines.begin();
                while (marine != Marines.end())
                {
                    marine->x++;
                    CheckPosition(marine->x, marine->y);
                    marine++;
                }
                lastMove = now;
            }
            // Update particle systems
            system = ParticleSystems.begin();
            while (system != ParticleSystems.end())
            {
                if (!system->Update(deltaTime))
                {
                    tempSystem = system;
                    system++;
                    ParticleSystems.erase(tempSystem);
                }
                else
                {
                    system++;
                }
            }
            // Ammo box
            if (now > AmmoBoxTime)
            {
                AmmoBoxVisible = false;
            }
            if (ShowGameOver)
            {
                if (endTime == 0)
                {
                    // Oil
                    ParticleSystems.push_back(cParticleSystem(432, 270, 250, MakeColor(95, 74, 40), MakeColor(0, 0, 0),
                                              2, 40, 1000, true));
                    ParticleSystems.push_back(cParticleSystem(541, 275, 250, MakeColor(90, 70, 45), MakeColor(0, 0, 0),
                                              2, 40, 1000, true));

                    endTime = now + 5000;
                    explosionTime = 0;
                }
                if (now > explosionTime)
                {
                    explosionTime = now + 500;

                    float x, y;

                    for (int i = 0; i < 3; i++)
                    {
                        x = 421.0f + 203.0f * rand() / (RAND_MAX + 1.0f);
                        y = 327.0f + 35.0f * rand() / (RAND_MAX + 1.0f);
                        MakeExplosion(int(x), int(y));
                        if (i % 2 == 0)
                        {
                            ExplosionDecal(int(x), int(y));
                        }
                        else
                        {
                            GrenadeDecal(int(x), int(y));
                        }
                    }
#ifdef SOUND
                    Mix_PlayChannel(-1, pExplosionSound, 0);
#endif
                }
                if (now > endTime)
                {
                    GameOver();
                    endTime = 0;
                    // Start a new game if still running after the game over screen
                    if (Running)
                    {
                        NewGame();
                    }
                }
            }
        }
        return 0;
    }
    catch (exception &x)
    {
        cerr << "Exception caught: " << x.what() << endl;
    }
    catch (...)
    {
        cerr << "Unknown exception caught" << endl;
    }
    return 1;
}

// ==============================================================================

// ==============================================================================
// Free data etc.
// ------------------------------------------------------------------------------
void CleanUp(void)
{
    // Save highscores
    SaveHighscores();

    // Free images
    SDL_FreeSurface(pBackground);
    SDL_FreeSurface(pBitmaps);

#ifdef SOUND
    // Free sounds
    Mix_FreeChunk(pGunSound);
    Mix_FreeChunk(pExplosionSound);
    Mix_FreeChunk(pDeathSound);

    // Close audio
    Mix_CloseAudio();
#endif

    // Show cursor
    SDL_ShowCursor(true);

    // Quit SDL
    SDL_Quit();
}

// ==============================================================================

// ==============================================================================
// Spawn units
// ------------------------------------------------------------------------------
void SpawnUnits(void)
{
    if (Kills % 5 == 0)
    {
        UnitData tank;

        tank.x = -TankRect.w;
        tank.y = int(310.0f + 120.0f * rand() / (RAND_MAX + 1.0f));
        Tanks.push_back(tank);
    }

    UnitData marine;

    marine.x = -10;
    marine.y = int(310.0f + 120.0f * rand() / (RAND_MAX + 1.0f));
    Marines.push_back(marine);
    if (Kills % 3 == 0)
    {
        marine.y = int(310.0f + 120.0f * rand() / (RAND_MAX + 1.0f));
        Marines.push_back(marine);
    }
    if (Kills % 100 == 0)
    {
        AmmoBoxVisible = true;
        AmmoBoxTime = SDL_GetTicks() + 5000;
        AmmoBox.x = Uint16(30.0f + 300.0f * rand() / (RAND_MAX + 1.0f));
        AmmoBox.y = Uint16(340.0f + 70.0f * rand() / (RAND_MAX + 1.0f));
    }
}

// ==============================================================================

// ==============================================================================
// Is the unit hit
// ------------------------------------------------------------------------------
bool UnitData::Hit(int mouseX, int mouseY, int w, int h)
{
    if (mouseX >= x && mouseX < x + w && mouseY >= y && mouseY < y + h)
    {
        return true;
    }
    return false;
}

// ==============================================================================

// ==============================================================================
// Is this unit in the explosion area
// ------------------------------------------------------------------------------
bool UnitData::ExplosionHit(int explosionX, int explosionY)
{
    int distance = (explosionX - x) * (explosionX - x) + (explosionY - y) * (explosionY - y);

    if (distance <= 2500)
    {
        return true;
    }
    return false;
}

// ==============================================================================

// ==============================================================================
// Check enemy position
// ------------------------------------------------------------------------------
void CheckPosition(int x, int y)
{
    y -= 310;
    if (x >= 290 + int(float(y) / 120.0f * 100.0f))
    {
        ShowGameOver = true;
    }
}

// ==============================================================================

// ==============================================================================
// Show game over
// ------------------------------------------------------------------------------
void GameOver(void)
{
    SDL_Rect dstRect, newRect, quitRect, boxRect;

    ShowGameOver = false;
    // Set highscores (highscores are saved in CleanUp)
    if (Kills > HighscoreKills || Score > HighscoreScore)
    {
        HighscoreKills = Kills;
        HighscoreScore = Score;
    }
    // Draw the menu
    boxRect = MakeRect((pScreen->w - 200) / 2, (pScreen->h - 300) / 2, 200, 300);
    SDL_FillRect(pScreen, &boxRect, SDL_MapRGB(pScreen->format, 136, 71, 1));
    boxRect = MakeRect((pScreen->w - 198) / 2, (pScreen->h - 298) / 2, 198, 298);
    SDL_FillRect(pScreen, &boxRect, SDL_MapRGB(pScreen->format, 174, 110, 0));
    boxRect = MakeRect((pScreen->w - 196) / 2, (pScreen->h - 296) / 2, 196, 296);
    SDL_FillRect(pScreen, &boxRect, SDL_MapRGB(pScreen->format, 200, 180, 140));

    dstRect = MakeRect((pScreen->w - GameOverRect.w) / 2, 220 / 2, 0, 0);
    SDL_BlitSurface(pBitmaps, &GameOverRect, pScreen, &dstRect);

    newRect = MakeRect((pScreen->w - NewGameRect.w) / 2, 300, 0, 0);
    SDL_BlitSurface(pBitmaps, &NewGameRect, pScreen, &newRect);

    quitRect = MakeRect((pScreen->w - QuitRect.w) / 2, 335, 0, 0);
    SDL_BlitSurface(pBitmaps, &QuitRect, pScreen, &quitRect);

    SDL_Flip(pScreen);

    // Show cursor
    SDL_ShowCursor(true);

    SDL_Event event;

    while (Running)
    {
        SDL_WaitEvent(&event);
        switch (event.type)
        {
        case SDL_QUIT:
            Running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                Running = false;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (InsideRect(event.button.x, event.button.y, newRect))
            {
                return;
            }
            else if (InsideRect(event.button.x, event.button.y, quitRect))
            {
                Running = false;
            }
            break;
        default:
            break;
        }
    }
}

// ==============================================================================

// ==============================================================================
// Fire primary gun
// ------------------------------------------------------------------------------
void FireGun(Sint16 x, Sint16 y)
{
    list<UnitData>::iterator marine;
    list<UnitData>::iterator tank;

    bool hit = false;

    if (AmmoBox.Hit(x, y, AmmoBoxRect.w, AmmoBoxRect.h) && AmmoBoxVisible)
    {
        GrenadeAmmo += 3;
        AmmoBoxVisible = false;
    }
    marine = Marines.begin();
    while (marine != Marines.end())
    {
        if (marine->Hit(x, y, MarineRect.w, MarineRect.h))
        {
            BloodDecal(marine->x, marine->y);
            MakeBlood(x, y);

            Kills++;
            Score += 10;

            Marines.erase(marine);
            SpawnUnits();
            hit = true;

#ifdef SOUND
            // Sounds
            Mix_PlayChannel(-1, pDeathSound, 0);
            Mix_PlayChannel(GunChannel, pGunSound, 0);
#endif

            break;
        }
        marine++;
    }
    tank = Tanks.begin();
    while (tank != Tanks.end())
    {
        if (tank->Hit(x, y, TankRect.w, TankRect.h))
        {
#ifdef SOUND
            // Sounds
            Mix_PlayChannel(GunChannel, pGunSound, 0);
#endif

            tank->hits++;
            hit = true;
            if (tank->hits >= 5)
            {
                ExplosionDecal(tank->x, tank->y);
                MakeExplosion(x, y);

                Kills++;
                Score += 100;

                Tanks.erase(tank);
                SpawnUnits();

#ifdef SOUND
                // Sounds
                Mix_PlayChannel(-1, pExplosionSound, 0);
#endif

                break;
            }
#ifdef SOUND
            else
            {
                Mix_PlayChannel(-1, pRicochetSound, 0);
            }
#endif
        }
        tank++;
    }
    if (!hit)
    {
        if (y > 250 && x <= 250 + int(float(y - 250) / 200.0f * 180.0f))
        {
            MakeSand(x, y);
            SandDecal(x, y);

#ifdef SOUND
            // Sounds
            Mix_PlayChannel(GunChannel, pGunSound, 0);
#endif
        }
    }
}

// ==============================================================================

// ==============================================================================
// Fire greanade
// ------------------------------------------------------------------------------
void FireGrenade(Sint16 x, Sint16 y)
{
    if (GrenadeAmmo <= 0)
    {
        return;
    }
    if (InGrenadeRange(x, y))
    {
        GrenadeAmmo--;
        Grenades.push_back(Grenade(x, y - 400, 0, 0, y));
    }
}

// ==============================================================================

// ==============================================================================
// Kill all units within explosion area
// ------------------------------------------------------------------------------
void Explosion(Sint16 x, Sint16 y)
{
    if (AmmoBox.ExplosionHit(x, y) && AmmoBoxVisible)
    {
        GrenadeAmmo += 3;
        AmmoBoxVisible = false;
    }
    // Kill marines
    list<UnitData>::iterator marine = Marines.begin();
    while (marine != Marines.end())
    {
        if (marine->ExplosionHit(x, y))
        {
            BloodDecal(marine->x, marine->y);
            MakeBlood(marine->x, marine->y);

            Kills++;
            Score += 10;

            marine->x = int(-100.0f + 90.0f * rand() / (RAND_MAX + 1.0f));
            marine->y = int(310.0f + 120.0f * rand() / (RAND_MAX + 1.0f));
        }
        else
        {
            marine++;
        }
    }
    // Kill tanks
    list<UnitData>::iterator tank = Tanks.begin();
    while (tank != Tanks.end())
    {
        if (tank->ExplosionHit(x, y))
        {
            ExplosionDecal(tank->x, tank->y);
            MakeExplosion(tank->x, tank->y);

            Kills++;
            Score += 10;

            tank->x = int(-100.0f + 70.0f * rand() / (RAND_MAX + 1.0f));
            tank->y = int(310.0f + 120.0f * rand() / (RAND_MAX + 1.0f));
            tank->hits = 0;
        }
        else
        {
            tank++;
        }
    }
}

// ==============================================================================

// ==============================================================================
// Load image from file
// ------------------------------------------------------------------------------
SDL_Surface *LoadImage(string filename, bool colorkey)
{
    string completeFilename = DATA_DIR;

    completeFilename.append(filename);

    SDL_Surface *pTemp = IMG_Load(completeFilename.c_str());

    if (pTemp == NULL)
    {
        pTemp = IMG_Load(filename.c_str());
        if (pTemp == NULL)
        {
            cerr << "Unable to load image " << completeFilename.c_str() << " or " << filename.c_str() << endl;
            throw runtime_error("Unable to load resources");
        }
    }
    if (colorkey)
    {
        SDL_SetColorKey(pTemp, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(pTemp->format, 255, 0, 255));
    }

    SDL_Surface *pTemp2 = SDL_DisplayFormat(pTemp);

    SDL_FreeSurface(pTemp);
    if (pTemp2 == NULL)
    {
        cerr << "Unable to convert image: " << SDL_GetError() << endl;
        throw runtime_error("Unable to load resources");
    }
    return pTemp2;
}

// ==============================================================================

// ==============================================================================
// Build SDL_Rect structure
// ------------------------------------------------------------------------------
SDL_Rect MakeRect(Sint16 x, Sint16 y, Uint16 w, Uint16 h)
{
    SDL_Rect temp = { x, y, w, h };

    return temp;
}

// ==============================================================================

// ==============================================================================
// Build SDL_Color structure
// ------------------------------------------------------------------------------
SDL_Color MakeColor(Uint8 r, Uint8 g, Uint8 b)
{
    SDL_Color temp = { r, g, b };

    return temp;
}

// ==============================================================================

// ==============================================================================
// Print number
// ------------------------------------------------------------------------------
void Print(int x, int y, int num)
{
    char buffer[10];

    snprintf(buffer, 10, "%09i", num);

    SDL_Rect srcRect, dstRect;

    srcRect = MakeRect(TextRect.x, TextRect.y, TextRect.w / 10, TextRect.h);
    dstRect = MakeRect(x, y, 0, 0);
    for (int i = 0; i < 10; i++)
    {
        dstRect.x += srcRect.w;
        if (isdigit(buffer[i]))
        {
            srcRect.x = TextRect.x + (buffer[i] - 48) * srcRect.w;
            SDL_BlitSurface(pBitmaps, &srcRect, pScreen, &dstRect);
        }
    }
}

// ==============================================================================

// ==============================================================================
// Spawn blood particle system
// ------------------------------------------------------------------------------
void MakeBlood(Uint16 x, Uint16 y)
{
    ParticleSystems.push_back(cParticleSystem(x, y, Blood1_Particles, Blood1_Start, Blood1_End, Blood1_Size,
                              Blood1_Power, Blood1_MaxEnergy));
    ParticleSystems.push_back(cParticleSystem(x, y, Blood2_Particles, Blood2_Start, Blood2_End, Blood2_Size,
                              Blood2_Power, Blood2_MaxEnergy));
}

// ==============================================================================

// ==============================================================================
// Spawn explosion particle system
// ------------------------------------------------------------------------------
void MakeExplosion(Uint16 x, Uint16 y)
{
    ParticleSystems.push_back(cParticleSystem(x, y, Explosion1_Particles, Explosion1_Start, Explosion1_End,
                              Explosion1_Size, Explosion1_Power, Explosion1_MaxEnergy));

    ParticleSystems.push_back(cParticleSystem(x, y, Explosion2_Particles, Explosion2_Start, Explosion2_End,
                              Explosion2_Size, Explosion2_Power, Explosion2_MaxEnergy));

    ParticleSystems.push_back(cParticleSystem(x, y, Explosion3_Particles, Explosion3_Start, Explosion3_End,
                              Explosion3_Size, Explosion3_Power, Explosion3_MaxEnergy));
}

// ==============================================================================

// ==============================================================================
// Spawn sand particle system
// ------------------------------------------------------------------------------
void MakeSand(Uint16 x, Uint16 y)
{
    ParticleSystems.push_back(cParticleSystem(x, y, Sand_Particles, Sand_Start, Sand_End, Sand_Size, Sand_Power,
                              Sand_MaxEnergy));
}

// ==============================================================================

// ==============================================================================
// Render crosshair
// ------------------------------------------------------------------------------
void RenderCrosshair(void)
{
    int x, y;

    // Get cursor position
    SDL_GetMouseState(&x, &y);

    SDL_Rect dstRect = { x - 10, y - 10, 0, 0 };

    if (InGrenadeRange(x, y) && GrenadeAmmo > 0)
    {
        SDL_BlitSurface(pBitmaps, &Crosshair2Rect, pScreen, &dstRect);
    }
    else
    {
        SDL_BlitSurface(pBitmaps, &CrosshairRect, pScreen, &dstRect);
    }
}

// ==============================================================================

// ==============================================================================
// Blit blood decal to background
// ------------------------------------------------------------------------------
void BloodDecal(Uint16 x, Uint16 y)
{
    SDL_Rect dstRect = { x, y + 5, 0, 0 };

    SDL_BlitSurface(pBitmaps, &BloodDecalRect, pBackground, &dstRect);
}

// ==============================================================================

// ==============================================================================
// Blit explosion decal to background
// ------------------------------------------------------------------------------
void ExplosionDecal(Uint16 x, Uint16 y)
{
    SDL_Rect dstRect = { x + 5, y + 10, 0, 0 };

    SDL_BlitSurface(pBitmaps, &ExplosionDecalRect, pBackground, &dstRect);
}

// ==============================================================================

// ==============================================================================
// Blit shell hole decal to background
// ------------------------------------------------------------------------------
void GrenadeDecal(Uint16 x, Uint16 y)
{
    SDL_Rect dstRect = { x - 6, y - 5, 0, 0 };

    SDL_BlitSurface(pBitmaps, &GrenadeDecalRect, pBackground, &dstRect);
}

// ==============================================================================

// ==============================================================================
// Blit sand decal to background
// ------------------------------------------------------------------------------
void SandDecal(Uint16 x, Uint16 y)
{
    SDL_Rect dstRect = { x - 2, y - 4, 0, 0 };

    SDL_BlitSurface(pBitmaps, &SandDecalRect, pBackground, &dstRect);
}

// ==============================================================================

#ifdef SOUND
// ==============================================================================
// Load sound (WAV)
// ------------------------------------------------------------------------------
Mix_Chunk *LoadSound(string filename)
{
    string completeFilename = DATA_DIR;

    completeFilename.append(filename);

    Mix_Chunk *pTemp = Mix_LoadWAV(completeFilename.c_str());

    if (pTemp == NULL)
    {
        pTemp = Mix_LoadWAV(filename.c_str());
        if (pTemp == NULL)
        {
            cerr << "Unable to load sound " << completeFilename.c_str() << " or " << filename.c_str() << endl;
            throw runtime_error("Unable to load resources");
        }
    }
    return pTemp;
}

// ==============================================================================
#endif

// ==============================================================================
// Save highscores
// ------------------------------------------------------------------------------
void LoadHighscores(void)
{
    string filename = SCORE_DIR;

    filename.append("oilwar.scores");

    FILE *pHighscoreFile = NULL;

    // Try to open "oilwar.scores" for reading
    pHighscoreFile = fopen("oilwar.scores", "r");
    if (pHighscoreFile == NULL)
    {
        // Unable to open "oilwar.scores" for reading
        // Try to open "SCORE_DIR/oilwar.scores"
        pHighscoreFile = fopen(filename.c_str(), "r");
        if (pHighscoreFile == NULL)
        {
            cerr << "Unable to open highscore file \'oilwar.scores\' or \'" << filename << "\' for reading." << endl;
            return;
        }
        else
        {
            cout << "Loading highscores from file \'" << filename << '\'' << endl;
        }
    }
    else
    {
        cout << "Loading highscores from file \'oilwar.scores\'" << endl;
    }
    // Check file size
    fseek(pHighscoreFile, 0, SEEK_END);
    if (ftell(pHighscoreFile) == 2 * sizeof (Uint16))
    {
        // Back to the begining
        rewind(pHighscoreFile);

        // Load kills
        fread(&HighscoreKills, sizeof (Uint16), 1, pHighscoreFile);
        // Load score
        fread(&HighscoreScore, sizeof (Uint16), 1, pHighscoreFile);
        // Check errors
        if (ferror(pHighscoreFile))
        {
            cerr << "Error occurred while reading highscores." << endl;
            HighscoreKills = 0;
            HighscoreScore = 0;
        }
    }
    else                                // Wrong file size
    {
        HighscoreKills = 0;
        HighscoreScore = 0;
    }
    fclose(pHighscoreFile);
}

// ==============================================================================

// ==============================================================================
// Save highscores
// ------------------------------------------------------------------------------
void SaveHighscores(void)
{
    if (Kills > HighscoreKills || Score > HighscoreScore)
    {
        HighscoreKills = Kills;
        HighscoreScore = Score;
    }

    string filename = SCORE_DIR;

    filename.append("oilwar.scores");

    FILE *pHighscoreFile = NULL;

    // Try to open "oilwar.scores" for writing (do NOT create a file)
    pHighscoreFile = fopen("oilwar.scores", "r+");
    if (pHighscoreFile == NULL)
    {
        // Unable to open "oilwar.scores"
        // Try to open "SCORE_DIR/oilwar.scores" for writing (do NOT create a file)
        pHighscoreFile = fopen(filename.c_str(), "r+");
        if (pHighscoreFile == NULL)
        {
            cerr << "Unable to open highscore file \'oilwar.scores\' or \'" << filename << " for writing." << endl;
            return;
        }
        else
        {
            cout << "Saving highscores to file \'" << filename << '\'' << endl;
        }
    }
    else
    {
        cout << "Saving highscores to file \'oilwar.scores\'" << endl;
    }
    // Save kills
    fwrite(&HighscoreKills, sizeof (Uint16), 1, pHighscoreFile);
    // Save score
    fwrite(&HighscoreScore, sizeof (Uint16), 1, pHighscoreFile);
    // Check errors
    if (ferror(pHighscoreFile) != 0)
    {
        cerr << "Error occurred while writing highscores." << endl;
    }
    // Close the file
    fclose(pHighscoreFile);
}

// ==============================================================================

// ==============================================================================
// Check if position x,y is insde the grenade range
// ------------------------------------------------------------------------------
bool InGrenadeRange(Sint16 x, Sint16 y)
{
    return y > 250 && x <= 250 + int(float(y - 250) / 200.0f * 180.0f) && x > 60;
}

// ==============================================================================

// ==============================================================================
// Check if position x,y is inside the rect
// ------------------------------------------------------------------------------
bool InsideRect(Sint16 x, Sint16 y, SDL_Rect rect)
{
    if (x > rect.x && x <= rect.x + rect.w && y > rect.y && y <= rect.y + rect.h)
    {
        return true;
    }
    return false;
}

// ==============================================================================

// ==============================================================================
// New game
// ------------------------------------------------------------------------------
void NewGame(void)
{
    // Weapons
    GrenadeAmmo = 3;

    // Ammo box
    AmmoBoxVisible = false;

    // Score
    Kills = 0;
    Score = 0;

    Tanks.clear();
    Marines.clear();
    Grenades.clear();
    ParticleSystems.clear();

    // Free background
    SDL_FreeSurface(pBackground);
    // Reload background
    pBackground = LoadImage("background.png");

    // Hide cursor
    SDL_ShowCursor(false);

    // Blit background image
    SDL_BlitSurface(pBackground, NULL, pScreen, NULL);
    SDL_Flip(pScreen);

    SpawnUnits();
}

// ==============================================================================

// ==============================================================================
// EOF
// ==============================================================================
