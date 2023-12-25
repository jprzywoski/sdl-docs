/*
 * Name: Eat
 * Author: Snoolas and the Homebrew Functions at GPWiki.
 * Description:
 * A simple example of a 2D game using SDL as a graphics library.
 * Move the mouth with the arrrow keys to catch the falling food.
 * Date: 2005
 */

#include <cstdlib>                      // For atexit() to destroy SDL properly when the application exits unexpectedly and the random number functions.
#include <stdio.h>                      // Used for reporting SDL errors with printf.
#include <ctime>                        // Used to get time to seed srand()
#include <SDL/SDL.h>                    // SDL Includes-If your directory paths are different, then change this accordingly.
#include "SDL/SDL_ttf.h"

#define SCREEN_WIDTH	640             // Defines that make it easy for us to change the screen size without even changing the functions.
#define SCREEN_HEIGHT	480
#define COLORKEY	255, 0, 255     // The colorkey that will not be included in sprites when they are blitted onto the screen.
#define endl		"\n"            // Used later for making line breaks when reporting errors.

// ------------------------
// BEGIN PROGRAM DECLARATIONS
// Declarations for functions to be used later.
bool initSDL();
void mainloop();
void exitvoid();
void render();
void drawimage(SDL_Surface *srcimg, int sx, int sy, int sw, int sh, SDL_Surface *dstimg, int dx, int dy, int alpha);

SDL_Surface *loadimage(char *file);

int random(int lowest_number, int highest_number);
void updatescore();

// END PROGRAM DECLARATIONS
// ------------------------
// BEGIN GAME DECLARATIONS

bool boundsmode;                        // A boolean representing whether the game is in boundary mode, where it won't let the ball go past the outside of the screen. Toggle with "b" key.
bool rubbermode;                        // The boolean representing whether it is in rubber mode, where keyups don't make the velocity 0.
unsigned int
gamespeed;                 // Used later in the timing of the main loop. Defaults at 50, change up with "," down with "."

/*
 * lollipop = 1,
 * cheese = 2,
 * apple =3,
 * cookie =4,
 * cake =5,
 */
struct foodobj                          // The struct that represents one food object that is falling from the ceiling.
{
    int xpos;                           // The x axis location of the upper left of the food item.
    int ypos;                           // The same as above, but y axis.
    char type;                          // The type of food item that the food object is.
    char velocity;                      // The velocity of the falling of the food object, represented in pixels per frame.
    bool inited;                        // A boolean representing whether the food object has been initialized.
};

// An array of pointers to 6 food objects.
foodobj *fallobj = NULL;
signed int catcherxpos;                 // Integer indicating the location of the catcher mouth x axis.
signed int catcherypos;                 // Same as above, but on the y axis.
signed int catchxvel;                   // The velocity, in pixels per frame, of the x axis of the catcher.
signed int catchyvel;                   // Same as above, but for the y axis.
int score;                              // The player's score.
Uint32 looptime;                        // A variable used in the timing for the main loop that holds the time of the last completion of the main loop.

// END GAME DECLARATIONS
// -------------------------
// BEGIN SDL DECLARATIONS
SDL_Surface *screen;                    // Pointer referencing the main SDL surface that we blit to.
SDL_Surface *bg;                        // Pointer to the Background we'll blit to the back of every frame. 640w x 480h
SDL_Surface
*food;                      // Surface containing all of the food objects in the order that they appear in the enum. 32h x (32*5)w
SDL_Surface *mouth;                     // Surface containing the mouth that will eat the food. 64w x 64h
TTF_Font *font;                         // Pointer to the font we will be using to render the score.
SDL_Surface *scoretext;                 // Surface containing the text rendered by SDL_ttf that has the score.

// END SDL DECLARATIONS
// -------------------------

int main(int argc, char *argv[])
{
    fallobj = new foodobj[5];           // Create the array of food objects.
    gamespeed = 50;                     // Default the game speed at 50 milliseconds.
    srand(static_cast<unsigned>(time(
                                    0))); // Seed the number generator with the time so that the numbers will be different every time the game is run.
    // printf("Like, time(0) is, uhhhh: %u", static_cast<unsigned>(time(0)));
    if (initSDL() ==
            false)             // If Initializing SDL didn't work for some reason, then don't go into the mainloop and just exit.
    {
        mainloop();
    }
    return 0;
}

