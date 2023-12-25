/***************************************************************************
                          SDLSprite.cpp  -  1.2
                             -------------------
    begin                : Sept 21 2000
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
// based on : 	$Source: /usr/cvsroot/cdx/src/cdx/cdxsprite.cpp,v $
// 				$Author: Mindcry $
// 				$Revision: 2.10 $
//
// Ported to SDL and modified by Regis Quercioli / edorul@caramail.com
//
// 1.1 : Sprite's shadow handling with InitShadow, SetShadowOffset, SetShadowValue,
//       It's automaticaly drawn by Draw.
//
// 1.2 : * Position and velocity variables are only float type (in order to use
//       velocity in pixel/sec rather than pixel/frame in the main program).
//       * Add m_DelayStart var
//
//////////////////////////////////////////////////////////////////////////////////
#include "SDLSprite.h"

// added by janni
SDLSprite::SDLSprite(void)
{
	Init();
}


//////////////////////////////////////////////////////////////////////////////////
// Creates a sprite object from a pointer to a SDLSprite object holding the
// sprite's bitmap data.
//////////////////////////////////////////////////////////////////////////////////
SDLSprite::SDLSprite(SDLSprite* pTile)
{
	Init();
	Create(pTile);
}

//////////////////////////////////////////////////////////////////////////////////
// Creates the sprite object. Requires a pointer to your games SDLScreen object
// and the name of the bitmap in which the sprite is stored. The w and h 
// parameters refer to the width and height of a single frame of the sprite and 
// the num parameter refers to the total number of frames in the bitmap file.
//////////////////////////////////////////////////////////////////////////////////
SDLSprite::SDLSprite(const char* Filename, int w, int h, int num)
{
	Init();
	Create(Filename, w, h, num);
}

//////////////////////////////////////////////////////////////////////////////////
// Initializes the class
//////////////////////////////////////////////////////////////////////////////////
void SDLSprite::Init()
{
	m_PosX			= 0.0;
	m_PosY			= 0.0;
	m_PosZ			= 0.0;
	Dest_X			= 0;
	Dest_Y			= 0;
	m_VelR			= 0.0;
	m_VelX			= 0.0;
	m_VelY			= 0.0;
	m_Frame			= 0;
	m_NbFrame		= 1;
	m_NbTotFrame	= 0;
	m_Delay			= 0;
	m_DelayStart	= 0;
	m_State			= 0;
	m_Type			= 0;
	m_Angle			= 0;
	m_Shadowed		= SDL_FALSE;

	m_ShadowOffsetX = 0;
	m_ShadowOffsetY = 0;
	m_ShadowValue	= 0;
	m_ColorKey 		= 0;
	m_AlphaValue	= 0;
	m_TileCreated	= SDL_FALSE;

	m_Next			= NULL;
	m_Prev			= NULL;
}

//////////////////////////////////////////////////////////////////////////////////
// Destroys the surface and frees the memory.
//////////////////////////////////////////////////////////////////////////////////
SDLSprite::~SDLSprite(void)
{
	if(m_TileCreated)
		SDL_FreeSurface(m_Surface);

	m_Next = NULL;
	m_Prev = NULL;
}

//////////////////////////////////////////////////////////////////////////////////
// Creates the sprite object. Requires a valid pointer to a SDLSprite
//////////////////////////////////////////////////////////////////////////////////
SDL_bool SDLSprite::Create(SDLSprite *pTile)
{
	m_Surface = pTile->m_Surface;
	m_BlockWidth = pTile->m_BlockWidth;
	m_BlockHeight = pTile->m_BlockHeight;
	m_BlockNum = pTile->m_BlockNum;
	m_PixelWidth = m_Surface->w;
	m_PixelHeight = m_Surface->h;

	m_NbFrame = pTile->m_NbFrame;	
	m_NbTotFrame = pTile->m_NbTotFrame;		
	m_ShadowOffsetX = pTile->m_ShadowOffsetX; 	
	m_ShadowOffsetY = pTile->m_ShadowOffsetY;
	m_ShadowValue = pTile->m_ShadowValue;
	m_ShadowAlpha = pTile->m_ShadowAlpha;
	m_AlphaValue = pTile->m_AlphaValue;
	m_ColorKey = pTile->m_ColorKey;
	
	m_TileCreated	= SDL_FALSE;

	return SDL_TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Creates the sprite object. Requires
// the name of the bitmap in which the sprite is stored. The w and h parameters
// refer to the width and height of a single frame of the sprite and the num
// parameter refers to the total number of frames in the bitmap file.
//////////////////////////////////////////////////////////////////////////////////
SDL_bool SDLSprite::Create(const char* Filename, int w, int h, int num)
{
	SDL_Surface *spritebmp;
	
	if(Filename == NULL)
		return SDL_FALSE;
	
	/* Load a BMP sprite into a surface */
	spritebmp = SDL_LoadBMP(Filename);
	if ( spritebmp == NULL )
		return SDL_FALSE;
		
	/* Convert the sprite to the video format (maps colors) */
	m_Surface = SDL_DisplayFormat(spritebmp);
	SDL_FreeSurface(spritebmp);
	if ( m_Surface == NULL )
		return SDL_FALSE;


    m_BlockWidth = w;
	m_BlockHeight = h;
	m_BlockNum = num;
	m_PixelWidth = m_Surface->w;
	m_PixelHeight = m_Surface->h;
	
	m_PosX			= m_PosY = m_PosZ = 0;
	m_VelX			= m_VelY = 0;
	m_Frame			= 0;
	m_NbFrame		= 1;
	m_NbTotFrame	= num;
	m_Delay			= 0;
	m_DelayStart	= 0;
	m_State			= 0;
	m_Type			= 0;
	m_Angle			= 0;
	m_Next			= NULL;
	m_Prev			= NULL;
	m_ShadowValue	= 0;
	m_AlphaValue	= 0;
    m_TileCreated   = SDL_TRUE;

	return SDL_TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Staging area for all blit types.
