/* This is a the controlling file for Paddles, a simple pong clone.*/
#include "use.hpp"
#define GAMESPEED	20
#define PlayerPaddleY	(SCREEN_HEIGHT - (SCREEN_HEIGHT / 10))
#define CompPaddleY	(SCREEN_HEIGHT / 10)

void loadmedia();
void mainloop();
void GetInput();
void Think();
void Draw();
void exitvoid();
void NewBall(bool players);

bool brunning;
bool gameovermode;
int PlayerPaddleX;
int PlayerPaddleVel;
int CompPaddleX;
int CompPaddleVel;
int BallX;
int BallY;
int BallXvel;
int BallYvel;
unsigned int PlayerScore;
unsigned int CompScore;
bool rightdown;
bool leftdown;
bool comprightdown;
bool compleftdown;
int widthoftext;
int heightoftext;
SDL_Surface *ball;
SDL_Surface *paddle;
SDL_Surface *gamedialog;
TTF_Font *font;

int main(int argc, char *argv[])
{
    /*Initialize the USE codebase*/
    initUSE();

    /*When the application exits, make sure everything is shutdown.*/
    atexit(exitvoid);

    /*Load our media, like images and fonts*/
    loadmedia();

    /*Set some variables so our mainloop does the right thing*/
    brunning = true;
    gameovermode = true;

    /*Enter the main loop, and when we return, exit*/
    mainloop();
    return 0;
}

void loadmedia()
{
    /*Load our media, like images and fonts*/
    ball = loadimage("paddlesart/ball.bmp");
    paddle = loadimage("paddlesart/paddle.bmp");
    font = loadfont("FreeSans.ttf", POINTSIZE);
}

void mainloop()
{
    /*This void determines whether the size of the text for the game over dialog is too large for the screen
     * and if all goes well it prints it to the screen and waits for the user's input to start a new game. */
    if (TTF_SizeText(font, "GAME OVER. Press N for new game.", &widthoftext, &heightoftext) == 0)
    {
        if (widthoftext >= SCREEN_WIDTH)
        {
            printf("Text is too wide to fit in screen");
            exit(1);
        }
    }
    else
    {
        printf("Error determining size of text: %s", TTF_GetError());
        exit(1);
    }
    /*Give the ball an initial position and velocity.*/
    NewBall(false);
    /*Start on our main loop*/
    while (brunning)
    {
        if (!gameovermode)
        {
            int looptime;

            GetInput();
            Think();
            Draw();
            /*Delay the game according to the gamespeed constant so we don't just eat up all the CPU.*/
            while (SDL_GetTicks() - looptime < GAMESPEED)
            {
                SDL_Delay(10);
            }
            looptime = SDL_GetTicks();
        }
        else
        {
            int looptime;

            /*Draw our dialog text to the gamedialog surface so it can be drawn.*/
            gamedialog = drawtext(font, 255, 255, 255, 255, 0, 0, 0, 255, "GAME OVER. Press N for new game.", blended);
            /*Check to see if anything went wrong in rendering the text.*/
            if (gamedialog == 0)
            {
                printf("Unable to render text");
                exit(1);
            }
            SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));
            drawimage(gamedialog, 0, 0, widthoftext, heightoftext, screen, 25, 25, 255);
            SDL_FreeSurface(gamedialog);
            SDL_Flip(screen);

            SDL_Event dialevent;

            while (SDL_PollEvent(&dialevent))
            {
                if (dialevent.type == SDL_KEYDOWN && dialevent.key.keysym.sym == SDLK_n)
                {
                    /*If n is pressed for new game, make the next iteration of the mainloop go into the REAL main loop.*/
                    gameovermode = false;

                    /*Change the scores to zero so we won't end up with it thinking that last round's scores are this round's.*/
                    PlayerScore = 0;
                    CompScore = 0;

                    /*Give the ball an initial position and velocity.*/
                    NewBall(false);
                }
                /*Check if the user is trying to escape with the escape key.*/
                if (dialevent.type == SDL_KEYDOWN && dialevent.key.keysym.sym == SDLK_ESCAPE)
                {
                    brunning = false;
                }
                /*If SDL is quitting, quit the mainloop.*/
                if (dialevent.type == SDL_QUIT)
                {
                    brunning = false;
                }
            }
            /*Use the same algorithm as above to delay execution and not hog CPU cycles.*/
            while (SDL_GetTicks() - looptime < GAMESPEED)
            {
                SDL_Delay(10);
            }
            looptime = SDL_GetTicks();
        }
    }
}

