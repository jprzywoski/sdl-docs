/* Modest Bricks by Greg Kennedy
 *      Feb. 2 2005
 *      Yet another Tetris clone.
 *
 *      I took the advice of a guide I saw and went all out on this one.  Scoring,
 *      high score, levels and speed increases, lookahead, custom sound effect and
 *      music, game options... the whole works. */
/* ************************************************************************ */
// INCLUDES
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>
#include <time.h>

/* ************************************************************************ */
// CONSTANTS
// Define the piece shapes
static const int base_piece_x[7][4] =
{
    { -1, 0, 1, 2 },                    // line
    { -1, 0, 1, 1 },                    // backwards L
    { -1, -1, 0, 1 },                   // L
    { 0, 1, 1, 0 },                     // square
    { -1, 0, 0, 1 },                    // S
    { -1, 0, 0, 1 },                    // Z
    { 0, -1, 0, 1 }                     // T
};
static const int base_piece_y[7][4] =
{
    { 0, 0, 0, 0 },                     // line
    { 0, 0, 0, -1 },                    // backwards L
    { -1, 0, 0, 0 },                    // L
    { 0, 0, -1, -1 },                   // square
    { 0, 0, -1, -1 },                   // S
    { -1, -1, 0, 0 },                   // Z
    { -1, 0, 0, 0 }                     // T
};

/* ************************************************************************ */
// GLOBALS
static SDL_Surface *screen;
static SDL_Surface *numbers;

/* ************************************************************************ */
// HELPER FUNCTIONS
// test if point is in rect
static int in(const int x, const int y, const int r_x, const int r_y, const int r_w, const int r_h)
{
    return x >= r_x && x < r_x + r_w && y >= r_y && y < r_y + r_h;
}

// SDL additions
// Function to load a BMP from a file, and convert it to the screen surface
static SDL_Surface *load_image(const char *path)
{
    SDL_Surface *img = SDL_LoadBMP(path);

    if (!img)
    {
        fprintf(stderr, "load_image: ERROR: Failed to load image '%s': %s\n", path, SDL_GetError());
        exit(1);
    }

    // convert loaded surface to our display format
    SDL_Surface *conv = SDL_DisplayFormat(img);

    if (!conv)
    {
        fprintf(stderr, "load_image: Warning: Failed to convert image '%s': %s\n", path, SDL_GetError());

        // the original may still be usable though
        return img;
    }
    // don't need img any more
    SDL_FreeSurface(img);

    return conv;
}

// draw surface on screen at x, y
static void blit(const int x, const int y, SDL_Surface *s)
{
    SDL_Rect rect;

    rect.x = x;
    rect.y = y;
    SDL_BlitSurface(s, NULL, screen, &rect);
}

// Draw a number (final digit positioned at x, y)
static void blit_number(const int x, const int y, int number)
{
    SDL_Rect src;
    SDL_Rect dst;

    src.x = src.y = 0;
    src.w = 16;
    src.h = 32;

    dst.x = x;
    dst.y = y;
    /* To show the value score, we start from the right and put numbers going
     *      left until we run out of numbers in the score. */
    do
    {
        src.x = (number % 10) * 16;
        SDL_BlitSurface(numbers, &src, screen, &dst);

        number /= 10;
        dst.x -= 17;
    }
    while (number > 0);
}

// Tetris Piece functions
// Draws a piece on the screen
static void draw_piece(const int x, const int y, const int piece_x[4], const int piece_y[4], SDL_Surface *img)
{
    for (int i = 0; i < 4; i++)
    {
        int px = x + 24 * piece_x[i];
        int py = y + 24 * piece_y[i];

        if (in(px, py, 0, 0, 380, 504))
        {
            blit(px, py, img);
        }
    }
}

// Test if the piece overlaps the board at this point
static int test_overlap(char board[20][10], const int x, const int y, const int piece_x[4], const int piece_y[4])
{
    for (int i = 0; i < 4; i++)
    {
        // test out-of-bounds move
        if (x + piece_x[i] < 0)
        {
            return 1;
        }
        if (x + piece_x[i] > 9)
        {
            return 1;
        }
        if (y + piece_y[i] > 19)
        {
            return 1;
        }
        // do not test off-screen collision
        if (y + piece_y[i] < 0)
        {
            continue;
        }
        // test overlap
        if (board[y + piece_y[i]][x + piece_x[i]])
        {
            return 1;
        }
    }
    return 0;
}

