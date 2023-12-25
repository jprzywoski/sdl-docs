/*
 * music module for SDLInvaders
 * Copyright (C) 2002 Dave Watson
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 */
#include <sys/stat.h>
#include <string.h>

#ifndef NOSOUND
#include <SDL/SDL_mixer.h>

Mix_Chunk *smp_explosion1 = NULL;
Mix_Chunk *smp_explosion2 = NULL;
Mix_Chunk *smp_invader_sound1 = NULL;

/*
 * Mix_Chunk *smp_invader_sound2 = NULL;
 */
Mix_Chunk *smp_shot1 = NULL;

/*
 * Mix_Chunk *smp_shot2 = NULL;
 */
Mix_Music *background_music = NULL;
int audio_used = 0;

#endif

void free_background_music(void)
{
#ifndef NOSOUND
    if (background_music)
    {
        Mix_FreeMusic(background_music);
        background_music = NULL;
    }
#endif
}

void set_background_volume(int percent)
{
#ifndef NOSOUND
    if (audio_used)
    {
        Mix_VolumeMusic((int)(SDL_MIX_MAXVOLUME * ((float)percent / 100)));
    }

#endif
}

void stop_background_music(int time)
{
#ifndef NOSOUND
    if (audio_used)
    {
        Mix_FadeOutMusic(time);
    }

#endif
}

/* load the file filename to play as background music */
int play_background_music(char *filename)
{
#ifndef NOSOUND
    /*  struct stat buf;     will use it later (Niels)*/
    char full_filename[256] = "";

    if (!audio_used)
    {
        return 0;
    }
    free_background_music();

    /* for later use, when switching to autoconf (Niels)
     * if (!stat("../data",&buf))
     * strcat(full_filename,"../data/music/");
     * else if(!stat(DATA_DIR,&buf)) {
     * strcat(full_filename,DATA_DIR);
     * strcat(full_filename,"music/");
     * }
     */

    strcat(full_filename, filename);

    background_music = Mix_LoadMUS(full_filename);
    if (!background_music)
    {
        printf("Mix_LoadMUS(\"%s\"): %s\n", full_filename, Mix_GetError());
        return 1;
    }
    if (Mix_FadeInMusic(background_music, -1, 3000) == -1)
    {
        printf("Mix_FadeInMusic: %s\n", Mix_GetError());
        /* well, there's no music, but most games don't break without music */
    }
#endif
    return 0;
}

#ifdef NOSOUND
void free_sample(char *sample)
{
#else
void free_sample(Mix_Chunk *sample)
{
    if (sample)
    {
        Mix_FreeChunk(sample);
        sample = NULL;
    }
#endif
}

void music_cleanup(void)
{
#ifndef NOSOUND
    free_background_music();

    free_sample(smp_explosion1);
    free_sample(smp_explosion2);
    free_sample(smp_invader_sound1);

    /*
     * free_sample(smp_invader_sound2);
     */
    free_sample(smp_shot1);

    /*
     * free_sample(smp_shot2);
     */
    if (audio_used)
    {
        Mix_CloseAudio();
        audio_used = 0;
    }
#endif
}

#ifdef NOSOUND
void play_sample(char *sample)
{
#else
void play_sample(Mix_Chunk *sample)
{
    if (sample && audio_used)
    {
        Mix_PlayChannel(-1, sample, 0);
    }

#endif
}

#ifdef NOSOUND
void load_sample(char **sample, char *filename)
{
#else
void load_sample(Mix_Chunk **sample, char *filename)
{
    /*  struct stat buf;*/
    char full_filename[256] = "";

    if (!audio_used)
    {
        return;
    }
    free_sample(*sample);

    /* autoconf (Niels)
     * if (!stat("../data",&buf))
     * strcat(full_filename,"../data/sounds/");
     * else if(!stat(DATA_DIR,&buf)) {
     * strcat(full_filename,DATA_DIR);
     * strcat(full_filename,"sounds/");
     * }
     */

    strcat(full_filename, filename);

    *sample = Mix_LoadWAV(full_filename);
    if (!sample)
    {
        printf("Mix_LoadWAV: %s\n", Mix_GetError());
        return;
    }
    Mix_VolumeChunk(*sample, MIX_MAX_VOLUME / 2);
#endif
}

int music_init(void)
{
#ifndef NOSOUND
    /* Open mixer device */
    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
    {
        printf("Mix_OpenAudio: %s\n", Mix_GetError());
        printf("Try running without sound.\n");
        return 0;
    }
    audio_used = 1;

    Mix_AllocateChannels(16);

    load_sample(&smp_explosion1, "data/explosion.wav");
    load_sample(&smp_explosion2, "data/bombexplosion.wav");
    load_sample(&smp_invader_sound1, "data/alienshot.wav");

    /*
     * load_sample(&smp_invader_sound2,"invader_sound2.wav");
     */
    load_sample(&smp_shot1, "data/shot.wav");

    /*
     * load_sample(&smp_shot2,"shot2.wav");
     */
#endif
    return 0;
}