//////////////////////////////////////////////////////////////////////////////////
void SDLSprite::Draw(SDL_Surface* lpSDLS, long int ScrnWorldX, long int ScrnWorldY)
{

	// lpSDLS		Destination SDLSurface pointer
	// ScrnWorldX	X position of the left edge of the screen in world space
	// ScrnWorldY	Y position of the top edge of the screen in world space

	SDL_Rect SpriteDestRect;	// Sprite RECT is defined in world space
	SDL_Rect SpriteSrcRect;		// RECT defining the source area for the BLT
	SDL_Rect ShadowDestRect;		// RECT defining the source area for the Shadow BLT
	short int TilesInWidth;		// Number of tiles in width of sprite tile surface
	long int SpriteSrcX;
	long int SpriteSrcY;

	// Validate the screen world space coordinates.
	if ( (ScrnWorldX == 0) || (ScrnWorldX < 0) )
		ScrnWorldX = 0;
	if ( (ScrnWorldY == 0) || (ScrnWorldY < 0) )
		ScrnWorldY = 0;

	// RECT defining the sprite in world space.
	SpriteDestRect.y	= (int)m_PosY-ScrnWorldY;
	SpriteDestRect.x	= (int)m_PosX-ScrnWorldX;
	SpriteDestRect.h	= m_BlockHeight;
	SpriteDestRect.w	=m_BlockWidth;

	// RECT defining the sprite in world space.
	ShadowDestRect.y	= (int)m_PosY-ScrnWorldY+m_ShadowOffsetY;
	ShadowDestRect.x	= (int)m_PosX-ScrnWorldX+m_ShadowOffsetX;
	ShadowDestRect.h	= m_BlockHeight;
	ShadowDestRect.w	=m_BlockWidth;

	// Get the number of tiles in the sprite tile surface width
	TilesInWidth = m_PixelWidth / m_BlockWidth;

	// Calc the upper left corner of the current frame of animation
	SpriteSrcX = (m_Frame % TilesInWidth) * m_BlockWidth;
	SpriteSrcY = (m_Frame / TilesInWidth) * m_BlockHeight;
	
	// Define the source RECT for the sprite BLT.
	SpriteSrcRect.y = SpriteSrcY;
	SpriteSrcRect.x = SpriteSrcX;
	SpriteSrcRect.h = m_BlockHeight;
	SpriteSrcRect.w = m_BlockWidth;

	if (m_Shadowed == SDL_TRUE)
	{
   		SDL_BlitSurface(m_Shadow, &SpriteSrcRect, lpSDLS, &ShadowDestRect);
   		SDL_BlitSurface(m_Surface, &SpriteSrcRect, lpSDLS, &SpriteDestRect);
   	}
   	else
   		SDL_BlitSurface(m_Surface, &SpriteSrcRect, lpSDLS, &SpriteDestRect);
   	
}

//////////////////////////////////////////////////////////////////////////////////////////
// Desc  : Determine la couleur de transparence
// Retour: Aucun
//////////////////////////////////////////////////////////////////////////////////////////
void SDLSprite::SetColorKey (Uint8 R, Uint8 G, Uint8 B)
{
	m_ColorKey=SDL_MapRGB(m_Surface->format, R, G, B);
	SDL_SetColorKey(m_Surface, SDL_SRCCOLORKEY,m_ColorKey);
}