bool initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) !=
            0)          // If SDL Video Initialization fails, then return true to main() so it won't go on with mainloop() and report the error for debugging.
    {
        printf("Unable to initialize SDL: %s", SDL_GetError(), endl);
        return true;
    }
    if (TTF_Init() == -1)   // Initialize SDL_ttf and if it fails, report the error.
    {
        printf("Unable to initialize SDL_ttf: ", TTF_GetError(), endl);
        return true;
    }
    atexit(exitvoid);                   // Clean up SDL when the application quits so we don't leave all sorts of junk in memory.

    // Load Caliban.ttf at size 16 into font.
    font = TTF_OpenFont("Caliban.ttf", 14);
    if (font == NULL)
    {
        printf("TTF_OpenFont: ", TTF_GetError(), endl);
        // Handle error by telling main() not to go into mainloop().
        return true;
    }
    // Attempt to set up the screen surface. On failure, return true to main() and report the error for debugging.
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, SDL_DOUBLEBUF);
    if (screen == NULL)
    {
        printf("Unable to set video mode: ", SDL_GetError(), endl);
        return true;
    }
    bg = loadimage("eat_art/bg.bmp");           // Call our image loading function and load the background.
    if (bg == NULL)
    {
        return true;
    }
    food = loadimage("eat_art/food.bmp");       // Call our image loading function and load the food tileset.
    if (food == NULL)
    {
        return true;
    }
    mouth = loadimage("eat_art/mouth.bmp");     // Call our image loading function and load the mouth tile.
    if (mouth == NULL)
    {
        return true;
    }
    SDL_ShowCursor(SDL_DISABLE);                // Make sure we don't have the cursor sitting in the middle of the game.
    return false;
}

SDL_Surface *loadimage(char *file)      // Our function to load BMP's into surfaces.
{
    SDL_Surface *tmp;

    tmp = SDL_LoadBMP(file);            // Load the requested BMP into the temporary surface.
    if (tmp ==
            NULL)                    // If the loading failed, then report the error and return null so the code that called this can deal with the error.
    {
        printf("Error: File could not be opened:", file, SDL_GetError(), endl);
        return NULL;
    }
    else                                // Set the colorkey for the surface we are loading so we can have transparent pixels where we want to.
    {
        if (SDL_SetColorKey(tmp, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(tmp->format, COLORKEY)) == -1)
        {
            printf("Warning: Colorkey will not be used, reason: \n", SDL_GetError(), endl);
        }
    }
    return tmp;
}

void newfood(char
             index)   // A void that takes the index of a food obj in the array and (re)creates it, randomizing everything.
{
    fallobj[index].type = (char)random(1, 5);           // Set the type randomly.
    fallobj[index].velocity =
        fallobj[index].type;      // Set the velocity accordingly to the type. Cakes are harder to catch than lollipops, and so give more points.
    fallobj[index].xpos = random(1, SCREEN_WIDTH -
                                 32); // Set the x location randomly, making sure that it isn't off of the picture.
    fallobj[index].ypos =
        0;                            // Set the y location to 0, so that it can start from the top and give the player a fair chance of catching it.
    fallobj[index].inited =
        true;                       // Set whether the object has been initialized, so that the main loop can know whether to use it.
}

