#include "ufo.h"

extern ExplosionL ExploL;
extern SDL_Surface *screen;
extern double DeltaTime;
extern SDLSprite *explosion;
extern SDLStarShip *starsheep;

NotAliveSprite::NotAliveSprite(): SDLSpriteList() {}

void NotAliveSprite::SetSprite(SDLSprite *s)
{
    _thesprite = s;
}

NotAliveSprite::~NotAliveSprite() {}

void NotAliveSprite::Update()
{
    SDLSprite *Tmp;

    // Loop the list and update the sprites
    for (Node = m_List.m_Next; Node != &m_List; Node = Node->m_Next)
    {
        if (WhatHappens() == 1)
        {
            continue;
        }
        Move();
        if (ShouldRemove() == 1)
        {
            continue;
        }
    }
    Draw(screen, 0, 0);                 // affichage des sprites autonomes
}

int NotAliveSprite::ShouldRemove()
{
    SDLSprite *Tmp;

    if (Node->m_PosX < 0)
    {
        printf("Remove Sprite \n");
        Tmp = Node->m_Next;
        DelSprite(Node);
        Node = Tmp;
        return 1;
    }
    if (Node->m_PosX > (Screen_w - Node->m_BlockWidth))
    {
        printf("Remove Sprite \n");
        Tmp = Node->m_Next;
        DelSprite(Node);
        Node = Tmp;
        return 1;
    }
    if (Node->m_PosY < 0)
    {
        printf("Remove Sprite \n");
        Tmp = Node->m_Next;
        DelSprite(Node);
        Node = Tmp;
        return 1;
    }
    if (Node->m_PosY > (Screen_h - Node->m_BlockHeight))
    {
        printf("Remove Sprite \n");
        Tmp = Node->m_Next;
        DelSprite(Node);
        Node = Tmp;
        return 1;
    }
    return 0;
}

int NotAliveSprite::WhatHappens()
{
    SDLSprite *Tmp;

    // Est-ce que un missille a touche la starship
    return 0;
}

void NotAliveSprite::Move()
{
    Node->m_PosX += (Node->m_VelX * DeltaTime);
    Node->m_PosY += (Node->m_VelY * DeltaTime);
    Node->SetFrame(((Node->m_DelayStart - SDL_GetTicks()) / Node->m_Delay) % Node->m_NbFrame);
}

void NotAliveSprite::HideListSprite()
{
    SDL_Rect dstrect;
    SDLSprite *Node;

    for (Node = m_List.m_Next; Node != &m_List; Node = Node->m_Next)
    {
        dstrect.x = (int)Node->m_PosX;
        dstrect.y = (int)Node->m_PosY;
        dstrect.w = Node->m_BlockWidth;
        dstrect.h = Node->m_BlockHeight;

        SDL_FillRect(screen, &dstrect, 0);
    }
}

// -----------------------------------------

void ExplosionL::Update()
{
    SDLSprite *Tmp;

    for (Node = m_List.m_Next; Node != &m_List; Node = Node->m_Next)
    {
        if (Node->m_Frame > Node->m_NbFrame)
        {
            Tmp = Node->m_Next;
            DelSprite(Node);
            Node = Tmp;
            continue;
        }
        else
        {
            Node->SetFrame(Node->m_Frame + 1);
        }
    }
    Draw(screen, 0, 0);                 // affichage des sprites autonomes
}

int ProjectileEnemi::WhatHappens()
{
    SDLSprite *Tmp;

    // Est-ce que un missille a touche la starship
    if (Node->SpriteHit(starsheep) == TRUE)
    {
        if (starsheep->m_Frame < 4)
        {
            starsheep->SetFrame(starsheep->m_Frame + 3);
            starsheep->Draw(screen, 0, 0);      // 0,0 because there's no scrolling
        }
        printf("Touche\n");
        // We use the m_State of Sprite to know how much point to retrieve
        starsheep->life += Node->m_State;
        if (starsheep->life >= 255)
        {
            printf("Explosion you loose \n");
            ExploL.AddSprite(explosion, 0, starsheep->m_PosX, starsheep->m_PosY, 0, 0, 0, 0, 1000, SDL_GetTicks());
        }
        Tmp = Node->m_Next;
        DelSprite(Node);
        Node = Tmp;
        return 1;
    }
    return 0;
}

void ProjectileEnemi::Move()
{
    Node->m_PosX += (Node->m_VelX * DeltaTime);
    Node->m_PosY += (Node->m_VelY * DeltaTime);
    // Node->SetFrame(((Node->m_DelayStart - SDL_GetTicks())/Node->m_Delay)%Node->m_NbFrame);
}

void ProjectileEnemi::Update()
{
    SDLSprite *Tmp;

    // Loop the list and update the sprites
    for (Node = m_List.m_Next; Node != &m_List; Node = Node->m_Next)
    {
        if (WhatHappens() == 1)
        {
            continue;
        }
        Move();
        if (ShouldRemove() == 1)
        {
            continue;
        }
    }
    Draw(screen, 0, 0);                 // affichage des sprites autonomes
}

int BonusList::WhatHappens()
{
    SDLSprite *Tmp;

    // Est-ce que  le bnonus  a touche la starship
    if (Node->SpriteHit(starsheep) == TRUE)
    {
        printf("Gagne\n");
        starsheep->nbshoot++;
        Tmp = Node->m_Next;
        DelSprite(Node);
        Node = Tmp;
        return 1;
    }
    switch (Node->m_Frame)
    {
    case 0:
        Node->m_Frame++;
        break;
    case 1:
        if (Node->m_State == 1)
        {
            Node->m_Frame++;
        }
        else
        {
            Node->m_Frame--;
        }
        break;
    case 2:
        Node->m_Frame--;
        Node->m_State = -1;
        break;
    }
    return 0;
}

int ProjectileS::WantHappens()
{
    switch (Node->m_Frame)
    {
    case 0:
        Node->m_Frame++;
        break;
    case 1:
        if (Node->m_State == 1)
        {
            Node->m_Frame++;
        }
        else
        {
            Node->m_Frame--;
        }
        break;
    case 2:
        Node->m_Frame--;
        Node->m_State = -1;
        break;
    }
    return 0;
}

/*int ProjectileS:: ShouldRemove() {
 *      }*/
