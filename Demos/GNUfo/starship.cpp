#include "ufo.h"

extern SDL_Surface *screen;
extern double DeltaTime;
extern int done;                        // You loose, break in the event game loop

SDLStarShip::SDLStarShip(const char *Filename, int x, int y, int n): SDLSprite(Filename, x, y, n)
{
    life = 10;

    direction_starsheep = 0;
    Ldirection_starsheep = 0;
    Rdirection_starsheep = 0;
    Vdirection_starsheep = 0;

    power = 2;
    nbshoot = 1;
}

SDLStarShip::~SDLStarShip() {}

void SDLStarShip::Update()
{
    // You loose
    if (life >= 255)
    {
        done = 1;
        return;
    }
    switch (direction_starsheep)
    {
    case 1:
        m_PosX += (m_VelX * DeltaTime);
        if (m_PosX > (Screen_w - 32 - m_ShadowOffsetX))
        {
            m_PosX = Screen_w - 32 - m_ShadowOffsetX;
        }
        break;

    case -1:
        m_PosX -= (m_VelX * DeltaTime);
        if (m_PosX < 0)
        {
            m_PosX = 0;
        }
        break;
    default:
        break;
    }
    switch (Vdirection_starsheep)
    {
    case 1:
        if (m_PosY + (m_VelY * DeltaTime) < Screen_h - 24)
        {
            m_PosY += (m_VelY * DeltaTime);
        }
        break;
    case -1:
        if ((m_PosY - (m_VelY * DeltaTime)) > Screen_h - 200)
        {
            m_PosY -= (m_VelY * DeltaTime);
        }
        break;
defautl:
        break;
    }
    Draw(screen, 0, 0);                 // 0,0 because there's no scrolling
    if (m_State >= 3)
    {
        m_State -= 3;
    }
}

void SDLStarShip::HideSprite()
{
    SDL_Rect dstrect;

    dstrect.x = (int)m_PosX;
    dstrect.y = (int)m_PosY;
    dstrect.w = m_BlockWidth;
    dstrect.h = m_BlockHeight;

    SDL_FillRect(screen, &dstrect, 0);
}
