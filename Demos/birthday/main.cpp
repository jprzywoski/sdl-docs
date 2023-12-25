/*
 *
 * - Birthday Intro -		v0.0.1
 *
 * by Torsten Giebl ( wizard@turricane.org )
 *
 * for the Birthday of Dominik Schmidt
 * one of the BIGGEST AMIGA FREAX and a cool
 * TRACKER MASTER. He loves IT ( Intelligent Tracking :=)
 *
 */
// All needed Includes
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <stdio.h>

// Some helper ENUM :=)

enum
{
    ASCII_SPACE = 32, ASCII_DOT = 46, ASCII_KOMMA = 44
};

enum
{
    START_X = 105, START_Y = 396,       // START_Y == END_Y
    END_X = 524, END_Y = 396
};

enum
{
    LETTER_KOMMA = 26, LETTER_DOT, LETTER_SPACE
};

enum
{
    LETTER_A = 0, LETTER_B, LETTER_C, LETTER_D, LETTER_E, LETTER_F, LETTER_G, LETTER_H, LETTER_I, LETTER_J, LETTER_K,
    LETTER_L, LETTER_M, LETTER_N, LETTER_O, LETTER_P, LETTER_Q, LETTER_R, LETTER_S, LETTER_T, LETTER_U, LETTER_V,
    LETTER_W, LETTER_X, LETTER_Y, LETTER_Z
};

// All needed Surfaces

SDL_Surface *screen = NULL;
SDL_Surface *picture = NULL;
SDL_Surface *letters[29];
SDL_Surface *t_left = NULL;
SDL_Surface *t_right = NULL;

// Music Stuff
Mix_Music *music = NULL;
int audio_rate = 44100;
int audio_channels = 2;
int audio_buffer = 4096;
Uint16 audio_format = AUDIO_S16;

// Scrolltext
char *Scrolltext =
    "BITTE DEN SCROLLTEXT VOLLKOMMEN LESEN.  HAPPY BIRTHDAY TO YOU, HAPPY BIRTHDAY TO YOU. ICH HOFFE BEI DIR LAEUFT DER SCROLLTEXT RUCKELFREI. NE SCHOENERE MUSIK HAB ICH LEIDER NICHT GEFUNDEN. DIE SCHRIFT IST UEBRIGENS DIE AUS DEM TURRICAN II INTRO. WENN ICH ETWAS KOMISCH SCHREIBE IST ES WEIL ICH KANN NICHT DEN BUCHSTABEN NACH I UND NICHT DEN NACH Y VERWENDEN, DIE HAB ICH IM T II INTRO NICHT GEFUNDEN SCHREIB MIR MAL NE MAIL WIE ES LIEF. HAB NUN AUCH IM INTERNET EINEN T II HD INSTALLER MIT GAME GEFUNDEN, ALLERDINGS FUNKTIONIERT DER NICHT VOLLKOMMEN.   CU    DU KANNST NUN ESCAPE DRUECKEN. DU KANNST NUN ESCAPE DRUECKEN. DU KANNST NUN ESCAPE DRUECKEN.";
char *current_letter = NULL;

// Event Mem
SDL_Event event;

// done flag
bool done = false;

// Endtime
Uint32 endtime = 0;

// Scrolltext Index
Sint32 Index = 0;
Sint32 temp_index = 0;

// Scrolltext Difference
int Diff_Index = 0;

// Letter Mem
SDL_Rect letter_rect;
SDL_Rect t_left_rect;
SDL_Rect t_right_rect;

// Load and Transform the Bitmap
SDL_Surface *LaT_Bitmap(char *Filename)
{
    SDL_Surface *temp = NULL;
    SDL_Surface *ready = NULL;

    temp = SDL_LoadBMP(Filename);
    ready = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);

    return ready;
}

void Load_Bitmaps()
{
    picture = LaT_Bitmap("./data/main.bmp");

    letters[LETTER_A] = LaT_Bitmap("./data/letter_a.bmp");
    letters[LETTER_B] = LaT_Bitmap("./data/letter_b.bmp");
    letters[LETTER_C] = LaT_Bitmap("./data/letter_c.bmp");
    letters[LETTER_D] = LaT_Bitmap("./data/letter_d.bmp");
    letters[LETTER_E] = LaT_Bitmap("./data/letter_e.bmp");
    letters[LETTER_F] = LaT_Bitmap("./data/letter_f.bmp");
    letters[LETTER_G] = LaT_Bitmap("./data/letter_g.bmp");
    letters[LETTER_H] = LaT_Bitmap("./data/letter_h.bmp");
    letters[LETTER_I] = LaT_Bitmap("./data/letter_i.bmp");
    // letters [LETTER_J] = LaT_Bitmap ("./data/letter_j.bmp");
    letters[LETTER_K] = LaT_Bitmap("./data/letter_k.bmp");
    letters[LETTER_L] = LaT_Bitmap("./data/letter_l.bmp");
    letters[LETTER_M] = LaT_Bitmap("./data/letter_m.bmp");
    letters[LETTER_N] = LaT_Bitmap("./data/letter_n.bmp");
    letters[LETTER_O] = LaT_Bitmap("./data/letter_o.bmp");
    letters[LETTER_P] = LaT_Bitmap("./data/letter_p.bmp");
    letters[LETTER_Q] = LaT_Bitmap("./data/letter_q.bmp");
    letters[LETTER_R] = LaT_Bitmap("./data/letter_r.bmp");
    letters[LETTER_S] = LaT_Bitmap("./data/letter_s.bmp");
    letters[LETTER_T] = LaT_Bitmap("./data/letter_t.bmp");
    letters[LETTER_U] = LaT_Bitmap("./data/letter_u.bmp");
    letters[LETTER_V] = LaT_Bitmap("./data/letter_v.bmp");
    letters[LETTER_W] = LaT_Bitmap("./data/letter_w.bmp");
    letters[LETTER_X] = LaT_Bitmap("./data/letter_x.bmp");
    letters[LETTER_Y] = LaT_Bitmap("./data/letter_y.bmp");
    // letters [LETTER_Z] = LaT_Bitmap ("./data/letter_z.bmp");
    letters[LETTER_KOMMA] = LaT_Bitmap("./data/letter_komma.bmp");
    letters[LETTER_DOT] = LaT_Bitmap("./data/letter_dot.bmp");
    letters[LETTER_SPACE] = LaT_Bitmap("./data/letter_space.bmp");

    t_left = LaT_Bitmap("./data/t_left.bmp");
    t_right = LaT_Bitmap("./data/t_right.bmp");
}

