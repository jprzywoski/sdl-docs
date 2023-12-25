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
// File: Config.hpp
// Author: Jarmo Hekkanen <jarski@2ndpoint.fi>
// ------------------------------------------------------------------------------
#ifndef Config_hpp
#define Config_hpp
// ------------------------------------------------------------------------------
// Includes
#include <SDL/SDL.h>
// ==============================================================================
using namespace std;

// Bitmap positions on the bitmap.png image
// { TopLeftX, TopLeftY, FrameWidth, FrameHeight }
SDL_Rect GameOverRect = { 0, 0, 150, 100 };
SDL_Rect CrosshairRect = { 129, 100, 21, 21 };
SDL_Rect TextRect = { 0, 100, 100, 20 };
SDL_Rect TankRect = { 0, 120, 34, 20 };
SDL_Rect MarineRect = { 78, 129, 10, 15 };
SDL_Rect ExplosionDecalRect = { 78, 120, 24, 9 };
SDL_Rect SandDecalRect = { 102, 120, 4, 9 };
SDL_Rect BloodDecalRect = { 106, 120, 8, 9 };
SDL_Rect GrenadeIconRect = { 120, 100, 9, 21 };
SDL_Rect GrenadeRect = { 143, 121, 7, 16 };
SDL_Rect GrenadeDecalRect = { 0, 140, 19, 15 };
SDL_Rect AmmoBoxRect = { 106, 100, 14, 9 };
SDL_Rect Crosshair2Rect = { 19, 140, 21, 21 };
SDL_Rect NewGameRect = { 0, 174, 131, 30 };
SDL_Rect QuitRect = { 40, 144, 73, 30 };

// Number of frames in animations
const Uint8 NumberOfMarineFrames = 5;
const Uint8 NumberOfTankFrames = 2;

// Animation speeds
const Uint32 MarineDelay = 100;
const Uint32 TankDelay = 100;

// Blood1 Particle system
Uint16 Blood1_Particles = 15;
Uint16 Blood1_Size = 2;
Uint16 Blood1_Power = 10;
SDL_Color Blood1_Start = { 255, 0, 0 };
SDL_Color Blood1_End = { 128, 0, 0 };
float Blood1_MaxEnergy = 400;

// Blood2 Particle system
Uint16 Blood2_Particles = 100;
Uint16 Blood2_Size = 1;
Uint16 Blood2_Power = 20;
SDL_Color Blood2_Start = { 200, 0, 0 };
SDL_Color Blood2_End = { 128, 0, 0 };
float Blood2_MaxEnergy = 400;

// Sand Particle system
Uint16 Sand_Particles = 20;
Uint16 Sand_Size = 1;
Uint16 Sand_Power = 15;
SDL_Color Sand_Start = { 80, 80, 90 };
SDL_Color Sand_End = { 50, 55, 15 };
float Sand_MaxEnergy = 400;

// Explosion1 Particle system
Uint16 Explosion1_Particles = 40;
Uint16 Explosion1_Size = 2;
Uint16 Explosion1_Power = 30;
SDL_Color Explosion1_Start = { 232, 56, 0 };
SDL_Color Explosion1_End = { 50, 0, 0 };
float Explosion1_MaxEnergy = 700;

// Explosion2 Particle system
Uint16 Explosion2_Particles = 40;
Uint16 Explosion2_Size = 2;
Uint16 Explosion2_Power = 30;
SDL_Color Explosion2_Start = { 225, 235, 0 };
SDL_Color Explosion2_End = { 23, 54, 0 };
float Explosion2_MaxEnergy = 700;

// Explosion3 Particle system
Uint16 Explosion3_Particles = 100;
Uint16 Explosion3_Size = 1;
Uint16 Explosion3_Power = 45;
SDL_Color Explosion3_Start = { 225, 0, 0 };
SDL_Color Explosion3_End = { 150, 0, 0 };
float Explosion3_MaxEnergy = 500;

// ==============================================================================
#endif                                  // Config_hpp
// ------------------------------------------------------------------------------
// EOF
// ==============================================================================
