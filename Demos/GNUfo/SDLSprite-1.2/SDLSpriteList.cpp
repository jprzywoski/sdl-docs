/***************************************************************************
                          SDLSpriteList.cpp  -  1.1
                             -------------------
    begin                : Tue Aug 15 2000
    copyright            : (C) 2000 by regis Quercioli
    email                : edorul@caramail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
//////////////////////////////////////////////////////////////////////////////////
// based on :	$Source: /usr/cvsroot/cdx/src/cdx/cdxspritelist.cpp,v $
// 				$Author: MICHAELR $
// 				$Revision: 2.2 $
//
// Ported to SDL and modified by Regis Quercioli / edorul@caramail.com
//
// 1.1 : Add DelayStart as parametre in AddSprite function
//
//////////////////////////////////////////////////////////////////////////////////
#include "SDLSpriteList.h"

//////////////////////////////////////////////////////////////////////////////////
// Default constructor.
//////////////////////////////////////////////////////////////////////////////////
SDLSpriteList::SDLSpriteList(void)

{
	m_List.m_Next = m_List.m_Prev = &m_List;
	m_List.SetPos(0,0);
	m_List.SetVel(0,0);

	m_nSprites = 0;
}

//////////////////////////////////////////////////////////////////////////////////
// Default contructor.
//////////////////////////////////////////////////////////////////////////////////
SDLSpriteList::~SDLSpriteList(void)
{
}

//////////////////////////////////////////////////////////////////////////////////
// Adds a sprite to the sprite list. Only a pointer to a SDLTile object
// is required, however you can also set up the sprite as required.
//////////////////////////////////////////////////////////////////////////////////
SDLSprite *SDLSpriteList::AddSprite(SDLSprite* pTile, int Type, int PosX, int PosY,
					int VelX, int VelY, int State, int Frame, int Delay, Uint32 DelayStart)
{
	SDLSprite* Node;

	// Create a new sprite
	Node = new SDLSprite(pTile);
	if(Node == NULL) 
		return Node;

	// Setup sprite
	Node->SetType(Type);
	Node->SetPos((float)PosX,(float) PosY);
	Node->SetVel(VelX, VelY);
	Node->SetState(State);
	Node->SetFrame(Frame);
	Node->SetDelay(Delay);
	Node->m_DelayStart = DelayStart;

	Node->SetNumber(m_nSprites);
	
	// Add sprite too list
	Node->m_Next = m_List.m_Next;
	Node->m_Prev = &m_List;
	m_List.m_Next->m_Prev = Node;
	m_List.m_Next = Node;

	m_nSprites++;

	return Node;
}

//////////////////////////////////////////////////////////////////////////////////
// Adds a sprite to the sprite list. A pointer to an already existing SDLSprite
// is required.
//////////////////////////////////////////////////////////////////////////////////
SDLSprite* SDLSpriteList::AddSprite(SDLSprite* pNode)
{
	pNode->m_Next = m_List.m_Next;
	pNode->m_Prev = &m_List;
	m_List.m_Next->m_Prev = pNode;
	m_List.m_Next = pNode;

	m_nSprites++;

	return pNode;
}

//////////////////////////////////////////////////////////////////////////////////
// Removes a sprite from the list.
//////////////////////////////////////////////////////////////////////////////////
void SDLSpriteList::DelSprite(SDLSprite* pNode)
{
        printf("SDLSpriteList::DelSprite %d \n",pNode->m_Number);
	if(m_List.m_Next == &m_List) 
		return;

	m_nSprites--;

	pNode->m_Next->m_Prev = pNode->m_Prev;
	pNode->m_Prev->m_Next = pNode->m_Next;

	delete pNode;
}

//////////////////////////////////////////////////////////////////////////////////
// Draws all the sprites in the list to the surface pointed to by lpDDS.
//////////////////////////////////////////////////////////////////////////////////
void SDLSpriteList::Draw(SDL_Surface* lpSDLS, long int ScrnWorldX, long int ScrnWorldY)
{
	SDLSprite* Node;


	// Loop the list and draw the objects
	for(Node = m_List.m_Next; Node != &m_List; Node = Node->m_Next)
	{

		Node->Draw(lpSDLS, ScrnWorldX, ScrnWorldY);
	}
}