void GetInput()
{
    /*Quite obviously, an input loop. :)*/
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            brunning = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                brunning = false;
                break;
            case SDLK_RIGHT:
                rightdown = true;
                break;
            case SDLK_LEFT:
                leftdown = true;
                break;
            default:
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym)
            {
            case SDLK_RIGHT:
                rightdown = false;
                break;
            case SDLK_LEFT:
                leftdown = false;
                break;
            default:
                break;
            }
            break;
        }
    }
}

void NewBall(bool players)
{
    /*This void takes a boolean representing whether the player has scored. If false it means the computer has just scored. It then updates the gamedialog surface
     * for the current score and places the ball randomly and sets its initial velocities according to who just scored.*/

    /*Write our score string to the buffer.*/
    char scoretext[30];

    sprintf(scoretext, "Player: %u, Computer: %u", PlayerScore, CompScore);
    /*Make sure the text isn't too big for the screen and that that doesn't fail.*/
    if (TTF_SizeText(font, "GAME OVER. Press N for new game.", &widthoftext, &heightoftext) == 0)
    {
        if (widthoftext >= SCREEN_WIDTH)
        {
            printf("Text is too wide to fit in screen");
            exit(1);
        }
    }
    else
    {
        printf("Error determining size of text: %s", TTF_GetError());
        exit(1);
    }
    /*Render our score text buffer to the gamedialog surface.*/
    gamedialog = drawtext(font, 0, 0, 0, 255, 255, 255, 255, 255, scoretext, blended);
    /*Check to see if anything went wrong in rendering the text.*/
    if (gamedialog == 0)
    {
        printf("Unable to render text");
        exit(1);
    }
    /*Check to see if either player or computer has won and if so enter game over mode so the next iteration of the mainlopp will be drawing the game over message on the screen.*/
    if (PlayerScore >= 11 || CompScore >= 11)
    {
        gameovermode = true;
    }
    /*Randomly but fairly position the ball on the screen.*/
    BallX = random(0, SCREEN_WIDTH);
    BallY = random(CompPaddleY + 100, PlayerPaddleY - 100);
    /*Set the ball's velocity according to who just scored.*/
    if (players)
    {
        BallXvel = random(-5, 5);
        BallYvel = random(-5, -2);
    }
    else
    {
        BallXvel = random(-5, 5);
        BallYvel = random(2, 5);
    }
}

