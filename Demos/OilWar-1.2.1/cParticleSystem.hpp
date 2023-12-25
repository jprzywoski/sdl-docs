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
// File: cParticleSystem.hpp
// Author: Jarmo Hekkanen <jarski@2ndpoint.fi>
// Copyright (c) Jarmo Hekkanen 2003 <jarski@2ndpoint.fi>
// ------------------------------------------------------------------------------
#ifndef cParticleSystem_hpp
#define cParticleSystem_hpp
// ------------------------------------------------------------------------------
// Includes
#include <SDL/SDL.h>
#include <vector>
// ------------------------------------------------------------------------------
// Namespaces
using namespace std;
// ==============================================================================

// ==============================================================================
// Particle system
// ------------------------------------------------------------------------------
class cParticleSystem
{
    // Constructor & Destructor

public:
    // Constructor
    cParticleSystem(Uint16 x, Uint16 y, Uint16 maxParticles, SDL_Color a, SDL_Color b, Uint16 size, Uint16 power, float
                    maxEnergy, bool regen = false);
    // Destructor
    ~cParticleSystem(void);

    // Public methods

public:
    // Update system
    bool Update(Uint32 deltaTime);

    // Render particles
    void Render(SDL_Surface *pScreen);

    // Internal structures

private:

    struct tParticle
    {
        // Constructor (all particles are born dead)
        tParticle(void)
        {
            mEnergy = 0;
        }

        float mX;                       // Particle's X coordinate
        float mY;                       // Particle's Y coordinate
        float mVelX;                    // Particle's X velocity
        float mVelY;                    // Particle's Y velocity
        Uint32 mEnergy;                 // Particle's energy
        Uint32 mMaxEnergy;              // Particle's starting energy
    };

    // Internal methods

private:
    // Emit particle
    void EmitParticle(tParticle &particle);

    // Render particle
    void RenderParticle(const tParticle &particle, SDL_Surface *pScreen);

    // Update particle
    void UpdateParticle(tParticle &particle, Uint32 deltaTime);

    // Member variables

private:

    Uint16 mX;                          // System's X coordinate
    Uint16 mY;                          // System's Y coordinate
    SDL_Color mStartColor;              // Start color
    SDL_Color mEndColor;                // End color
    Uint16 mSize;                       // Size of the particle
    Uint16 mPower;                      // Affects the starting speed
    float mMaxEnergy;                   // Max energy
    bool mRegen;                        // Regen particles

    vector<tParticle> mParticles;       // List of particles in this system
};

// ==============================================================================

// ==============================================================================
#endif                                  // cParticleSystem_hpp
// ------------------------------------------------------------------------------
// EOF
// ==============================================================================
