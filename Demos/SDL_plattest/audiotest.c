/*
 * audiotest.c written by Tyler Montbriand, 2004.
 * music_game.wav borrowed from open-source game "Chromium BSU",
 * sound is apparently in public domain
 */

#define WAV_FILE "music_game.wav"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>

Uint8 *audio_buf = NULL;
Uint32 wav_bytes = 0;
int running;

void audio_callback(void *udata, Uint8 *stream, int len)
{
    static int pos = 0;

    if ((pos + len) > wav_bytes)
    {
        int maxlen = wav_bytes - (pos + len);

        running = 0;
        if (maxlen > 0)
        {
            memcpy(stream, audio_buf + pos, maxlen);
            pos += maxlen;
        }
        fprintf(stderr, "!\n");
    }
    else
    {
        memcpy(stream, audio_buf + pos, len);
        pos += len;
        fprintf(stderr, ".");
    }
}

int main(int argc, char *argv[])
{
    SDL_AudioSpec awant, aget, wavefmt;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Couldn't init audio: %s\n", SDL_GetError());
        return 1;
    }
    if (getenv("SDL_AUDIO_DRIVER") != NULL)
    {
        if (SDL_AudioInit(getenv("SDL_AUDIO_DRIVER")) < 0)
        {
            fprintf(stderr, "Couldn't init audio driver \"%s\"\n", getenv("SDL_AUDIO_DRIVER"));
            SDL_Quit();
            return 4;
        }
    }
    else
    {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
        {
            fprintf(stderr, "Couldn't init audio: %s\n", SDL_GetError());
            return 5;
        }
    }
    SDL_SetVideoMode(128, 128, 32, SDL_ANYFORMAT);      /* Necessary for windows alone */

    {
        char namebuf[512];

        if (SDL_AudioDriverName(namebuf, 512) == NULL)
        {
            fprintf(stderr, "No audio driver\n");
        }
        else
        {
            fprintf(stderr, "Audio driver: %s\n", namebuf);
        }
    }

    awant.freq = 22050;
    awant.format = AUDIO_S16SYS;
    awant.channels = 1;
    awant.samples = 4096;
    awant.callback = audio_callback;
    awant.userdata = &aget;
    if (SDL_OpenAudio(&awant, &aget) != 0)
    {
        fprintf(stderr, "Couldn't open audio device: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    fprintf(stderr, "Audio initialized at %d hz\n", aget.freq);
    if (SDL_LoadWAV(WAV_FILE, &wavefmt, &audio_buf, &wav_bytes) == NULL)
    {
        fprintf(stderr, "Could not load WAV file: %s", SDL_GetError());
        SDL_CloseAudio();
        SDL_Quit();
        return 1;
    }
    fprintf(stderr, "Loaded %s\n", WAV_FILE);
    fprintf(stderr, "%d bytes going into conversion...\n", wav_bytes);
    {
        SDL_AudioCVT cvt;
        int retval;

        fprintf(stderr, "SDL_BuiltAudioCVT(%p,0x%08x,%d,%d,0x%08x,%d,%d)=", &cvt, wavefmt.format, wavefmt.channels,
                wavefmt.freq, aget.format, aget.channels, aget.freq);

        retval = SDL_BuildAudioCVT(&cvt, wavefmt.format, wavefmt.channels, wavefmt.freq, aget.format, aget.channels,
                                   aget.freq);
        fprintf(stderr, "%d\n", retval);
        if (retval < 0)
        {
            fprintf(stderr, "Couldn't create audio converter: %s\n", SDL_GetError());
            free(audio_buf);
            SDL_CloseAudio();
            SDL_Quit();
            return 2;
        }
        {
            /* Cannot use realloc, since windows mungs malloc() up for you */
            Uint8 *nbuf = (Uint8 *)malloc(wav_bytes * cvt.len_mult);

            if (nbuf == NULL)
            {
                fprintf(stderr, "Couldn't create buffer\n");
                free(audio_buf);
                SDL_CloseAudio();
                SDL_Quit();
                return 5;
            }
            memcpy(nbuf, audio_buf, wav_bytes);
            SDL_FreeWAV(audio_buf);
            audio_buf = nbuf;
        }

        cvt.len = wav_bytes;
        cvt.buf = audio_buf;
        wav_bytes = cvt.len * cvt.len_mult;
        SDL_ConvertAudio(&cvt);
    }
    fprintf(stderr, "%d bytes coming out of conversion.\n", wav_bytes);

    fprintf(stderr, "Playback test:\n");

    running = 1;
    SDL_PauseAudio(0);
    while (running)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
            ;                           /* Ignore all events */
        SDL_Delay(10);
    }
    SDL_PauseAudio(1);

    fprintf(stderr, "Playback test done\n");

    SDL_CloseAudio();
    SDL_Quit();
    if (audio_buf != NULL)
    {
        free(audio_buf);
    }
    return 0;
}