void Kill_Surfaces()
{
    Uint8 temp = 0;

    for (temp = 0; temp < 29; temp++)
    {
        if (letters[temp] != NULL)
        {
            SDL_FreeSurface(letters[temp]);
        }
    }
    SDL_FreeSurface(picture);
    SDL_FreeSurface(t_left);
    SDL_FreeSurface(t_right);
}

void Load_Music()
{
    if (music == NULL)
    {
        music = Mix_LoadMUS("./data/birthday.mod");
        Mix_PlayMusic(music, -1);
    }
}

void Kill_Music()
{
    if (music != NULL)
    {
        Mix_HaltMusic();
        Mix_FreeMusic(music);
    }
}

// The main Programm

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    screen = SDL_SetVideoMode(640, 480, 32, SDL_HWPALETTE | SDL_HWSURFACE | SDL_DOUBLEBUF);

    Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffer);

    SDL_Delay(2000);

    Load_Bitmaps();

    t_left_rect.x = 0;
    t_left_rect.y = 480 - t_left->h;
    t_left_rect.w = t_left->w;
    t_left_rect.h = t_left->h;

    t_right_rect.x = 640 - t_right->w;
    t_right_rect.y = 480 - t_right->h;
    t_right_rect.w = t_right->w;
    t_right_rect.h = t_right->h;

    Index = END_X + 16;

    letter_rect.x = Index;
    letter_rect.y = START_Y;
    letter_rect.w = 16;
    letter_rect.h = 16;

    Load_Music();
    while (!done)
    {
        SDL_BlitSurface(picture, NULL, screen, NULL);

        Index--;

        temp_index = Index;
        for (current_letter = Scrolltext; *current_letter != NULL; current_letter++)
        {
            letter_rect.x = temp_index;
            letter_rect.y = START_Y;
            letter_rect.w = 16;
            letter_rect.h = 16;
            switch ((unsigned int)*current_letter)
            {
            case 65:
            case 66:
            case 67:
            case 68:
            case 69:
            case 70:
            case 71:
            case 72:
            case 73:
            case 74:
            case 75:
            case 76:
            case 77:
            case 78:
            case 79:
            case 80:
            case 81:
            case 82:
            case 83:
            case 84:
            case 85:
            case 86:
            case 87:
            case 88:
            case 89:
            case 90:
                if ((temp_index > -16) && (temp_index < 640))
                {
                    SDL_BlitSurface(letters[((unsigned int)*current_letter) - 65], NULL, screen, &letter_rect);
                }
                break;

            case ASCII_DOT:
                if ((temp_index > -16) && (temp_index < 640))
                {
                    SDL_BlitSurface(letters[LETTER_DOT], NULL, screen, &letter_rect);
                }
                break;

            case ASCII_KOMMA:
                if ((temp_index > -16) && (temp_index < 640))
                {
                    SDL_BlitSurface(letters[LETTER_KOMMA], NULL, screen, &letter_rect);
                }
                break;

            case ASCII_SPACE:
                if ((temp_index > -16) && (temp_index < 640))
                {
                    SDL_BlitSurface(letters[LETTER_SPACE], NULL, screen, &letter_rect);
                }
                break;

            default:

                break;
            }
            temp_index += 16;
        }
        SDL_BlitSurface(t_left, NULL, screen, &t_left_rect);
        SDL_BlitSurface(t_right, NULL, screen, &t_right_rect);
        SDL_Flip(screen);
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:       // Pressing
                    done = true;        // ESC
                    break;

                default:
                    break;
                }
                break;

            case SDL_QUIT:              // Closing Window
                done = true;
                break;

            default:
                break;
            }
            endtime = SDL_GetTicks() + 10;
            SDL_Delay(10);
            while (endtime > SDL_GetTicks())
                ;
        }
    }
    Kill_Surfaces();
    Kill_Music();

    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}