//////////////////////////////////////////////////////////////////////////////////////////
// Desc  : the transparency color is the color of top-left pixel of the sprites' picture
// Retour: none
//////////////////////////////////////////////////////////////////////////////////////////
void SDLSprite::SetColorKey (void)
{
	switch(m_Surface->format->BitsPerPixel)
	{
		case 8:
			m_ColorKey = *(Uint8 *)m_Surface->pixels;
			SDL_SetColorKey(m_Surface, SDL_SRCCOLORKEY, m_ColorKey);
			break;
		case 16:
			m_ColorKey = *(Uint16 *)m_Surface->pixels;
			SDL_SetColorKey(m_Surface, SDL_SRCCOLORKEY, m_ColorKey);
			break;
		case 24:
		case 32:
			m_ColorKey = *(Uint32 *)m_Surface->pixels;
			SDL_SetColorKey(m_Surface, SDL_SRCCOLORKEY, m_ColorKey);
			break;
		default:
			break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// Desc  : Modifie Dest_X et Dest_Y en fonction du nouveau point à atteindre
// Retour: Aucun
//////////////////////////////////////////////////////////////////////////////////////////
void SDLSprite::SetObjective(int ObjX, int ObjY)
{
	Dest_X=ObjX;
	Dest_Y=ObjY;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Desc  : Calcule la direction du deplacement du Sprite en fonction de sa position
//         et de son objectif
// Retour: (float) Direction dont le sprite fait face
//////////////////////////////////////////////////////////////////////////////////////////
float SDLSprite::F_Direction()
{
	float Dir;

	// détermination de la direction du deplacement
	if(Dest_X>m_PosX)
		Dir=(float)(atan((Dest_Y-m_PosY)/(Dest_X-m_PosX)));
	if(Dest_X<m_PosX)
		Dir=(float)(atan((m_PosY-Dest_Y)/(m_PosX-Dest_X))+Pi);
	if((Dest_X-m_PosX)==0)
	{
		if((Dest_Y-m_PosY)>0)
			Dir=(float)Pi/2;
		else
			Dir=-(float)Pi/2;
	}

	m_Angle=Dir;
	return Dir;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Desc  : Oriente le Sprite en fonction de la direction de son déplacement
//         (img 0 -> vers le haut, puis rotation vers la droite)
// Retour: Aucun
//////////////////////////////////////////////////////////////////////////////////////////
void SDLSprite::Orientation(float Dir, short int NbPos)
{
	// détermination de l'image du vaisceau en fonction de sa direction
	m_Frame=(((short)((Pi/2+Dir)*NbPos/2/Pi))%NbPos)*m_NbFrame;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Desc  : * Deplace le sprite en fonction de sa vitesse suivant X et Y, s'il n'a pas
//           deja atteint sa destination
//         * DeltaTime (time between 2 frames) is used to translate velocity
//           from Pixel/Frame to Pixel/Sec
// Retour: Aucun
// Remarq: NbPos = Nb de frames representant l'orientation
//           if NbPos = -1 so the NbPos will be calculated like this :
//                NbPos = m_NbTotFrame / m_NbFrame (which mean that there only is
//                         normal mvt sprites i.e. no special mvt like jump, crunch...)
//////////////////////////////////////////////////////////////////////////////////////////
void SDLSprite::Mvt(double	DeltaTime, short int NbPos)
{
	float Dir;
	float VelRByFrame;
	
	if (NbPos == -1)
		NbPos = m_NbTotFrame / m_NbFrame;
		
	VelRByFrame = m_VelR * DeltaTime;

	if((fabs(Dest_X-m_PosX)>VelRByFrame) ||
			(fabs(Dest_Y-m_PosY)>VelRByFrame))
	{
		Dir=F_Direction();

		// calcul des nouvelles coordonnées
		m_PosX+=(float)(VelRByFrame*cos(Dir));
		m_PosY+=(float)(VelRByFrame*sin(Dir));

		Orientation(Dir, NbPos);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// Desc  : Verifie si le sprite est arrivé à son lieu de destination (fonction utilisee
//         en interne par Mvt(short int NbPos), mais peut aussi etre utilisee par le
//         programme principal)
// Retour: FALSE -> pas arrivé
//         TRUE  -> arrivé
//////////////////////////////////////////////////////////////////////////////////////////
SDL_bool SDLSprite::End_Mvt(SDLSprite* Sprite)
{
	if ((Sprite->m_PosX==Sprite->Dest_X) && (Sprite->m_PosY==Sprite->Dest_Y))
		return SDL_TRUE;

	return SDL_FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
// Checks for collisions between sprites. Returns TRUE if any part of pSprite 
// overlaps with the source sprite.
//////////////////////////////////////////////////////////////////////////////////
SDL_bool SDLSprite::SpriteHit(SDLSprite* pSprite)
{
	if(pSprite == this) return SDL_FALSE;

	RECT Rect1 = { (int)m_PosX, (int)m_PosY,
								 (int)m_PosX +m_BlockWidth,
								 (int)m_PosY + m_BlockHeight };
	RECT Rect2 = { (int)pSprite->m_PosX, (int)pSprite->m_PosY,
								 (int)pSprite->m_PosX + pSprite->m_BlockWidth,
								 (int)pSprite->m_PosY + pSprite->m_BlockHeight };

	if(Rect1.top > Rect2.bottom) return SDL_FALSE;
	else if(Rect1.bottom < Rect2.top) return SDL_FALSE;
	else if(Rect1.right < Rect2.left) return SDL_FALSE;
	else if(Rect1.left > Rect2.right) return SDL_FALSE;

	return SDL_TRUE;
}

/*
//////////////////////////////////////////////////////////////////////////////////
// Checks for collisions between the source sprite and tiles in a map. Returns 
// TRUE if any part of the sprite overlaps with a tile of value Tile in the specified map.
// In mMap->GetTile(x,y) it's necessary to keep within the X and Y boundaries!
// So min(X,MaxX-1) ensures that you can't exceed the max X or Y position
//////////////////////////////////////////////////////////////////////////////////
SDL_bool SDLSprite::TileMappyHit(SDLMappy* pMap, int Tile)
{
	int MapX, MapY, TileType, pMapPosX, pMapPosY, pMapWidth, pMapHeight;

	pMapPosX=pMap->MapGetXPosition();
	pMapPosY=pMap->MapGetYPosition();
	pMapWidth=pMap->GetMapWidthInBlocks();
	pMapHeight=pMap->GetMapHeightInBlocks();

	// Check top left
	MapX = (pMapPosX + m_PosX) / pMap->GetMapBlockWidth();
	  MapY = (pMapPosY + m_PosY) / pMap->GetMapBlockHeight();
	TileType = pMap->MapGetTile(min(MapX, pMapWidth-1), min(MapY, pMapHeight-1));
	if(TileType == Tile) return SDL_TRUE;

	// Check top right
	MapX = (pMapPosX + m_PosX + m_Tile->m_BlockWidth) / pMap->GetMapBlockWidth();
	MapY = (pMapPosY + m_PosY) / pMap->GetMapBlockHeight();
	TileType = pMap->MapGetTile(min(MapX, pMapWidth-1), min(MapY, pMapHeight-1));
	if(TileType == Tile) return SDL_TRUE;

	// Check bottom left
	MapX = (pMapPosX + m_PosX) / pMap->GetMapBlockWidth();
	MapY = (pMapPosY + m_PosY + m_Tile->m_BlockHeight) / pMap->GetMapBlockHeight();
	TileType = pMap->MapGetTile(min(MapX, pMapWidth-1), min(MapY, pMapHeight-1));
	if(TileType == Tile) return SDL_TRUE;

	// Check bottom right
	MapX = (pMapPosX + m_PosX + m_Tile->m_BlockWidth) / pMap->GetMapBlockWidth();
	MapY = (pMapPosY + m_PosY + m_Tile->m_BlockHeight) / pMap->GetMapBlockHeight();
	TileType = pMap->MapGetTile(min(MapX, pMapWidth-1), min(MapY, pMapHeight-1));
	if(TileType == Tile) return SDL_TRUE;

	return SDL_FALSE;
}
*/

//////////////////////////////////////////////////////////////////////////////////
// SDLSprite::SpriteHitPixel -	This function first performs a bounding box collision
//								detection check and if the 2 bounding boxes are
//								coliding it then performs a pixel level collision
//								detection check.
//
// Author					 -  John Hebert Aug 06, 1999
//
// Return Values			 -  TRUE  - if a pixel level collision was found
//							    FALSE - no collision
//////////////////////////////////////////////////////////////////////////////////
SDL_bool SDLSprite::SpriteHitPixel(SDLSprite* pSprite)
{	
	SDLSprite *rightSprite;
	SDLSprite *leftSprite;
	unsigned int  intersectWidth;
	unsigned int  intersectHeight;
	POINT LSPoint;
	POINT RSPoint;
	unsigned int  LSFrame;
	unsigned int  RSFrame;
	long int  LSPitch;
	long int  RSPitch;
	unsigned int  LSWidth;
	unsigned int  LSHeight;
	unsigned int  RSWidth;
	unsigned int  RSHeight;
	unsigned int  LSFramesInWidth;
	unsigned int  RSFramesInWidth;
	unsigned int  LSFrameX;
	unsigned int  LSFrameY;
	unsigned int  RSFrameX;
	unsigned int  RSFrameY;
	long int LSColorKey;
	long int RSColorKey;
	char* LSptr;
	char* RSptr;
	unsigned int  x,y;

	// If we are checking against ourseves, no collision.
	if(pSprite == this) 
		return SDL_FALSE;

	// Rectangle describing sprites current world location.
	RECT Rect1 = { (int)m_PosX,
				   (int)m_PosY,
				   (int)m_PosX + m_BlockWidth,
				   (int)m_PosY + m_BlockHeight };

	// Rectangle describing the current sprite we are checking colisions against.
	RECT Rect2 = { (int)pSprite->m_PosX,
				   (int)pSprite->m_PosY,
				   (int)pSprite->m_PosX + pSprite->m_BlockWidth,
				   (int)pSprite->m_PosY + pSprite->m_BlockHeight };

	// We must perform a bounding box collision detection first
	// otherwise we might get unexpected results.
	if(Rect1.top > Rect2.bottom) 
		return SDL_FALSE;
	else if(Rect1.bottom < Rect2.top) 
		return SDL_FALSE;
	else if(Rect1.right < Rect2.left) 
		return SDL_FALSE;
	else if(Rect1.left > Rect2.right) 
		return SDL_FALSE;

	// Determine which sprite is to the left and which is to the right.
	if (pSprite->m_PosX > m_PosX )
	{
		rightSprite = pSprite;
		leftSprite = this;
	}
	else
	{
		rightSprite = this;
		leftSprite = pSprite;
	}
	
	// Get the width and height of each sprite
	LSWidth  = leftSprite->m_BlockWidth;
	LSHeight = leftSprite->m_BlockHeight;
	RSWidth  = rightSprite->m_BlockWidth;
	RSHeight = rightSprite->m_BlockHeight;
	
	// Get the current frame for each sprite
	LSFrame = leftSprite->m_Frame;
	RSFrame = rightSprite->m_Frame;
	
	// Get the amount the two sprites overlap in the X direction
	intersectWidth = LSWidth - ((int)rightSprite->m_PosX - (int)leftSprite->m_PosX);

	// The intersecting width cannot be greater then the sprite width.
	if (intersectWidth > RSWidth)
		intersectWidth = RSWidth;


	// Determine which sprite is lower on the screen.
	if (leftSprite->m_PosY > rightSprite->m_PosY) 
	{
		LSPoint.y = 0;
		RSPoint.y = (int)leftSprite->m_PosY - (int)rightSprite->m_PosY;
		intersectHeight = RSHeight - ((int)leftSprite->m_PosY - (int)rightSprite->m_PosY);
		if (intersectHeight > LSWidth)
			intersectHeight = LSWidth;
	}
	else // left sprite is higher on the screen.
	{
		RSPoint.y = 0;
		LSPoint.y = (int)rightSprite->m_PosY - (int)leftSprite->m_PosY;
		intersectHeight = LSHeight - ((int)rightSprite->m_PosY - (int)leftSprite->m_PosY);
		if (intersectHeight > LSWidth)
			intersectHeight = LSWidth;
	}

	// Calculate the starting X position of collision.
	RSPoint.x = 0;
	LSPoint.x = (int)rightSprite->m_PosX - (int)leftSprite->m_PosX;

	// Determine the number of frames that fit across the surface.
	LSFramesInWidth = leftSprite->m_PixelWidth / LSWidth;
	RSFramesInWidth = rightSprite->m_PixelWidth / RSWidth;

	// Find x & y location on the surface(s) of first pixel in question
	LSFrameX = ((LSFrame % LSFramesInWidth) * LSWidth) + (unsigned int)LSPoint.x;
	RSFrameX = ((RSFrame % RSFramesInWidth) * RSWidth) + (unsigned int)RSPoint.x;
	LSFrameY = ((LSFrame / LSFramesInWidth) * LSHeight) + (unsigned int)LSPoint.y;
	RSFrameY = ((RSFrame / RSFramesInWidth) * RSHeight) + (unsigned int)RSPoint.y;

	// Get the color key for each surface.
	LSColorKey = leftSprite->m_Surface->format->colorkey;
	RSColorKey = rightSprite->m_Surface->format->colorkey;


	// We are assumming that both surfaces have the same pixel bit depth.
	switch(leftSprite->m_Surface->format->BitsPerPixel)

	{
	case 8:
		// The potential exists that the two sprites we are collition detecting
		// exist on the same surface.  In that case we only need to lock one surface.
		if (leftSprite->m_Surface == rightSprite->m_Surface)
		{
			if (SDL_LockSurface(leftSprite->m_Surface) != 0)
				return SDL_FALSE;

			// Get the pitch of the surface. Pitch is width of surface in bytes.
			LSPitch = leftSprite->m_Surface->pitch;
			RSPitch = LSPitch;

			// Initialize the pointers to the first pixel in the collition rectangle.
			LSptr      = (char*)leftSprite->m_Surface->pixels;
			LSptr     += LSFrameX + (LSFrameY * (unsigned int)LSPitch);

			RSptr      = (char*)rightSprite->m_Surface->pixels;
			RSptr     += RSFrameX + (RSFrameY * (unsigned int)RSPitch);


			for ( y = 0; y < intersectHeight; y++)
			{
				for (x = 0; x < intersectWidth; x++)
				{
					if ((*LSptr != (char)LSColorKey) && (*RSptr != (char)RSColorKey))
					{
						SDL_UnlockSurface(leftSprite->m_Surface);
						return SDL_TRUE;
					}
					LSptr ++;
					RSptr ++;
				}
				LSptr += LSPitch - intersectWidth;
				RSptr += RSPitch - intersectWidth;
			}
			SDL_UnlockSurface(leftSprite->m_Surface);
		}
		else // Sprites are on different surfaces.
		{
			if (SDL_LockSurface(leftSprite->m_Surface) != 0)
				return SDL_FALSE;
			if (SDL_LockSurface(rightSprite->m_Surface) != 0)
				return SDL_FALSE;

			// Get the pitch of the surface. Pitch is width of surface in bytes.
			LSPitch = leftSprite->m_Surface->pitch;
			RSPitch = rightSprite->m_Surface->pitch;

			// Initialize the pointers to the first pixel in the collition rectangle.
			LSptr      = (char*)leftSprite->m_Surface->pixels;
			LSptr     += LSFrameX + (LSFrameY * (unsigned int)LSPitch);

			RSptr      = (char*)rightSprite->m_Surface->pixels;
			RSptr     += RSFrameX + (RSFrameY * (unsigned int)RSPitch);


			for ( y = 0; y < intersectHeight; y++)
			{
				for (x = 0; x < intersectWidth; x++)
				{
					if ((*LSptr != (char)LSColorKey) && (*RSptr != (char)RSColorKey))
					{
						SDL_UnlockSurface(leftSprite->m_Surface);
						SDL_UnlockSurface(rightSprite->m_Surface);
						return SDL_TRUE;
					}
					LSptr ++;
					RSptr ++;
				}
				LSptr += LSPitch - intersectWidth;
				RSptr += RSPitch - intersectWidth;
			}
			SDL_UnlockSurface(leftSprite->m_Surface);
			SDL_UnlockSurface(rightSprite->m_Surface);
		}
		break;

	case 15:
	case 16:
		// The potential exists that the two sprites we are collition detecting
		// exist on the same surface.  In that case we only need to lock one surface.
		if (leftSprite->m_Surface == rightSprite->m_Surface)
		{
			if (SDL_LockSurface(leftSprite->m_Surface) != 0)
				return SDL_FALSE;

			// Get the pitch of the surface. Pitch is width of surface in bytes.
			LSPitch = leftSprite->m_Surface->pitch;
			RSPitch = LSPitch;

			// Initialize the pointers to the first pixel in the collition rectangle.
			LSptr      = (char*)leftSprite->m_Surface->pixels;
			LSptr     += (LSFrameX * 2) + (LSFrameY * (unsigned int)LSPitch);

			RSptr      = (char*)rightSprite->m_Surface->pixels;
			RSptr     += (RSFrameX * 2) + (RSFrameY * (unsigned int)RSPitch);


			for ( y = 0; y < intersectHeight; y++)
			{
				for (x = 0; x < intersectWidth; x++)
				{
					if ((*((short int*)LSptr) != (short int)LSColorKey)
								&& (*((short int*)RSptr) != (short int)RSColorKey))
					{
						SDL_UnlockSurface(leftSprite->m_Surface);
						return SDL_TRUE;
					}
					LSptr += 2;
					RSptr += 2;
				}
				LSptr += LSPitch - (intersectWidth * 2);
				RSptr += RSPitch - (intersectWidth * 2);
			}
			SDL_UnlockSurface(leftSprite->m_Surface);
		}
		else // Sprites are on different surfaces.
		{
			if (SDL_LockSurface(leftSprite->m_Surface) != 0)
				return SDL_FALSE;
			if (SDL_LockSurface(rightSprite->m_Surface) != 0)
				return SDL_FALSE;

			// Get the pitch of the surface. Pitch is width of surface in bytes.
			LSPitch = leftSprite->m_Surface->pitch;
			RSPitch = rightSprite->m_Surface->pitch;

			// Initialize the pointers to the first pixel in the collition rectangle.
			LSptr      = (char*)leftSprite->m_Surface->pixels;
			LSptr     += (LSFrameX * 2) + (LSFrameY * (unsigned int)LSPitch);

			RSptr      = (char*)rightSprite->m_Surface->pixels;
			RSptr     += (RSFrameX * 2) + (RSFrameY * (unsigned int)RSPitch);


			for ( y = 0; y < intersectHeight; y++)
			{
				for (x = 0; x < intersectWidth; x++)
				{
					if ((*((short int*)LSptr) != (short int)LSColorKey)
									&& (*((short int*)RSptr) != (short int)RSColorKey))
					{
						SDL_UnlockSurface(leftSprite->m_Surface);
						SDL_UnlockSurface(rightSprite->m_Surface);
						return SDL_TRUE;
					}
					LSptr += 2;
					RSptr += 2;
				}
				LSptr += LSPitch - (intersectWidth * 2);
				RSptr += RSPitch - (intersectWidth * 2);
			}
			SDL_UnlockSurface(leftSprite->m_Surface);
			SDL_UnlockSurface(rightSprite->m_Surface);
		}
		break;

	case 24:
		// The potential exists that the two sprites we are collition detecting
		// exist on the same surface.  In that case we only need to lock one surface.
		if (leftSprite->m_Surface == rightSprite->m_Surface)
		{
			if (SDL_LockSurface(leftSprite->m_Surface) != 0)
				return SDL_FALSE;

			// Get the pitch of the surface. Pitch is width of surface in bytes.
			LSPitch = leftSprite->m_Surface->pitch;
			RSPitch = LSPitch;

			// Initialize the pointers to the first pixel in the collition rectangle.
			LSptr      = (char*)leftSprite->m_Surface->pixels;
			LSptr     += (LSFrameX * 3) + (LSFrameY * (unsigned int)LSPitch);

			RSptr      = (char*)rightSprite->m_Surface->pixels;
			RSptr     += (RSFrameX * 3) + (RSFrameY * (unsigned int)RSPitch);


			for ( y = 0; y < intersectHeight; y++)
			{
				for (x = 0; x < intersectWidth; x++)
				{
					if (( (*((long int*)LSptr) & 0xFFFFFF) != (long int)LSColorKey)
								&& ( (*((long int*)RSptr) & 0xFFFFFF) != (long int)RSColorKey))
					{
						SDL_UnlockSurface(leftSprite->m_Surface);
						return SDL_TRUE;
					}
					LSptr += 3;
					RSptr += 3;
				}
				LSptr += LSPitch - (intersectWidth * 3);
				RSptr += RSPitch - (intersectWidth * 3);
			}
			SDL_UnlockSurface(leftSprite->m_Surface);
		}
		else // Sprites are on different surfaces.
		{
			if (SDL_LockSurface(leftSprite->m_Surface) != 0)
				return SDL_FALSE;
			if (SDL_LockSurface(rightSprite->m_Surface) != 0)
				return SDL_FALSE;

			// Get the pitch of the surface. Pitch is width of surface in bytes.
			LSPitch = leftSprite->m_Surface->pitch;
			RSPitch = rightSprite->m_Surface->pitch;

			// Initialize the pointers to the first pixel in the collition rectangle.
			LSptr      = (char*)leftSprite->m_Surface->pixels;
			LSptr     += (LSFrameX * 3) + (LSFrameY * (unsigned int)LSPitch);

			RSptr      = (char*)rightSprite->m_Surface->pixels;
			RSptr     += (RSFrameX * 3) + (RSFrameY * (unsigned int)RSPitch);


			for ( y = 0; y < intersectHeight; y++)
			{
				for (x = 0; x < intersectWidth; x++)
				{
					if (( (*((long int*)LSptr) & 0xFFFFFF) != (long int)LSColorKey)
									&& ( (*((long int*)RSptr) & 0xFFFFFF) != (long int)RSColorKey))
					{
						SDL_UnlockSurface(leftSprite->m_Surface);
						SDL_UnlockSurface(rightSprite->m_Surface);
						return SDL_TRUE;
					}
					LSptr += 3;
					RSptr += 3;
				}
				LSptr += LSPitch - (intersectWidth * 3);
				RSptr += RSPitch - (intersectWidth * 3);
			}
			SDL_UnlockSurface(leftSprite->m_Surface);
			SDL_UnlockSurface(rightSprite->m_Surface);
		}
		break;

	case 32:
		// The potential exists that the two sprites we are collition detecting
		// exist on the same surface.  In that case we only need to lock one surface.
		if (leftSprite->m_Surface == rightSprite->m_Surface)
		{
			if (SDL_LockSurface(leftSprite->m_Surface) != 0)
				return SDL_FALSE;

			// Get the pitch of the surface. Pitch is width of surface in bytes.
			LSPitch = leftSprite->m_Surface->pitch;
			RSPitch = LSPitch;

			// Initialize the pointers to the first pixel in the collition rectangle.
			LSptr      = (char*)leftSprite->m_Surface->pixels;
			LSptr     += (LSFrameX * 4) + (LSFrameY * (unsigned int)LSPitch);

			RSptr      = (char*)rightSprite->m_Surface->pixels;
			RSptr     += (RSFrameX * 4) + (RSFrameY * (unsigned int)RSPitch);


			for ( y = 0; y < intersectHeight; y++)
			{
				for (x = 0; x < intersectWidth; x++)
				{
					if ((*((long int*)LSptr) != (long int)LSColorKey)
								&& (*((long int*)RSptr) != (long int)RSColorKey))
					{
						SDL_UnlockSurface(leftSprite->m_Surface);
						return SDL_TRUE;
					}
					LSptr += 4;
					RSptr += 4;
				}
				LSptr += LSPitch - (intersectWidth * 4);
				RSptr += RSPitch - (intersectWidth * 4);
			}
			SDL_UnlockSurface(leftSprite->m_Surface);
		}
		else // Sprites are on different surfaces.
		{
			if (SDL_LockSurface(leftSprite->m_Surface) != 0)
				return SDL_FALSE;
			if (SDL_LockSurface(rightSprite->m_Surface) != 0)
				return SDL_FALSE;

			// Get the pitch of the surface. Pitch is width of surface in bytes.
			LSPitch = leftSprite->m_Surface->pitch;
			RSPitch = rightSprite->m_Surface->pitch;

			// Initialize the pointers to the first pixel in the collition rectangle.
			LSptr      = (char*)leftSprite->m_Surface->pixels;
			LSptr     += (LSFrameX * 4) + (LSFrameY * (unsigned int)LSPitch);

			RSptr      = (char*)rightSprite->m_Surface->pixels;
			RSptr     += (RSFrameX * 4) + (RSFrameY * (unsigned int)RSPitch);


			for ( y = 0; y < intersectHeight; y++)
			{
				for (x = 0; x < intersectWidth; x++)
				{
					if (( (*((long int*)LSptr) & 0xFFFFFF) != (long int)LSColorKey)
								&& ( (*((long int*)RSptr) & 0xFFFFFF) != (long int)RSColorKey))
					{
						SDL_UnlockSurface(leftSprite->m_Surface);
						SDL_UnlockSurface(rightSprite->m_Surface);
						return SDL_TRUE;
					}
					LSptr += 4;
					RSptr += 4;
				}
				LSptr += LSPitch - (intersectWidth * 4);
				RSptr += RSPitch - (intersectWidth * 4);
			}
			SDL_UnlockSurface(leftSprite->m_Surface);
			SDL_UnlockSurface(rightSprite->m_Surface);
		}
		break;

	default:
		break;
	}
	return SDL_FALSE;
}

SDL_bool SDLSprite::InitShadow(int OffX, int OffY, Uint8 Shade, Uint32 Alpha)
{
	int i;
	char *PSurface,*PShadow;
	
	// create a new surface only for the shadowed sprites
	m_Shadow = SDL_CreateRGBSurface(SDL_HWSURFACE, m_Surface->w, m_Surface->h,
						m_Surface->format->BitsPerPixel, m_Surface->format->Rmask,
						m_Surface->format->Gmask, m_Surface->format->Bmask,
						m_Surface->format->Amask);
	if (m_Shadow == NULL)
    	return SDL_FALSE;

    // fill this surface with grey pixels at the right place
    m_ShadowValue = SDL_MapRGB(m_Surface->format, Shade, Shade, Shade);
	PSurface=(char *)m_Surface->pixels;
	PShadow=(char *)m_Shadow->pixels;
	switch(m_Surface->format->BitsPerPixel)
	{
		case 8:
			for(i=0; i<m_Surface->w*m_Surface->h; i++)
			{
				if (*((Uint8*)PSurface) != (Uint8)m_ColorKey)
					*((Uint8*)PShadow) = (Uint8)m_ShadowValue;
				else
					*((Uint8*)PShadow) = (Uint8)m_ColorKey;
				PSurface++;
				PShadow++;
			}
			break;
		case 16:
			for(i=0; i<m_Surface->w*m_Surface->h; i++)
			{
				if (*((Uint16*)PSurface) != (Uint16)m_ColorKey)
					*((Uint16*)PShadow) = (Uint16)m_ShadowValue;
				else
					*((Uint16*)PShadow) = (Uint16)m_ColorKey;
				PSurface+=2;
				PShadow+=2;
			}
			break;
		case 24:
			for(i=0; i<m_Surface->w*m_Surface->h; i++)
			{
				if (*((Uint32*)PSurface) != (Uint32)m_ColorKey)
					*((Uint32*)PShadow) = (Uint32)m_ShadowValue;
				else
					*((Uint32*)PShadow) = (Uint32)m_ColorKey;
				PSurface+=3;
				PShadow+=3;
			}
			break;
		case 32:
			for(i=0; i<m_Surface->w*m_Surface->h; i++)
			{
				if (*((Uint32*)PSurface) != (Uint32)m_ColorKey)
					*((Uint32*)PShadow) = (Uint32)m_ShadowValue;
				else
					*((Uint32*)PShadow) = (Uint32)m_ColorKey;
				PSurface+=4;
				PShadow+=4;
			}
			break;
		default:
			return SDL_FALSE;
			break;
	}

    m_ShadowAlpha = Alpha;
    SDL_SetAlpha(m_Shadow, SDL_SRCALPHA, Alpha);
	
	m_ShadowOffsetX = OffX;
	m_ShadowOffsetY = OffY;
	
	SDL_SetColorKey(m_Shadow, SDL_SRCCOLORKEY,m_ColorKey);
    m_Shadowed = SDL_TRUE;
	return SDL_TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// Sets the offset of the shadow to use when bltting.
//////////////////////////////////////////////////////////////////////////////////
void SDLSprite::SetShadowOffset(int dx, int dy)
{
	m_ShadowOffsetX = dx;
	m_ShadowOffsetY = dy;
}

//////////////////////////////////////////////////////////////////////////////////
// Sets the shadow value and the alpha value of the shadow
//////////////////////////////////////////////////////////////////////////////////
void SDLSprite::SetShadowValue(Uint8 Shade, Uint32 Alpha)
{
	int i;
	char *PSurface,*PShadow;
	
    m_ShadowValue = SDL_MapRGB(m_Surface->format, Shade, Shade, Shade);
	PSurface=(char *)m_Surface->pixels;
	PShadow=(char *)m_Shadow->pixels;
	switch(m_Surface->format->BitsPerPixel)
	{
		case 8:
			for(i=0; i<m_Surface->w*m_Surface->h; i++)
			{
				if (*((Uint8*)PSurface) != (Uint8)m_ColorKey)
					*((Uint8*)PShadow) = (Uint8)m_ShadowValue;
				else
					*((Uint8*)PShadow) = (Uint8)m_ColorKey;
				PSurface++;
				PShadow++;
			}
			break;
		case 16:
			for(i=0; i<m_Surface->w*m_Surface->h; i++)
			{
				if (*((Uint16*)PSurface) != (Uint16)m_ColorKey)
					*((Uint16*)PShadow) = (Uint16)m_ShadowValue;
				else
					*((Uint16*)PShadow) = (Uint16)m_ColorKey;
				PSurface+=2;
				PShadow+=2;
			}
			break;
		case 24:
			for(i=0; i<m_Surface->w*m_Surface->h; i++)
			{
				if (*((Uint32*)PSurface) != (Uint32)m_ColorKey)
					*((Uint32*)PShadow) = (Uint32)m_ShadowValue;
				else
					*((Uint32*)PShadow) = (Uint32)m_ColorKey;
				PSurface+=3;
				PShadow+=3;
			}
			break;
		case 32:
			for(i=0; i<m_Surface->w*m_Surface->h; i++)
			{
				if (*((Uint32*)PSurface) != (Uint32)m_ColorKey)
					*((Uint32*)PShadow) = (Uint32)m_ShadowValue;
				else
					*((Uint32*)PShadow) = (Uint32)m_ColorKey;
				PSurface+=4;
				PShadow+=4;
			}
			break;
		default:
			break;
	}
	
	m_ShadowAlpha=Alpha;
    SDL_SetAlpha(m_Shadow, SDL_SRCALPHA, Alpha);
}
