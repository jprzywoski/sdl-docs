#include <stdio.h>
#include <string.h>                     // Thank's to David Olofson
#include <stdlib.h>
#include <time.h>
#include <SDL/SDL.h>
#include "SDLSprite.h"
#include "SDLSpriteList.h"

#define  TRUE		SDL_TRUE
#define  FALSE		SDL_FALSE

// program variables
#define Screen_w	640             // Width ecran affiche
#define Screen_h	480             // Height ecran affiche
#define Screen_bpp	16              // Nb de bits par pixel

class SDLStarShip: public SDLSprite
{
public:
// of the ship
// Ldri... left
// Rdri... right
// Udri... Up
// Ddri... Down
    int direction_starsheep;
    int Ldirection_starsheep;
    int Rdirection_starsheep;
    int Vdirection_starsheep;
    int life;
    int nbshoot;
    int power;

    SDLStarShip(const char *Filename, int x, int y, int n);     // :SDLSprite(Filename,x,y,n);

    ~SDLStarShip();
    void Update();
    void HideSprite();
};

class SDLUfo: public SDLSprite
{
    int life;
    int number;

public:
    SDLUfo(const char *n, int x, int y, int s); // :SDLSprite(n,x,y,s);
    ~SDLUfo();
};

void HideListSprite(SDLSpriteList *sl);

typedef struct _coord
{
    int x;
    int y;
    int frame;
} coord;

/////////////////////////////////////////////////
// Class direction enemi
////////////////////////////////////////////////
class direc_enemi
{
public:
    int xstart;
    int ystart;
    int delta_x;
    int delta_y;
    int nbelm;
    coord *elm;
    int xvelstart;
    int yvelstart;
    direc_enemi(char *filename, int deltax, int deltay);

    ~direc_enemi();
    void SetXEnemi(SDLSprite *Node, int nbe);
    void SetYEnemi(SDLSprite *Node, int nbe);
    void SetFrameEnemi(SDLSprite *Node);
    int GetNbElm();
};

//////////////////////////////////////////////////

// ------------------------------------------------------

class NotAliveSprite: public SDLSpriteList
{
    SDLSprite *_thesprite;

public:
    SDLSprite *Node;
    NotAliveSprite();

    ~NotAliveSprite();
    void SetSprite(SDLSprite *s);
    void Update();

    virtual int ShouldRemove();
    virtual int WhatHappens();
    virtual void Move();

    void HideListSprite();
};

// -----------------------------------------

class ExplosionL: public NotAliveSprite
{
public:
    ExplosionL(): NotAliveSprite() {}
    ~ExplosionL() {}

    virtual void Update();
};

class ProjectileEnemi: public NotAliveSprite
{
public:
    ProjectileEnemi() {}
    ~ProjectileEnemi() {}

    virtual int WhatHappens();
    virtual void Move();
    virtual void Update();
};

class BonusList: public NotAliveSprite
{
public:
    BonusList() {}
    ~BonusList() {}

    virtual int WhatHappens();
};

class ProjectileS: public NotAliveSprite
{
public:
    ProjectileS(): NotAliveSprite() {}
    ~ProjectileS() {}

    virtual int WantHappens();
};

class obj_enemi
{
protected:
    // SDLSprite *_enemi;
    SDLUfo *_enemi;
    SDLSpriteList *_enemilist;
    direc_enemi *_de;
    int y_enemi;
    int direction_enemi;
    int timetonextenemi;
    int nb_enemitotal;
    int nbenemibywave;

public:
    obj_enemi(SDLUfo *enemi, SDLSpriteList *enemilist, direc_enemi *dir_e, int nbebw);
    ~obj_enemi();
    void Update();
    int IsFinish();
    void HideListSprite();
};