void mainloop()                                         // The void that enters into the main game loop that actually runs the game.
{
    bool brunning;                                      // The boolean value representing whether to continue with the main loop.

    brunning =
        true;                                    // When it is set to true the loop continues. When false, it stops and passes control back to main to quit.
    catcherxpos = SCREEN_WIDTH / 2;                     // Place the catcher mouth in the middle of the screen.
    catcherypos = SCREEN_HEIGHT / 2;
    boundsmode = true;
    score = 0;                                          // Initialize score before its first rendering.
    updatescore();                                      // Render score to get ready for render().

    bool upup =
        true;                                   // A set of booleans used in the input loop so the keydown case knows whether the key has been up or not
    bool downup = true;
    bool leftup = true;
    bool rightup = true;
    SDL_Event keyevent;                 // The event that we will poll to get the users keystrokes.

    for (char n = 0; n <= 5; n++)
    {
        newfood(n);
    }
    while (brunning)
    {
        while (SDL_PollEvent(&keyevent))   // Poll our SDL key event for any keystrokes.
        {
            switch (keyevent.type)
            {
            case SDL_QUIT:              // If SDL is quiting, exit the mainloop.
                brunning = false;
                break;
            case SDL_KEYDOWN:
                switch (keyevent.key.keysym.sym)
                {
                case SDLK_ESCAPE:       // If the user presses the escape button, exit the loop.
                    brunning = false;
                    break;
                case SDLK_b:            // If the user presses b then toggle boundsmode.
                    if (boundsmode)
                    {
                        boundsmode = false;
                    }
                    else
                    {
                        boundsmode = true;
                    }
                    updatescore();      // So we can see whether boundsmode is on.
                    break;
                case SDLK_r:            // If the user presses r then toggle rubbermode.
                    if (rubbermode)
                    {
                        rubbermode = false;
                    }
                    else
                    {
                        rubbermode = true;
                    }
                    updatescore();      // So we can see it toggle.
                    break;
                case SDLK_COMMA:
                    gamespeed += 10;    // Change the gamespeed.
                    updatescore();      // So we see the new speed.
                    break;
                case SDLK_PERIOD:
                    if ((gamespeed - 10) >= 10)
                    {
                        gamespeed -= 10;        // Change the gamespeed if we aren't going below 0.
                        updatescore();          // So we see the new speed.
                    }
                    break;
                case SDLK_h:
                    catcherxpos = SCREEN_WIDTH / 2;     // Place the catcher mouth in the middle of the screen.
                    catcherypos = SCREEN_HEIGHT / 2;
                    break;
                case SDLK_UP:
                    catchyvel +=
                        -1; // When the user presses the arrow keys, adjust the velocities of the catcher mouth accordingly and set the keyup boolean so that we can continue to move the mouth until there is a keyup event.
                    upup = false;
                    break;
                case SDLK_DOWN:
                    catchyvel += 1;
                    downup = false;
                    break;
                case SDLK_RIGHT:
                    catchxvel += 1;
                    rightup = false;
                    break;
                case SDLK_LEFT:
                    catchxvel += -1;
                    leftup = false;
                    break;
                default:
                    break;
                }
                break;
            case SDL_KEYUP:
                switch (keyevent.key.keysym.sym)
                {
                case SDLK_UP:
                    if (!rubbermode)
                    {
                        catchyvel =
                            0;    // When the keys are let up, set the velocities accordingly and change the keyup booleans so that it will know that the keys aren't down anymore.
                    }
                    upup = true;
                    break;
                case SDLK_DOWN:
                    if (!rubbermode)
                    {
                        catchyvel = 0;
                    }
                    downup = true;
                    break;
                case SDLK_RIGHT:
                    if (!rubbermode)
                    {
                        catchxvel = 0;
                    }
                    rightup = true;
                    break;
                case SDLK_LEFT:
                    if (!rubbermode)
                    {
                        catchxvel = 0;
                    }
                    leftup = true;
                    break;
                default:
                    break;
                }
            }
        }
        if (!upup)
        {
            catchyvel -= 1;    // These continue to accelerate the mouth if the user is holding down a button.
        }
        if (!downup)
        {
            catchyvel +=
                1;    // They have to use these booleans and be out here because no matter how long you hold down the key, it only generates two events: down and up.
        }
        if (!rightup)
        {
            catchxvel += 1;
        }
        if (!leftup)
        {
            catchxvel -= 1;
        }
        if (boundsmode)
        {
            // These lines first make sure that a movement isn't out of the screen, then make the movement.
            // If it is trying to be outside the screen, it rejects it by making the velocity 0.
            if (catcherxpos + catchxvel <= (SCREEN_WIDTH - 64) && catcherxpos + catchxvel >= 0)
            {
                catcherxpos += catchxvel;
            }
            else
            {
                catchxvel = 0;
            }
            if (catcherypos + catchyvel <= (SCREEN_HEIGHT - 64) && catcherypos + catchyvel >= 0)
            {
                catcherypos += catchyvel;
            }
            else
            {
                catchyvel = 0;
            }
        }
        else
        {
            catcherxpos += catchxvel;   // If the user doesn't want to use the boundary along the outside, this code runs.
            catcherypos += catchyvel;
        }
        for (char x = 0; x <= 5; x++)   // Loop through all the food objects and do things to them.
        {
            if (fallobj[x].inited == false)
            {
                newfood(x);                                     // If it has been eaten, then reinitialize it.
            }
            else
            {
                if (fallobj[x].ypos + fallobj[x].velocity <=
                        SCREEN_HEIGHT)   // If the result of a drop down doesn't put the food below the screen.
                {
                    fallobj[x].ypos += fallobj[x].velocity;     // Make the food object fall down the screen at its individual velocity.
                    if (((fallobj[x].xpos >= catcherxpos) && (fallobj[x].xpos <= catcherxpos + 64)) &&
                            ((fallobj[x].ypos >= catcherypos) && (fallobj[x].ypos <= catcherypos + 64)))
                    {
                        fallobj[x].inited = false;              // If the object is inside of the mouth, then un init it.
                        score += fallobj[x].type;               // Update the score based upon the worth of the object.
                        updatescore();                          // Update the score surface so that when the scene is rendered, we will see the new score.
                    }
                }
                else
                {
                    fallobj[x].inited =
                        false;                  // If the result of a drop down puts the food below the screen, then uninit the food so it will be reinitialized on the next iteration of the loop.
                }
            }
        }
        render();                                       // Render all these things onto the screen surface.
        SDL_Flip(screen);                               // Flip screen to the video memory so we can see what we just drew.
        while (SDL_GetTicks() - looptime < gamespeed)   // Algorithm to keep roughly the same game speed on all systems.

        {
            SDL_Delay(10);    // So we don't hog the CPU cycles.
        }
        looptime =
            SDL_GetTicks();                      // Store time of completion of main loop so the next iteration can so this timing thing.
    }
}

