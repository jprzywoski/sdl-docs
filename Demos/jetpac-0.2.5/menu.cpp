/*menu.cpp
 * Implements the menu screens at the start of the game.
 *
 * Copyright (C) 2001  James Blewitt
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "jetpac.h"
#include <iostream>

using namespace std;

extern SDL_Joystick *Joystick;
extern unsigned int Initial_Stage;
extern SDL_Surface *LoadBMP(char *file);
extern void Draw(unsigned int Xpos, unsigned int Ypos, unsigned int Width, unsigned int Height, SDL_Surface *image,
                 SDL_Surface *screen);
extern void Erase(unsigned int Xpos, unsigned int Ypos, unsigned int Width, unsigned int Height, SDL_Surface *screen,
                  SDL_Surface *background);
extern void Update(unsigned int Xpos, unsigned int Ypos, unsigned int Width, unsigned int Height, SDL_Surface *screen);
extern void Quit();
char *Get_Keyboard(Uint8 *keys)
{
    if (keys[SDLK_a])
    {
        return "a";
    }
    if (keys[SDLK_b])
    {
        return "b";
    }
    if (keys[SDLK_c])
    {
        return "c";
    }
    if (keys[SDLK_d])
    {
        return "d";
    }
    if (keys[SDLK_e])
    {
        return "e";
    }
    if (keys[SDLK_f])
    {
        return "f";
    }
    if (keys[SDLK_g])
    {
        return "g";
    }
    if (keys[SDLK_h])
    {
        return "h";
    }
    if (keys[SDLK_i])
    {
        return "i";
    }
    if (keys[SDLK_j])
    {
        return "j";
    }
    if (keys[SDLK_k])
    {
        return "k";
    }
    if (keys[SDLK_l])
    {
        return "l";
    }
    if (keys[SDLK_m])
    {
        return "m";
    }
    if (keys[SDLK_n])
    {
        return "n";
    }
    if (keys[SDLK_o])
    {
        return "o";
    }
    if (keys[SDLK_p])
    {
        return "p";
    }
    if (keys[SDLK_q])
    {
        return "q";
    }
    if (keys[SDLK_r])
    {
        return "r";
    }
    if (keys[SDLK_s])
    {
        return "s";
    }
    if (keys[SDLK_t])
    {
        return "t";
    }
    if (keys[SDLK_u])
    {
        return "u";
    }
    if (keys[SDLK_v])
    {
        return "v";
    }
    if (keys[SDLK_w])
    {
        return "w";
    }
    if (keys[SDLK_x])
    {
        return "x";
    }
    if (keys[SDLK_y])
    {
        return "y";
    }
    if (keys[SDLK_z])
    {
        return "z";
    }
    if (keys[SDLK_RETURN])
    {
        return "\r";
    }
    if (keys[SDLK_BACKSPACE])
    {
        return "\b";
    }
    return NULL;
}

void Menu_Screen(SDL_Surface *screen)
{
// Implements the menu interface.
    unsigned int state = 0;
    unsigned int choice = 0;
    unsigned int max_choice = 3;
    unsigned int Pointer_Ypos = 200;
    unsigned int Code_Match = 0;
    unsigned int i = 0;
    char *key_store;
    unsigned char CurrentCodePos = 0;
    char LevelCodeStore[5];
    char Level2Code[] = { 'r', 'a', 'd', 'a', 'r' };
    char Level3Code[] = { 'a', 's', 't', 'r', 'o' };
    char Level4Code[] = { 'o', 'r', 'b', 'i', 't' };
    char Level5Code[] = { 'l', 'u', 'n', 'a', 'r' };
    SDL_Surface *Menu_Background = LoadBMP("pixmaps/background.bmp");
    SDL_Surface *Instructions = LoadBMP("pixmaps/instructions.bmp");
    SDL_Surface *Start_Button = LoadBMP("pixmaps/startgamebutton.bmp");
    SDL_Surface *Instructions_Button = LoadBMP("pixmaps/instructbutton.bmp");
    SDL_Surface *Load_Button = LoadBMP("pixmaps/loadlevelbutton.bmp");
    SDL_Surface *Quit_Button = LoadBMP("pixmaps/quitbutton.bmp");
    SDL_Surface *Key_Icon = LoadBMP("pixmaps/keyicon.bmp");
    SDL_Surface *Load_Message = LoadBMP("pixmaps/loadmessage.bmp");

    SDL_SetColorKey(Load_Message, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(Load_Message->format, 255, 255, 255));

    SDL_Surface *Pointer = LoadBMP("pixmaps/jetmanb.bmp");

    SDL_SetColorKey(Pointer, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(Pointer->format, 255, 255, 255));

    SDL_Event event;
    Uint8 *keys;                        // Saves the keys.

    while (1)
    {
// Draw the screen.
        switch (state)
        {
        case 0:
            Draw(0, 0, Menu_Background->w, Menu_Background->h, Menu_Background, screen);
            Draw(245, 200, Start_Button->w, Start_Button->h, Start_Button, screen);
            Draw(245, 260, Instructions_Button->w, Instructions_Button->h, Instructions_Button, screen);
            Draw(245, 320, Load_Button->w, Load_Button->h, Load_Button, screen);
            Draw(245, 380, Quit_Button->w, Quit_Button->h, Quit_Button, screen);
            Draw(180, Pointer_Ypos, Pointer->w, Pointer->h, Pointer, screen);
            break;
        case 1:
            Draw(0, 0, Instructions->w, Instructions->h, Instructions, screen);
            break;
        case 2:
            Draw(0, 0, Menu_Background->w, Menu_Background->h, Menu_Background, screen);
            Draw(100, 250, Load_Message->w, Load_Message->h, Load_Message, screen);
            break;
        }
        switch (state)
        {
        case 2:                         // Level code input screen
            while (state == 2)
            {
                SDL_WaitEvent(&event);
                keys = SDL_GetKeyState(NULL);

                key_store = Get_Keyboard(keys);
                if (key_store != NULL && key_store != "\r" && key_store != "\b")
                {
                    if (CurrentCodePos < 5)
                    {
                        LevelCodeStore[CurrentCodePos] = *key_store;
                        Draw(250 + (50 * CurrentCodePos), 250, Key_Icon->w, Key_Icon->h, Key_Icon, screen);
                        CurrentCodePos++;
                    }
                }
                if (key_store == "\b")
                {
                    if (CurrentCodePos != 0)
                    {
                        CurrentCodePos--;
                    }
                    Erase(250 + (50 * CurrentCodePos), 250, Key_Icon->w, Key_Icon->h, screen, Menu_Background);
                    Update(250 + (50 * CurrentCodePos), 250, Key_Icon->w, Key_Icon->h, screen);
                }
                if (key_store == "\r")
                {
                    Code_Match = 2;
                    for (i = 0; i < 5; i++)
                        if (LevelCodeStore[i] != Level2Code[i])
                        {
                            Code_Match = 0;
                        }
                    if (Code_Match == 2)
                    {
                        Initial_Stage = 6;
                        return;
                    }
                    Code_Match = 3;
                    for (i = 0; i < 5; i++)
                        if (LevelCodeStore[i] != Level3Code[i])
                        {
                            Code_Match = 0;
                        }
                    if (Code_Match == 3)
                    {
                        Initial_Stage = 4;
                        return;
                    }
                    // No correct code found, exit to main menu!
                    state = 0;
                    CurrentCodePos = 0;
                }
                if (keys[SDLK_SPACE] || keys[SDLK_TAB] || keys[SDLK_ESCAPE])
                {
                    state = 0;
                    CurrentCodePos = 0;
                }
            }
            break;

        case 1:                         // Info page
            while (state == 1)
            {
                SDL_WaitEvent(&event);
                keys = SDL_GetKeyState(NULL);
                if (keys[SDLK_SPACE] || SDL_JoystickGetButton(Joystick, 0) || keys[SDLK_RETURN] || keys[SDLK_TAB] ||
                        keys[SDLK_ESCAPE])
                {
                    state = 0;
                }
            }
            break;

        case 0:                         // Main menu screen
            while (state == 0)
            {
                SDL_WaitEvent(&event);
                keys = SDL_GetKeyState(NULL);
                switch (choice)
                {
                case 0:
                    while (Pointer_Ypos != 200)
                    {
                        SDL_Delay(8);   // Use timer to slow things down a little.
                        if (Pointer_Ypos > 200)
                        {
                            Pointer_Ypos--;
                        }
                        if (Pointer_Ypos < 200)
                        {
                            Pointer_Ypos++;
                        }
                        Erase(180, Pointer_Ypos, Pointer->w, Pointer->h, screen, Menu_Background);
                        Draw(180, Pointer_Ypos, Pointer->w, Pointer->h, Pointer, screen);
                    }
                    break;
                case 1:
                    while (Pointer_Ypos != 260)
                    {
                        SDL_Delay(8);   // Use timer to slow things down a little.
                        if (Pointer_Ypos > 260)
                        {
                            Pointer_Ypos--;
                        }
                        if (Pointer_Ypos < 260)
                        {
                            Pointer_Ypos++;
                        }
                        Erase(180, Pointer_Ypos, Pointer->w, Pointer->h, screen, Menu_Background);
                        Draw(180, Pointer_Ypos, Pointer->w, Pointer->h, Pointer, screen);
                    }
                    break;
                case 2:
                    while (Pointer_Ypos != 320)
                    {
                        SDL_Delay(8);   // Use timer to slow things down a little.
                        if (Pointer_Ypos > 320)
                        {
                            Pointer_Ypos--;
                        }
                        if (Pointer_Ypos < 320)
                        {
                            Pointer_Ypos++;
                        }
                        Erase(180, Pointer_Ypos, Pointer->w, Pointer->h, screen, Menu_Background);
                        Draw(180, Pointer_Ypos, Pointer->w, Pointer->h, Pointer, screen);
                    }
                    break;
                case 3:
                    while (Pointer_Ypos != 380)
                    {
                        SDL_Delay(8);
                        if (Pointer_Ypos > 380)
                        {
                            Pointer_Ypos--;
                        }
                        if (Pointer_Ypos < 380)
                        {
                            Pointer_Ypos++;
                        }
                        Erase(180, Pointer_Ypos, Pointer->w, Pointer->h, screen, Menu_Background);
                        Draw(180, Pointer_Ypos, Pointer->w, Pointer->h, Pointer, screen);
                    }
                    break;
                }
                Draw(180, Pointer_Ypos, Pointer->w, Pointer->h, Pointer, screen);
                if (keys[SDLK_UP] || SDL_JoystickGetAxis(Joystick, 1) < -3000)
                    if (choice > 0)
                    {
                        choice--;
                    }
                if (keys[SDLK_DOWN] || SDL_JoystickGetAxis(Joystick, 1) > 3000)
                    if (choice < max_choice)
                    {
                        choice++;
                    }
                if (keys[SDLK_SPACE] || keys[SDLK_RETURN] || keys[SDLK_TAB] || SDL_JoystickGetButton(Joystick, 0))
                {
                    switch (choice)
                    {
                    case 0:             // Start new game pressed
                        SDL_FreeSurface(Menu_Background);
                        SDL_FreeSurface(Instructions);
                        SDL_FreeSurface(Instructions_Button);
                        SDL_FreeSurface(Start_Button);
                        SDL_FreeSurface(Quit_Button);
                        SDL_FreeSurface(Key_Icon);
                        SDL_FreeSurface(Load_Message);
                        SDL_FreeSurface(Pointer);
                        Initial_Stage = 1;
                        return;         // return and continue with the game.
                        break;

                    case 1:
                        state = 1;
                        break;

                    case 2:
                        state = 2;
                        break;

                    case 3:
                        SDL_FreeSurface(Menu_Background);
                        SDL_FreeSurface(Instructions);
                        SDL_FreeSurface(Instructions_Button);
                        SDL_FreeSurface(Start_Button);
                        SDL_FreeSurface(Quit_Button);
                        SDL_FreeSurface(Key_Icon);
                        SDL_FreeSurface(Load_Message);
                        SDL_FreeSurface(Pointer);
                        // only run the quit procedure if Initial level is set
                        // otherwise will get seg fault
                        if (Initial_Stage != 0)
                        {
                            Quit();    // else just exit game.
                        }
                        cout << "\nGuess this just ain't your kind of game eh?" << endl;
                        exit(2);
                        break;
                    }
                }
            }
            break;
        }
    }
}