/* ************************************************************************ */
// MAIN ENTRY POINT
int main(int argc, char *argv[])
{
    // COMMON VARS
    // temporary buffer variable
    char tempstring[20];

    /* ******************************************************************** */
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    // register SDL_Quit to be called at termination
    atexit(SDL_Quit);

    // Load the image and then make that my window icon.
    SDL_WM_SetIcon(SDL_LoadBMP("tetimg/icon.bmp"), NULL);
    SDL_WM_SetCaption("Modest Bricks", NULL);

    // I want a 380x504 window, double buffered, any format (16/32 bit color)
    screen = SDL_SetVideoMode(380, 504, 0, SDL_ANYFORMAT | SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (screen == NULL)
    {
        fprintf(stderr, "Failed to open 380x504px window: %s\n", SDL_GetError());
        exit(1);
    }
    // Load all the BMP files we need
    // Numbers atlas
    numbers = load_image("tetimg/numbers.bmp");

    // Block images
    SDL_Surface *bimg[7] = { NULL };

    for (int i = 0; i < 7; i++)
    {
        sprintf(tempstring, "tetimg/%dbrick.bmp", i + 1);
        bimg[i] = load_image(tempstring);
    }

    // UI items
    SDL_Surface *title = load_image("tetimg/title.bmp");
    SDL_Surface *play = load_image("tetimg/play.bmp");
    SDL_Surface *musicon = load_image("tetimg/musicon.bmp");
    SDL_Surface *musicoff = load_image("tetimg/musicoff.bmp");
    SDL_Surface *sfxon = load_image("tetimg/sfxon.bmp");
    SDL_Surface *sfxoff = load_image("tetimg/sfxoff.bmp");
    SDL_Surface *quit = load_image("tetimg/quit.bmp");
    SDL_Surface *author = load_image("tetimg/author.bmp");
    SDL_Surface *level = load_image("tetimg/level.bmp");
    SDL_Surface *score = load_image("tetimg/score.bmp");
    SDL_Surface *high = load_image("tetimg/high.bmp");
    SDL_Surface *next = load_image("tetimg/next.bmp");
    SDL_Surface *line = load_image("tetimg/line.bmp");
    SDL_Surface *vline = load_image("tetimg/vline.bmp");
    SDL_Surface *lose = load_image("tetimg/lose.bmp");
    SDL_Surface *pause = load_image("tetimg/pause.bmp");

    // Solid black color
    const Uint32 black = SDL_MapRGB(screen->format, 0, 0, 0);

    /* ******************************************************************** */

    /* This is where we open up our audio device.  Mix_OpenAudio takes
     *      as its parameters the audio format we'd /like/ to have. */
    Mix_Chunk *chunk = NULL;
    Mix_Music *music = NULL;

    if (Mix_OpenAudio(22050, AUDIO_S16, 2, 2048))
    {
        // Failed to open the sound device
        fprintf(stderr, "Unable to open audio: %s\n", Mix_GetError());
    }
    else
    {
        /* If we actually care about what we got, we can ask here.  In this
         *      program we don't, but I'm showing the function call here anyway
         *      in case we'd want to know later. */
        // Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
        // This is loading a sound effect for a block landing.
        if (!(chunk = Mix_LoadWAV("fall.wav")))
        {
            fprintf(stderr, "Unable to load 'fall.wav' sample: %s\n", Mix_GetError());
        }
        // This is the music to play.
        if (!(music = Mix_LoadMUS("modest.mid")))
        {
            fprintf(stderr, "Unable to load 'modest.mid' music file: %s\n", Mix_GetError());
        }
    }

    /* ******************************************************************** */
    // Get high score data
    // default values
    int hiscore = 1;
    int music_on = 1;
    int sfx_on = 1;
    int game_level = 1;
    int game_score = 0;
    FILE *hifile = fopen("hiscore.dat", "r");

    if (hifile)
    {
        // read data from file
        fscanf(hifile, "%19s", tempstring);
        music_on = atoi(tempstring);
        fscanf(hifile, "%19s", tempstring);
        sfx_on = atoi(tempstring);
        fscanf(hifile, "%19s", tempstring);
        hiscore = atoi(tempstring);

        fclose(hifile);
    }
    if (music_on && music)
    {
        Mix_PlayMusic(music, -1);
    }
    /* ******************************************************************** */

    // seed the RNG
    srand((unsigned)time(NULL));

    // Gamestates we can be in:
    // * waiting at main menu
    // * playing a game
    // * quitting
    enum { state_menu, state_play, state_quit } state = state_menu;
    do
    {
        if (state == state_menu)
        {
            /* MAIN MENU */
            int dirty = 1;
            int mus_dirty = 1;
            int sfx_dirty = 1;

            while (state == state_menu)
            {
                // handle input first
                SDL_Event event;

                while (SDL_PollEvent(&event))
                {
                    switch (event.type)
                    {
                    case SDL_VIDEOEXPOSE:
                        dirty = 1;
                        break;
                    case SDL_MOUSEBUTTONUP:
                        // Mouse button pressed!
                        if (in(event.button.x, event.button.y, 60, 240, 120, 64))
                        {
                            // if they clicked in the "play" rectangle
                            state = state_play;
                        }
                        else if (in(event.button.x, event.button.y, 60, 340, 120, 64))
                        {
                            // if they clicked in the "quit" rectangle
                            state = state_quit;
                        }
                        else if (in(event.button.x, event.button.y, 50, 185, 64, 32))
                        {
                            // if they clicked in the "music" rectangle
                            if (music_on)
                            {
                                music_on = 0;
                                if (Mix_PlayingMusic())
                                {
                                    Mix_HaltMusic();
                                }
                            }
                            else
                            {
                                music_on = 1;
                                if (music)
                                {
                                    Mix_PlayMusic(music, -1);
                                }
                            }
                            mus_dirty = 1;
                        }
                        else if (in(event.button.x, event.button.y, 140, 185, 64, 32))
                        {
                            // if they clicked in the "sfx" rectangle
                            sfx_on = !sfx_on;
                            sfx_dirty = 1;
                        }
                        break;
                    case SDL_KEYUP:
                        // Keypress: Enter starts a game.
                        if (event.key.keysym.sym == SDLK_RETURN)
                        {
                            state = state_play;
                        }
                        else if (event.key.keysym.sym == SDLK_ESCAPE)
                        {
                            state = state_quit;
                        }
                        break;
                    case SDL_QUIT:
                        // if they quit some other way
                        state = state_quit;
                    }
                }

                // here we draw the main menu
                int needs_flip = 0;

                if (dirty)
                {
                    /* basically the main menu works like this:  we'll fill the whole screen
                     *      in black, then open all the images one by one to make up the screen
                     *      including borders, title and buttons and checkboxes, etc.  Also we
                     *      draw the current high score on the right. */
                    SDL_FillRect(screen, NULL, black);

                    blit(0, 0, title);
                    blit(60, 240, play);
                    blit(60, 340, quit);
                    blit(35, 105, author);
                    blit(270, 20, level);
                    blit(270, 120, score);
                    blit(270, 220, high);
                    blit(270, 320, next);
                    blit(0, 480, line);
                    blit(240, 0, vline);

                    blit_number(350, 65, game_level);
                    blit_number(350, 165, game_score);
                    blit_number(350, 265, hiscore);

                    mus_dirty = 1;
                    sfx_dirty = 1;

                    needs_flip = 1;
                }
                if (mus_dirty)
                {
                    blit(50, 185, (music_on ? musicon : musicoff));
                    needs_flip = 1;
                }
                if (sfx_dirty)
                {
                    blit(140, 185, (sfx_on ? sfxon : sfxoff));
                    needs_flip = 1;
                }
                if (needs_flip)
                {
                    // Swap the buffers to show the screen.
                    SDL_Flip(screen);

                    dirty = 0;
                    mus_dirty = 0;
                    sfx_dirty = 0;
                }
                SDL_Delay(1);
            }
        }
        else if (state == state_play)
        {
            /* PLAY MODE */

            // Set the game to starting conditions
            // points etc
            int game_lines = 0;

            game_level = 1;
            game_score = 0;

            int gameover = 0;

            // keyboard states
            int paused = 0;
            int drop_held = 0;

            // board positions - a board is 20 lines high.
            char board[20][10] =
            {
                { 0 }
            };

            // x and y positions of the four squares making one piece
            int piece_x[4];
            int piece_y[4];

            // generate our two pieces, the current and the next
            int piece_cur = rand() % 7;

            for (int i = 0; i < 4; i++)
            {
                piece_x[i] = base_piece_x[piece_cur][i];
                piece_y[i] = base_piece_y[piece_cur][i];
            }

            int piece_next = rand() % 7;

            // current x/y position on the board
            int x = 4;
            int y = 0;

            // prev. x/y/piece positions
            int prev_x;
            int prev_y;
            int prev_piece_x[4];
            int prev_piece_y[4];

            // trigger redraw flags
            int dirty = 1;
            int piece_dirty = 1;
            int field_dirty = 1;
            int next_piece_dirty = 1;
            int score_dirty = 1;
            Uint32 clock = SDL_GetTicks();

            while (state == state_play)
            {
                // Collect input
                int left_push = 0;
                int right_push = 0;
                int rotate_push = 0;
                SDL_Event event;

                while (SDL_PollEvent(&event))
                {
                    switch (event.type)
                    {
                    case SDL_ACTIVEEVENT:
                        // Minimizing the window pauses the game
                        if (((event.active.state & SDL_APPACTIVE) || (event.active.state & SDL_APPINPUTFOCUS)) &&
                                event.active.gain == 0)
                        {
                            SDL_WM_SetCaption("Modest Bricks - PAUSED (SPACE resumes)", NULL);
                            paused = 1;
                            field_dirty = 1;
                        }
                        break;
                    case SDL_VIDEOEXPOSE:
                        // Window reveal triggers a full-screen redraw
                        dirty = 1;
                        break;
                    case SDL_KEYUP:
                        // Handle key release
                        if (event.key.keysym.sym == SDLK_DOWN)
                        {
                            drop_held = 0;
                        }
                        else if (event.key.keysym.sym == SDLK_ESCAPE)
                        {
                            state = state_menu;
                        }
                        break;
                    case SDL_KEYDOWN:
                        // Key push events
                        if (event.key.keysym.sym == SDLK_DOWN)
                        {
                            drop_held = 1;
                        }
                        else if (event.key.keysym.sym == SDLK_UP)
                        {
                            rotate_push = 1;
                        }
                        else if (event.key.keysym.sym == SDLK_LEFT)
                        {
                            left_push = 1;
                        }
                        else if (event.key.keysym.sym == SDLK_RIGHT)
                        {
                            right_push = 1;
                        }
                        else if (event.key.keysym.sym == SDLK_SPACE)
                        {
                            if (paused)
                            {
                                SDL_WM_SetCaption("Modest Bricks", NULL);
                                clock = SDL_GetTicks();
                                paused = 0;
                                field_dirty = 1;
                            }
                            else
                            {
                                SDL_WM_SetCaption("Modest Bricks - PAUSED (SPACE resumes)", NULL);
                                paused = 1;
                                field_dirty = 1;
                            }
                        }
                        break;
                    case SDL_QUIT:
                        // Application exit
                        state = state_quit;
                    }
                }
                // Apply input and do game updates
                prev_x = x;
                prev_y = y;
                for (int i = 0; i < 4; i++)
                {
                    prev_piece_x[i] = piece_x[i];
                    prev_piece_y[i] = piece_y[i];
                }
                if (!paused)
                {
                    if (left_push && (!test_overlap(board, x - 1, y, piece_x, piece_y)))
                    {
                        x--;
                        piece_dirty = 1;
                    }
                    if (right_push && (!test_overlap(board, x + 1, y, piece_x, piece_y)))
                    {
                        x++;
                        piece_dirty = 1;
                    }
                    if (rotate_push)
                    {
                        if (piece_cur != 3)
                        {
                            int new_piece_x[4];
                            int new_piece_y[4];

                            for (int i = 0; i < 4; i++)
                            {
                                new_piece_x[i] = -piece_y[i];
                                new_piece_y[i] = piece_x[i];
                            }
                            if (!test_overlap(board, x, y, new_piece_x, new_piece_y))
                            {
                                for (int i = 0; i < 4; i++)
                                {
                                    piece_x[i] = new_piece_x[i];
                                    piece_y[i] = new_piece_y[i];
                                }
                                piece_dirty = 1;
                            }
                        }
                    }

                    // Timer-based effects
                    Uint32 now = SDL_GetTicks();

                    if (now > clock + (drop_held ? 50 : (500 - game_level * 50)))
                    {
                        // reset the clock
                        clock = now;
                        if (!test_overlap(board, x, y + 1, piece_x, piece_y))
                        {
                            // room to fall another spot
                            y++;
                            piece_dirty = 1;
                        }
                        else
                        {
                            // piece cannot fall further
                            // lock piece into place
                            for (int i = 0; i < 4; i++)
                            {
                                int py = y + piece_y[i];

                                if (py >= 0)
                                {
                                    board[py][x + piece_x[i]] = piece_cur + 1;
                                }
                            }
                            // Play sound
                            if (sfx_on && chunk)
                            {
                                Mix_PlayChannel(-1, chunk, 0);
                            }
                            // mark playfield AND piece for redraw
                            field_dirty = 1;

                            // test completed lines
                            int tetri = 0;

                            // TODO: consider that we only need to test the lines
                            // around y, and not all 20...
                            for (int j = 0; j < 20; j++)
                            {
                                int i;

                                for (i = 0; i < 10; i++)
                                    if (board[j][i] == 0)
                                    {
                                        break;
                                    }
                                if (i == 10)
                                {
                                    tetri++;

                                    // TODO: The next block may be called 4x,
                                    // maybe count lines and do one move at end
                                    // shift everything down from the top
                                    memmove(&board[1], &board[0], 10 * j);
                                    // set top line to empty
                                    memset(&board[0], 0, 10);

                                    game_lines++;
                                    if (game_lines > 9)
                                    {
                                        // if they got 10 lines, level up!
                                        game_lines -= 10;
                                        if (game_level < 8)
                                        {
                                            game_level++;
                                        }
                                    }
                                    score_dirty = 1;
                                }
                            }
                            // Scoring depends on how many you get at a time
                            if (tetri == 1)
                            {
                                game_score++;
                            }
                            else if (tetri == 2)
                            {
                                game_score += 3;
                            }
                            else if (tetri == 3)
                            {
                                game_score += 6;
                            }
                            else if (tetri == 4)
                            {
                                game_score += 10;
                            }
                            if (game_score > hiscore)
                            {
                                hiscore = game_score;
                            }
                            // Switch to next piece and copy next piece info to current piece
                            x = 4;
                            y = 0;
                            piece_cur = piece_next;
                            for (int i = 0; i < 4; i++)
                            {
                                piece_x[i] = base_piece_x[piece_cur][i];
                                piece_y[i] = base_piece_y[piece_cur][i];
                            }
                            piece_next = rand() % 7;

                            next_piece_dirty = 1;
                            // gameover happens if the new piece has nowhere to go
                            if (test_overlap(board, x, y, piece_x, piece_y))
                            {
                                gameover = 1;
                                state = state_menu;
                            }
                        }
                    }
                }

                /* DRAW TIME */
                int needs_flip = 0;

                if (dirty)
                {
                    /* redraw entire screen */

                    // blank UI areas
                    SDL_Rect r;

                    r.x = 264;
                    r.y = 0;
                    r.w = 116;
                    r.h = 480;
                    SDL_FillRect(screen, &r, black);

                    // draw ui elements
                    blit(270, 20, level);
                    blit(270, 120, score);
                    blit(270, 220, high);
                    blit(270, 320, next);
                    blit(0, 480, line);
                    blit(240, 0, vline);

                    // set flags for other items to show
                    piece_dirty = 1;
                    field_dirty = 1;
                    next_piece_dirty = 1;
                    score_dirty = 1;

                    needs_flip = 1;
                }
                if (piece_dirty && !field_dirty)
                {
                    // erase the existing piece but only if we aren't also planning to erase the whole field
                    SDL_Rect r;

                    r.w = r.h = 24;
                    for (int i = 0; i < 4; i++)
                    {
                        r.x = 24 * (prev_x + prev_piece_x[i]);
                        r.y = 24 * (prev_y + prev_piece_y[i]);
                        if (r.y >= 0)
                        {
                            SDL_FillRect(screen, &r, black);
                        }
                    }
// needs_flip = 1;
                }
                if (field_dirty)
                {
                    /* redraw board */
                    SDL_Rect r;

                    r.x = r.y = 0;
                    r.w = 240;
                    r.h = 480;
                    SDL_FillRect(screen, &r, black);
                    // draw saved board
                    for (int j = 0; j < 20; j++)
                        for (int i = 0; i < 10; i++)
                            if (board[j][i])
                            {
                                blit(i * 24, j * 24, bimg[board[j][i] - 1]);
                            }
                    // always trigger piece redraw too
                    piece_dirty = 1;

// needs_flip = 1;
                }
                if (piece_dirty)
                {
                    // draw current piece
                    draw_piece(24 * x, 24 * y, piece_x, piece_y, bimg[piece_cur]);

                    needs_flip = 1;
                }
                if (field_dirty && paused)
                {
                    // game paused sign
                    blit(0, 128, pause);

                    needs_flip = 1;
                }
                if (next_piece_dirty)
                {
                    // clear the area where next piece appears
                    SDL_Rect r;

                    r.x = 274;
                    r.y = 366;
                    r.w = r.h = 96;

                    SDL_FillRect(screen, &r, black);

                    // draw next piece
                    draw_piece(298, 390, base_piece_x[piece_next], base_piece_y[piece_next], bimg[piece_next]);

                    needs_flip = 1;
                }
                if (score_dirty)
                {
                    // redraw the score, level, and high score fields
                    SDL_Rect r;

                    r.x = 264;
                    r.y = 65;
                    r.w = 116;
                    r.h = 32;
                    SDL_FillRect(screen, &r, black);
                    blit_number(350, 65, game_level);

                    r.y = 165;
                    SDL_FillRect(screen, &r, black);
                    blit_number(350, 165, game_score);

                    r.y = 265;
                    SDL_FillRect(screen, &r, black);
                    blit_number(350, 265, hiscore);

                    needs_flip = 1;
                }
                // gameover sign
                if (gameover)
                {
                    blit(0, 128, lose);

                    needs_flip = 1;
                }
                /* Swap the buffers to show the screen. */
                if (needs_flip)
                {
                    dirty = 0;

                    piece_dirty = 0;
                    field_dirty = 0;
                    next_piece_dirty = 0;
                    score_dirty = 0;

                    SDL_Flip(screen);
                }
                if (gameover)
                {
                    // one second on Game Over and return to main menu
                    SDL_Delay(1000);
                }
                else
                {
                    SDL_Delay(1);
                }
            }
        }
    }
    while (state != state_quit);
    // Save settings
    sprintf(tempstring, "%1d\n%1d\n%11d", music_on, sfx_on, hiscore);
    hifile = fopen("hiscore.dat", "w");
    if (hifile)
    {
        fputs(tempstring, hifile);
        fclose(hifile);
    }
    // Turn off the music and shut it all down
    Mix_FreeChunk(chunk);
    Mix_FreeMusic(music);
    Mix_CloseAudio();

    // You can free your surfaces here, except the main screen!
// for (int i = 0; i < 7; i++)
// SDL_FreeSurface(bimg[i]);
// ... etc ...

    // Let SDL do its quitting thing
    // (this is already set by atexit, above)
    // SDL_Quit();

    return 0;
}