void render()
{
    drawimage(bg, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, screen, 0, 0,
              255);        // Draw the background onto the screen before anything else.
    for (char x = 0; x <= 5; x++)
    {
        drawimage(food, fallobj[x].type * 32 - 32, 0, 32, 32, screen, fallobj[x].xpos, fallobj[x].ypos,
                  255);    // For every food item, draw it on the screen.
    }
    drawimage(mouth, 0, 0, 64, 64, screen, catcherxpos, catcherypos,
              255);      // Draw the catcher at the proper location on the screen.
    drawimage(scoretext, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 6, screen, 0, 0, 255); // Draw the score on the screen.
}

void updatescore()
{
    SDL_Color fontcolor = { 0, 0, 0, 255 };     // The color that the text will be rendered in by SDL_ttf.
    char fonttext[800];                         // The text that is rendered by SDL_ttf. Hopefully 80 characters are enough ;).
    char rubbertext[6];                         // The text arrays that hold a true or false about rubber and bounds modes.
    char boundstext[6];

    if (boundsmode)
    {
        sprintf(boundstext, "True");
    }
    else
    {
        sprintf(boundstext, "False");    // Make the strings with thrue and false for the modes.
    }
    if (rubbermode)
    {
        sprintf(rubbertext, "True");
    }
    else
    {
        sprintf(rubbertext, "False");
    }
    sprintf(fonttext, "\"Eat\" by Snoolas: Score: %d, Speed: %d, Bounds: %s, Rubber: %s", score, gamespeed, boundstext,
            rubbertext);                        // The text is put together in fontext by sprintf, then rendered.
    scoretext = TTF_RenderText_Solid(font, fonttext, fontcolor);
}

void drawimage(SDL_Surface *srcimg, int sx, int sy, int sw, int sh, SDL_Surface *dstimg, int dx, int dy, int alpha =
                   255)
{
    if ((!srcimg) || (alpha == 0))
    {
        return;    // If there's no image, or its 100% transparent.
    }

    SDL_Rect src, dst;                  // The two rectangles are filled with the information passed to the function.

    src.x = sx;
    src.y = sy;
    src.w = sw;
    src.h = sh;
    dst.x = dx;
    dst.y = dy;
    dst.w = src.w;
    dst.h = src.h;
    if (alpha != 255)
    {
        SDL_SetAlpha(srcimg, SDL_SRCALPHA,
                     alpha);    // Make SDL aware of the desired level of Alpha transparency in the source image.
    }
    SDL_BlitSurface(srcimg, &src, dstimg, &dst);        // Finally Blit the source on to the destination surface.
}

void exitvoid()                                         // Void to be called by atexit() that properly deinitializes SDL and SDL_ttf.
{
    // Delete all the SDL_Surface's so we aren't in danger of memory leaks.
    SDL_FreeSurface(screen);
    SDL_FreeSurface(bg);
    SDL_FreeSurface(mouth);
    SDL_FreeSurface(food);
    SDL_FreeSurface(scoretext);
    TTF_CloseFont(font); // Delete the font.
    delete[] fallobj;                   // Clean up our array of food objects at the end of the program.
    SDL_Quit();
    TTF_Quit();
}

int random(int lowest_number, int
           highest_number)       // This function returns a pseudo-random number between the parameters.
{
    // int range = highest_number - lowest_number + 1;
    // return lowest_number + int(range * rand()/(RAND_MAX + 1.0));
    return (rand() % highest_number) + lowest_number;
}
