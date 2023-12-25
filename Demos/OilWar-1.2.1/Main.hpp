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
// File: Main.hpp
// Author: Jarmo Hekkanen <jarski@2ndpoint.fi>
// ------------------------------------------------------------------------------
#ifndef Main_hpp
#define Main_hpp
// ------------------------------------------------------------------------------
// Includes
#include <string>
#include <list>
#include <SDL/SDL.h>
#include "cParticleSystem.hpp"
#ifdef SOUND
#include <SDL/SDL_mixer.h>
#endif
// ==============================================================================
using namespace std;

struct UnitData
{
    UnitData(void): hits(0) {}
    bool Hit(int x, int y, int w, int h);
    bool ExplosionHit(int x, int y);

    int x;
    int y;
    int hits;
};

struct Grenade
{
    Grenade(float X, float Y, float VelX, float VelY, int Ground): x(X), y(Y), velX(VelX), velY(VelY), ground(Ground) {}

    float x;
    float y;
    float velX;
    float velY;
    int ground;
};

SDL_Surface *LoadImage(string filename, bool colorkey = false);

void CleanUp(void);
SDL_Rect MakeRect(Sint16 x, Sint16 y, Uint16 w, Uint16 h);
SDL_Color MakeColor(Uint8 r, Uint8 g, Uint8 b);
void SpawnUnits(void);
void Print(int x, int y, int num);
void CheckPosition(int x, int y);
void GameOver(void);
void MakeBlood(Uint16 x, Uint16 y);
void MakeExplosion(Uint16 x, Uint16 y);
void MakeSand(Uint16 x, Uint16 y);
void RenderCrosshair(void);
void BloodDecal(Uint16 x, Uint16 y);
void ExplosionDecal(Uint16 x, Uint16 y);
void SandDecal(Uint16 x, Uint16 y);
void LoadHighscores(void);
void SaveHighscores(void);
void FireGun(Sint16 x, Sint16 y);
void FireGrenade(Sint16 x, Sint16 y);
void Explosion(Sint16 x, Sint16 y);
bool InGrenadeRange(Sint16 x, Sint16 y);
void GrenadeDecal(Uint16 x, Uint16 y);
bool InsideRect(Sint16 x, Sint16 y, SDL_Rect rect);
void NewGame(void);

#ifdef SOUND

Mix_Chunk *LoadSound(string filename);

#endif

// ==============================================================================
#endif                                  // Main_hpp
// ------------------------------------------------------------------------------
// EOF
// ==============================================================================
