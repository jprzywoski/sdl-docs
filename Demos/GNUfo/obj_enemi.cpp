#include "ufo.h"

extern SDL_Surface *screen;
extern SDLStarShip *starsheep;
extern BonusList BonusL;
extern ProjectileS ProjectS;
extern ProjectileEnemi projectE;
extern ExplosionL ExploL;
extern SDLSprite *bonus;
extern SDLSprite *enemi;
extern SDLSprite *explosion;
extern SDLSprite *projectileenemi;
extern int powerfire;
extern int liveenemi;
extern int lescore;

obj_enemi::~obj_enemi() {}

obj_enemi::obj_enemi(SDLUfo *enemi, SDLSpriteList *enemilist, direc_enemi *dir_e, int nbebw)
{
    _enemi = enemi;
    _enemilist = enemilist;
    _de = dir_e;
    // printf("%d %d \n",_de->xstart,_de->ystart);
    y_enemi = 10;
    direction_enemi = 1;
    timetonextenemi = 0;
    nb_enemitotal = 0;
    nbenemibywave = nbebw;
}

void obj_enemi::HideListSprite()
{
    SDL_Rect dstrect;
    SDLSprite *Node;

    for (Node = _enemilist->m_List.m_Next; Node != &_enemilist->m_List; Node = Node->m_Next)
    {
        dstrect.x = (int)Node->m_PosX;
        dstrect.y = (int)Node->m_PosY;
        dstrect.w = Node->m_BlockWidth;
        dstrect.h = Node->m_BlockHeight;

        SDL_FillRect(screen, &dstrect, 0);
    }
}

void obj_enemi::Update()
{
    SDLSprite *Node, *Tmp, *Tmp2;
    SDLSprite *Node2;
    SDL_Rect dstrect;
    int enemideletebyrocket;            // A boolean variable to know if the enemi has been delete of the list in the loop of ProjectS

    for (Node = _enemilist->m_List.m_Next; Node != &_enemilist->m_List; Node = Node->m_Next)
    {
        _de->SetXEnemi(Node, Node->m_Number);
        _de->SetYEnemi(Node, Node->m_Number);
        _de->SetFrameEnemi(Node);
        Node->m_Type++;
        if (Node->m_Type >= _de->GetNbElm())
        {
            Tmp = Node->m_Next;
            _enemilist->DelSprite(Node);
            if (Tmp == NULL)
            {
                break;
            }
            Node = Tmp;
            continue;
        }
        //
        // If the enemi encounter your starship
        //
        if (Node->SpriteHit(starsheep) == TRUE)
        {
            dstrect.x = (int)Node->m_PosX;
            dstrect.y = (int)Node->m_PosY;
            dstrect.w = Node->m_BlockWidth;
            dstrect.h = Node->m_BlockHeight;

            SDL_FillRect(screen, &dstrect, 0);

            Tmp = Node->m_Next;

            _enemilist->DelSprite(Node);
            if (_enemilist->m_nSprites == 0)
            {
                BonusL.AddSprite(bonus, 0, dstrect.x, dstrect.y - 34, 0, 50, 1, 0, 1000, (int)SDL_GetTicks());
            }
            starsheep->life += 100;
            ExploL.AddSprite(explosion, 0, dstrect.x, dstrect.y - 34, 0, 0, 0, 0, 1000, (int)SDL_GetTicks());
            if (Tmp == NULL)
            {
                break;
            }
            Node = Tmp;

            continue;
        }
        //
        // If the enemi encounter your missiles
        //
        enemideletebyrocket = 0;
        for (Node2 = ProjectS.m_List.m_Next; Node2 != &ProjectS.m_List; Node2 = Node2->m_Next)
        {
            if (Node2->SpriteHit(Node) == TRUE)
            {
                // On efface le projectile
                dstrect.x = (int)Node->m_PosX;
                dstrect.y = (int)Node->m_PosY;
                dstrect.w = Node->m_BlockWidth;
                dstrect.h = Node->m_BlockHeight;

                // m_State converve le niveau de vie des appareils enemis
                // ici il est possitionne a deux mias devrais etre variable voir AddSprite

                Node->m_State -= Node2->m_State;
                if (Node->m_State < 1)
                {
                    Tmp2 = Node->m_Next;

                    enemideletebyrocket = 1;
                    _enemilist->DelSprite(Node);
                    if (_enemilist->m_nSprites == 0)
                    {
                        BonusL.AddSprite(bonus, 0, dstrect.x, dstrect.y - 34, 0, 50, 1, 0, 1000, (int)SDL_GetTicks());
                    }
                    SDL_FillRect(screen, &dstrect, 0);
                    if (Tmp2 == NULL)
                    {
                        break;
                    }
                    Node = Tmp2;
                }
                // L'ajout de l'explosion n'est peut etre pas forcement la bonne idee
                // Il faudrait plutot faire des sprite lorsque qu'ils sont touches

                ExploL.AddSprite(explosion, 0, dstrect.x, dstrect.y - 34, 0, 0, 0, 0, 1000, (int)SDL_GetTicks());
                lescore++;
                ProjectS.DelSprite(Node2);
                break;
            }
        }
        if (enemideletebyrocket == 1)
        {
            continue;
        }
        //
        // Enemi fire
        //
        if (rand() % 200 == 1)
        {
            // 10 sont les points a retirer si le vaisseau est touche
            projectE.AddSprite(projectileenemi, 0, Node->m_PosX, Node->m_PosY, starsheep->m_PosX - Node->m_PosX,
                               starsheep->m_PosY - Node->m_PosY, powerfire, 0, 1000, SDL_GetTicks());
        }
    }
    //
    // New enemi appears
    //
    // nb_enemitotal means the number of enemi by wave
    if (timetonextenemi == 20 && nb_enemitotal < nbenemibywave)
    {
        _enemilist->AddSprite((SDLSprite *)_enemi, 0, _de->xstart * _de->delta_x * _enemilist->m_nSprites, _de->ystart *
                              _de->delta_y * _enemilist->m_nSprites, _de->xvelstart, _de->yvelstart, liveenemi, 0, 300, SDL_GetTicks());
        y_enemi += 30 * direction_enemi;
        if (y_enemi > (Screen_w - 32 - enemi->m_ShadowOffsetX))
        {
            direction_enemi *= -1;
        }
        if (y_enemi < 0)
        {
            direction_enemi *= -1;
        }
        timetonextenemi = 0;
        nb_enemitotal++;
    }
    timetonextenemi++;
    _enemilist->Draw(screen, 0, 0);
}

int obj_enemi::IsFinish()
{
    if (nb_enemitotal >= nbenemibywave && _enemilist->m_nSprites == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