void Think()
{
    /*The computer paddle's AI to make it follow the ball. It is pretty rough and unnatural looking, but it seems to be quite effective in stopping the ball.*/
    if (BallYvel < 0)
    {
        if (CompPaddleX + 12 > BallX - 10)
        {
            compleftdown = true;
            comprightdown = false;
        }
        if (CompPaddleX + 12 < BallX - 10)
        {
            comprightdown = true;
            compleftdown = false;
        }
    }
    else
    {
        compleftdown = false;
        comprightdown = false;
    }
    /*Move the computer's paddle according to how the AI has its buttons pushed.*/
    if (comprightdown)
    {
        CompPaddleVel += 1;
    }
    if (compleftdown)
    {
        CompPaddleVel -= 1;
    }
    if (!comprightdown && CompPaddleVel > 0)
    {
        CompPaddleVel -= 1;
    }
    if (!compleftdown && CompPaddleVel < 0)
    {
        CompPaddleVel += 1;
    }
    /*Move the Player's paddle according to how they have their buttons pushed.*/
    if (rightdown)
    {
        PlayerPaddleVel += 1;
    }
    if (leftdown)
    {
        PlayerPaddleVel -= 1;
    }
    if (!rightdown && PlayerPaddleVel > 0)
    {
        PlayerPaddleVel -= 1;
    }
    if (!leftdown && PlayerPaddleVel < 0)
    {
        PlayerPaddleVel += 1;
    }
    /*Make sure the player's paddle velocity and position wouldn't put the paddle through the wall and if that goes well, move the paddle, if not,
     * bounce the paddle off of the wall.*/
    if ((PlayerPaddleX + PlayerPaddleVel <= SCREEN_WIDTH - 64) && (PlayerPaddleX + PlayerPaddleVel >= 0))
    {
        PlayerPaddleX += PlayerPaddleVel;
    }
    else
    {
        PlayerPaddleVel = (PlayerPaddleVel * -1) / 2;
    }
    /*Just like above except for the computer's paddle*/
    if ((CompPaddleX + CompPaddleVel <= SCREEN_WIDTH - 64) && (CompPaddleX + CompPaddleVel >= 0))
    {
        CompPaddleX += CompPaddleVel;
    }
    else
    {
        CompPaddleVel = (CompPaddleVel * -1) / 2;
    }
    /*Check if the ball is intersecting the player's paddle and if so, adjust the ball's velocities according to how it was moving in relation to the paddle.*/
    if ((BallX >= PlayerPaddleX && BallX + 4 <= PlayerPaddleX + 64) && (BallY >= PlayerPaddleY && BallY <=
            PlayerPaddleY + 4))
    {
        if (PlayerPaddleVel == 0)
        {
            BallYvel *= -1;
        }
        else
        {
            if ((PlayerPaddleVel < 0 && BallXvel < 0) || (PlayerPaddleVel < 0 && BallXvel < 0))
            {
                BallYvel *= -1;
                BallXvel *= 2;
            }
            else
            {
                BallYvel = (BallYvel * -1) + (PlayerPaddleVel / 2);
            }
        }
    }
    /*The same thing as above, but for the computer's paddle.*/
    if ((BallX >= CompPaddleX && BallX + 4 <= CompPaddleX + 64) && (BallY >= CompPaddleY && BallY <= CompPaddleY + 4))
    {
        if (CompPaddleVel == 0)
        {
            BallYvel *= -1;
        }
        else
        {
            if ((CompPaddleVel < 0 && BallXvel < 0) || (CompPaddleVel < 0 && BallXvel < 0))
            {
                BallYvel *= -1;
                BallXvel *= 2;
            }
            else
            {
                BallYvel = (BallYvel * -1) + (CompPaddleVel / 2);
            }
        }
    }
    /*First check if the ball location and ball velocity suggest that the ball needs to be bounced and if so, do it, but if not, continue moving the ball according to the velocity.*/
    if ((BallX + BallXvel) > 0 && (BallX + BallXvel) <= SCREEN_WIDTH)
    {
        BallX += BallXvel;
    }
    else
    {
        BallXvel *= -1;
    }
    /*This is like the section above, but it only checks to see if it is going offscreen and if not it updates the ball's location, because this section operates with a dimension
     * that shouldn't bounce the ball because if the ball is there, it means either the player or the computer has scored.*/
    if ((BallY + BallYvel) > 0 && (BallY + BallYvel) <= SCREEN_HEIGHT)
    {
        BallY += BallYvel;
    }
    /*If the ball is moving offscreen in certain dimensions, it means that a player has scored, so update the scores and reposition the ball for more play.*/
    if (BallY + BallYvel <= 0)
    {
        PlayerScore += 1;
        NewBall(false);
    }
    if (BallY + BallYvel >= SCREEN_HEIGHT)
    {
        CompScore += 1;
        NewBall(true);
    }
}

void Draw()
{
    /*First fill the screen with white so our black images will show up and then draw our images to the screen surface in the correct locations and finally flip the
     * screen surface to the actual screen.*/
    SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 255, 255, 255));
    drawimage(gamedialog, 0, 0, widthoftext, heightoftext, screen, 0, 0, 255);
    drawimage(paddle, 0, 0, 64, 4, screen, PlayerPaddleX, PlayerPaddleY, 255);
    drawimage(paddle, 0, 0, 64, 4, screen, CompPaddleX, CompPaddleY, 255);
    drawimage(ball, 0, 0, 4, 4, screen, BallX, BallY, 255);
    SDL_Flip(screen);
}

void exitvoid()
{
    /*Destroy our libraries and media so we don't leave bunches of junk in memory.*/
    SDL_FreeSurface(screen);
    SDL_FreeSurface(paddle);
    SDL_FreeSurface(ball);
    SDL_FreeSurface(gamedialog);

    TTF_CloseFont(font);

    TTF_Quit();
    SDL_Quit();
}
