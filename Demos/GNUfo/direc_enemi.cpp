#include "ufo.h"

/////////////////////////////////////////////////
// Class direction enemi
////////////////////////////////////////////////
direc_enemi::direc_enemi(char *filename, int deltax, int deltay)
{
    delta_x = deltax;
    delta_y = deltay;

    FILE *f = NULL;

    if ((f = fopen(filename, "r")) != NULL)
    {
        fscanf(f, "%d\n", &xstart);
        fscanf(f, "%d\n", &ystart);
        fscanf(f, "%d\n", &nbelm);
        printf("*** %s %d %d nb %d\n", filename, xstart, ystart, nbelm);
        if ((elm = (coord *)malloc(sizeof (coord) * nbelm)) != NULL)
        {
            for (int n = 0; n < nbelm; n++)
            {
                fscanf(f, "%d %d %d\n", &elm[n].x, &elm[n].y, &elm[n].frame);
            }
        }
        fclose(f);
    }
}

direc_enemi::~direc_enemi() {}

void direc_enemi::SetXEnemi(SDLSprite *Node, int nbe)
{
    if (Node == NULL || elm == NULL)
    {
        return;
    }
    if (Node->m_Type >= nbelm)
    {
        return;
    }
    Node->m_PosX = elm[Node->m_Type].x + delta_x * nbe;
}

void direc_enemi::SetYEnemi(SDLSprite *Node, int nbe)
{
    if (Node == NULL || elm == NULL)
    {
        return;
    }
    if (Node->m_Type >= nbelm)
    {
        return;
    }
    Node->m_PosY = elm[Node->m_Type].y + delta_y * nbe;
}

void direc_enemi::SetFrameEnemi(SDLSprite *Node)
{
    if (Node == NULL || elm == NULL)
    {
        return;
    }
    if (Node->m_Type >= nbelm)
    {
        return;
    }
    Node->SetFrame(elm[Node->m_Type].frame);
    if (elm[Node->m_Type].frame == -1)
    {
        Node->SetFrame(((Node->m_DelayStart - SDL_GetTicks()) / Node->m_Delay) % Node->m_NbFrame);
    }
    else
    {
        Node->SetFrame(elm[Node->m_Type].frame);
    }
}

int direc_enemi::GetNbElm()
{
    return nbelm;
}

//////////////////////////////////////////////////
