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
// File: cParticleSystem.cpp
// Author: Jarmo Hekkanen <jarski@2ndpoint.fi>
// Copyright (c) Jarmo Hekkanen 2003 <jarski@2ndpoint.fi>
// ------------------------------------------------------------------------------
// Includes
#include "cParticleSystem.hpp"
#include <math.h>
#include <stdlib.h>
// ==============================================================================

// ==============================================================================
// Constructor
// ------------------------------------------------------------------------------
cParticleSystem::cParticleSystem(Uint16 x, Uint16 y, Uint16 maxParticles, SDL_Color a, SDL_Color b, Uint16 size, Uint16
                                 power, float maxEnergy, bool regen): mX(x), mY(y), mStartColor(a), mEndColor(b), mSize(size), mPower(power),
    mMaxEnergy(maxEnergy), mRegen(regen)
{
    mParticles.resize(maxParticles);
    // Emit particles
    for (Uint16 i = 0; i < mParticles.size(); i++)
    {
        EmitParticle(mParticles[i]);
    }
}

// ==============================================================================

// ==============================================================================
// Destructor
// ------------------------------------------------------------------------------
cParticleSystem::~cParticleSystem(void)
{
    mParticles.clear();
}

// ==============================================================================

// ==============================================================================
// Render particles
// ------------------------------------------------------------------------------
void cParticleSystem::Render(SDL_Surface *pScreen)
{
    // Render particles
    for (Uint16 i = 0; i < mParticles.size(); i++)
    {
        // Check if particle is alive
        if (mParticles[i].mEnergy > 0)
        {
            // Render particle
            RenderParticle(mParticles[i], pScreen);
        }
    }
}

// ==============================================================================

// ==============================================================================
// Update system
// ------------------------------------------------------------------------------
bool cParticleSystem::Update(Uint32 deltaTime)
{
    // Update particles
    bool systemAlive = false;

    for (Uint16 i = 0; i < mParticles.size(); i++)
    {
        // Check if particle is alive
        if (mParticles[i].mEnergy > 0)
        {
            // Update particle
            UpdateParticle(mParticles[i], deltaTime);
            systemAlive = true;
        }
        else if (mRegen)
        {
            EmitParticle(mParticles[i]);
        }
    }
    return systemAlive || mRegen;
}

// ==============================================================================

// ==============================================================================
// Emit particle
// ------------------------------------------------------------------------------
void cParticleSystem::EmitParticle(tParticle &particle)
{
    // Setup particle
    particle.mX = float(mX);
    particle.mY = float(mY);

    // Emitting angle
    float angle = -45.0f - 90.0f * rand() / (RAND_MAX + 1.0f);
    int power = mPower + int((-0.1f + 0.2f * rand() / (RAND_MAX + 1.0f) * float(mPower)));

    particle.mVelX = cos(angle / 180.0 * M_PI) * power;
    particle.mVelY = sin(angle / 180.0 * M_PI) * power;
    particle.mEnergy = 700;
    particle.mMaxEnergy = 700;

    UpdateParticle(particle, int(100.0f * rand() / (RAND_MAX + 1.0f)));

    particle.mEnergy = int(0.1f * mMaxEnergy + mMaxEnergy * rand() / (RAND_MAX + 1.0f));
}

// ==============================================================================

// ==============================================================================
// Render particle
// ------------------------------------------------------------------------------
void cParticleSystem::RenderParticle(const tParticle &particle, SDL_Surface *pScreen)
{
    SDL_Rect rect = { int(particle.mX), int(particle.mY), mSize, mSize };
    float interpolation = 1.0f - float(particle.mEnergy) / float(particle.mMaxEnergy);
    SDL_Color color = mStartColor;

    color.r += Uint8(float(int(mEndColor.r) - mStartColor.r) * interpolation);
    color.g += Uint8(float(int(mEndColor.g) - mStartColor.g) * interpolation);
    color.b += Uint8(float(int(mEndColor.b) - mStartColor.b) * interpolation);

    SDL_FillRect(pScreen, &rect, SDL_MapRGB(pScreen->format, color.r, color.g, color.b));
}

// ==============================================================================

// ==============================================================================
// Update particle
// ------------------------------------------------------------------------------
void cParticleSystem::UpdateParticle(tParticle &particle, Uint32 deltaTime)
{
    if (int(particle.mEnergy) - int(deltaTime) <= 0)
    {
        particle.mEnergy = 0;
        return;
    }
    particle.mEnergy -= deltaTime;

    float time = float(deltaTime) * 0.01f;

    // Move particle
    particle.mX += particle.mVelX * time;
    particle.mY += particle.mVelY * time;
    // Gravity
    if (particle.mY <= mY)
    {
        particle.mVelY += 9.21f * time;
    }
    else
    {
        particle.mY = mY;
        particle.mVelX = 0;
        particle.mVelY = 0;
    }
}

// ==============================================================================

// ==============================================================================
// EOF
// ==============================================================================
