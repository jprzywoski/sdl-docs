/***************************************************************************
                          flares.c  -  description
                             -------------------
    begin                : Sat Nov 18 2000
    copyright            : (C) 2000 by Stephan Uhlmann
    email                : su@codewizards.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "flares.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "sdldemotools.h"
#include <SDL/SDL_image.h>


// called every time the mixer has mixed a chunk of data
// we go through it and try to determine if the chinese image
// has to bw flipped
void flares_mix_callback(void *udata, Uint8 *stream, int len)
{
	int i,h;
	flares_mix_stream=(Sint16*)stream;
	flares_mix_len=len;

	h=0;
	for (i=0;i<len/2;i++)
	{
		if ( ((Sint16*)stream)[i]>5000) h++;
		i++;
	}
	if (h>16) flares_image_flip=1;
}



void flares_loadData(const char* musicfile)
{
	int i;
	char filename[256];
	SDL_Surface *s;



	// preload chinese characters
	for (i=0;i<=18;i++)
	{
		sprintf(filename,"../data/chinese/%d.png",i);
		s = IMG_Load(filename);
		if ( s != NULL )
			fprintf(stdout, "Loaded image %s\n",filename);
		else
		{
			sprintf(filename,"data/chinese/%d.png",i);
			s = IMG_Load(filename);
			if ( s != NULL )
				fprintf(stdout, "Loaded image %s\n",filename);
			else
				fprintf(stderr, "Couldn't load %s: %s\n",filename, SDL_GetError());
		}
		flares_chineseImages[i] = SDL_DisplayFormatAlpha(s);
		SDL_SetAlpha(flares_chineseImages[i],SDL_SRCALPHA | SDL_RLEACCEL,128);
	}

	sprintf(filename,"../data/flare0.png");
	s=IMG_Load(filename);
	if ( s != NULL )
		fprintf(stdout, "Loaded image %s\n",filename);
	else
	{
		sprintf(filename,"data/flare0.png");
		s=IMG_Load(filename);
		if ( s != NULL )
			fprintf(stdout, "Loaded image %s\n",filename);
		else
			fprintf(stderr, "Couldn't load %s: %s\n",filename, SDL_GetError());
	}
	flares_back = SDL_DisplayFormat(s);


	sprintf(filename,"../data/flare1.png");
	s=IMG_Load(filename);
	if ( s != NULL )
		fprintf(stdout, "Loaded image %s\n",filename);
	else
	{
		sprintf(filename,"data/flare1.png");
		s=IMG_Load(filename);
		if ( s != NULL )
			fprintf(stdout, "Loaded image %s\n",filename);
		else
			fprintf(stderr, "Couldn't load %s: %s\n",filename, SDL_GetError());
	}
	flares_flare = SDL_DisplayFormatAlpha(s);
	SDL_SetAlpha(flares_flare,SDL_SRCALPHA | SDL_RLEACCEL,128);

	flares_music=NULL;
	if (musicfile!=NULL)
	{
		sprintf(filename,"%s",musicfile);
		flares_music = Mix_LoadMUS(filename);
		if ( flares_music == NULL )
			fprintf(stderr, "Couldn't load %s: %s\n",filename, Mix_GetError());
	}

	if ( flares_music != NULL )
	{
		fprintf(stdout, "Loaded music %s\n",filename);
	}
	else
	{
		sprintf(filename,"../data/totalywikkid.mod");
		flares_music = Mix_LoadMUS(filename);
		if ( flares_music != NULL )
			fprintf(stdout, "Loaded music %s\n",filename);
		else
		{
			sprintf(filename,"data/totalywikkid.mod");
			flares_music = Mix_LoadMUS(filename);
			if ( flares_music != NULL )
				fprintf(stdout, "Loaded music %s\n",filename);
			else
				fprintf(stderr, "Couldn't load %s: %s\n",filename, Mix_GetError());
		}
	}


 fprintf(stdout, "\n");
}


void flares_run(SDL_Surface *screen)
{
	int i,chinese_i=0;
	int x,y;
	SDL_Event event;
	SDL_Rect back_rect,left_rect,right_rect,chinese_rect,flare_rects[8];
	int flares_d[8];
	float flares_angle,flares_rad,flares_rad_delta;



	// callback function for mixer
	Mix_SetPostMix(flares_mix_callback,0);

	// setup some rects

	// chinese character
	chinese_rect.x = screen->w/2-flares_chineseImages[0]->w/2;
	chinese_rect.y = screen->h/2-flares_chineseImages[0]->h/2;
	chinese_rect.w = flares_chineseImages[0]->w;
	chinese_rect.h = flares_chineseImages[0]->h;

	// background
	back_rect.x = screen->w/2-flares_back->w/2;
	back_rect.y = screen->h/2-flares_back->h/2;
	back_rect.w = flares_back->w;
	back_rect.h = flares_back->h;

	// space left from background
	left_rect.x = 0;
	left_rect.y = 0;
	left_rect.w = back_rect.x;
	left_rect.h = screen->h;

	// space right from background
	right_rect.x = back_rect.x+back_rect.w;
	right_rect.y = 0;
	right_rect.w = screen->w-right_rect.x;
	right_rect.h = screen->h;


	// flares
	for (i=0;i<8;i++)
	{
		flare_rects[i].x=screen->w/2-flares_flare->w/2;
		flare_rects[i].y=screen->h/2-flares_flare->h/2;
		flare_rects[i].w = flares_flare->w;
		flare_rects[i].h = flares_flare->h;
	}

	flares_d[0]=-20;
	flares_d[1]=-14;
	flares_d[2]=-10;
	flares_d[3]=- 2;
	flares_d[4]=+ 5;
	flares_d[5]=+ 8;
	flares_d[6]=+16;
	flares_d[7]=+22;

	flares_angle=0;
	flares_rad=0;
	flares_rad_delta=0.05;


	// main loop
	SDL_PollEvent(&event);
	while ((event.type!=SDL_QUIT) && (event.type!=SDL_KEYDOWN))
	{
		if (!Mix_PlayingMusic())
			Mix_PlayMusic(flares_music, 0);

		// clearscreen
		SDL_FillRect(flares_buffer,&left_rect,0);
		SDL_BlitSurface(flares_back, NULL, flares_buffer, &back_rect);
		SDL_FillRect(flares_buffer,&right_rect,0);


		// draw scopes
		if (flares_mix_stream!=NULL)
			for (x=0;x<1024;x+=2)
			{
				y=(flares_mix_stream[x]/256);
				((Uint8*)(flares_buffer->pixels))[y*flares_buffer->pitch+x*2+128*flares_buffer->pitch+256]=192;
				y=(flares_mix_stream[x+1]/256);
				((Uint8*)(flares_buffer->pixels))[y*flares_buffer->pitch+x*2+272*flares_buffer->pitch+256]=192;
			}


		// draw flares
		for (i=0;i<8;i++)
		{
			flare_rects[i].x=(screen->w/2-flares_flare->w/2)+flares_d[i]*cos(flares_angle+flares_d[i]/3)*flares_rad;
			flare_rects[i].y=(screen->h/2-flares_flare->h/2)+flares_d[i]*sin(flares_angle+flares_d[i]/3)*flares_rad;

			SDL_BlitSurface(flares_flare, NULL, flares_buffer, &flare_rects[i]);
		}

		flares_angle=flares_angle+0.05;
		flares_rad=flares_rad+flares_rad_delta;
		if (abs(flares_rad)>12.566368) // 4*3.141592
			flares_rad_delta=-flares_rad_delta;


		// draw chinese character
		if (flares_image_flip!=0)
		{
			i=(int)(19.0*rand()/RAND_MAX);
			while (i==chinese_i)
				i=(int)(19.0*rand()/RAND_MAX);
			chinese_i=i;
			flares_image_flip=0;
		}

		SDL_BlitSurface(flares_chineseImages[chinese_i], NULL, screen, &chinese_rect);

		SDL_BlitSurface(flares_buffer, NULL, screen, NULL);

		// j0
		SDL_UpdateRect(screen,0,0,0,0);

		sdldemo_tickfps();

		SDL_PollEvent(&event);
	}
}

