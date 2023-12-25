/**
 * SDL OpenGL Tutorial.
 * (c) Michael Vance, 2000
 * briareos@lokigames.com
 *
 * Modified by Tyler Montbriand, 2005
 *
 * Distributed under terms of the LGPL.
 *
 * original source at http://sdldoc.csn.ul.ie/guidevideoopengl.php
 *
 * About this program:
 *   When running, it will show a spinning cube.  Hit the spacebar
 *   to start and stop the movement of the cube, or ESC to exit.
 */
#include <stdio.h>
#include <SDL/SDL.h>
/* Includes gl.h and glu.h, as well as things like windows.h if needed */
#include <SDL/SDL_opengl.h>

/* Handles events -- mouse, keyboard, etc.  A return value of 0
 * means the program should stop running.
 */
int process_events(void);

/* draws a spinning cube on the screen.  speed is rotations per second. */
void draw_screen(float speed);

/* Sets up the OpenGL viewport */
void setup_opengl(int width, int height);

/* Returns the number of milliseconds since this was last called */
int stopwatch();

int should_rotate = 1;                  /* set to 0 to stop cube spinning */

/* The operating system calls this to run the program */
int main(int argc, char *argv[])        /* Dimensions of our window. */
{
    int width = 640, height = 480;

    /* Flags we pass into SDL_SetVideoMode. We want an OpenGL window,
     * but we'll let SDL pick the best value of bits-per-pixel.
     * Forcing SDL to use a different BPP than your desktop will
     * slow it down.
     */
    int flags = SDL_OPENGL | SDL_ANYFORMAT;

    /* Initialize SDL
     * http://www.libsdl.org/cgi/docwiki.cgi/SDL_5fInit
     */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)   /* Failed, exit. */
    {
        fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    /*
     * Now, we want to setup our requested
     * window attributes for our OpenGL window.
     * We want *at least* 5 bits of red, green
     * and blue. We also want at least a 16-bit
     * depth buffer.
     *
     * http://www.libsdl.org/cgi/docwiki.cgi/SDL_5fGL_5fSetAttribute
     */
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

    /*
     * If the window isn't fullscreen, chances are we
     * won't get a real doublebuffer, just an emulated one.
     * That's slow, so not good.
     */
    if (flags & SDL_FULLSCREEN)         /*
	                                 * Request a double buffered window. '1' turns on double
	                                 * buffering, '0' turns it off.
	                                 *
	                                 * Note that we do not use SDL_DOUBLEBUF in
	                                 * the flags to SDL_SetVideoMode. That does
	                                 * not affect the GL attribute state, only
	                                 * the standard 2D blitting setup.
	                                 */

    {
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    }
    /* Set the video mode, or create a window
     * http://www.libsdl.org/cgi/docwiki.cgi/SDL_5fSetVideoMode
     */
    if (SDL_SetVideoMode(width, height, 32, flags) == NULL)
    {
        fprintf(stderr, "Video mode set failed: %s\n", SDL_GetError());
        SDL_Quit();                     /* Quit SDL.  Can't forget that! */
        return 1;                       /* Code 1 means error. */
    }

    /* At this point, we should have a properly setup
     * double-buffered window for use with OpenGL.
     */
    setup_opengl(width, height);

    /*
     * Now we want to begin our normal app process--
     * an event loop with a lot of redrawing.
     */
    while (process_events())            /* Loop while we haven't been told to quit */
    {
        /* 16RPM, like old records */
        draw_screen(16.0f / 60.0f);     /* Draw the screen. */
    }
    SDL_Quit();                         /* Close the window */
    return 0;                           /* normal exit */
}

int process_events()
{
    /* Events get put in here
     * http://www.libsdl.org/cgi/docwiki.cgi/SDL_5fEvent
     */
    SDL_Event event;

    /* Grab all the events off the queue.
     * http://www.libsdl.org/cgi/docwiki.cgi/SDL_5fPollEvent
     */
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            /* Handle key presses. */
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                return 0;               /* Stop running immediately */
            case SDLK_SPACE:
                /* Start or stop rotation */
                should_rotate = !should_rotate;
                break;
            default:                    /* Ignore any other keys */
                break;
            }
            break;
        case SDL_QUIT:
            /* Handle quit requests (like Ctrl-c, or close-window */
            return 0;                   /* Stop running immediately */
        default:                        /* anything we didn't handle already */
            break;
        }
    }

    /* A tiny delay, to be polite to the operating system
     * http://www.libsdl.org/cgi/docwiki.cgi/SDL_5fDelay
     */
    SDL_Delay(10);
    return 1;                           /* 1 means keep going */
}

/* Arrays for OpenGL colors.  Global because blue's ALWAYS blue. */
const GLubyte red[] = { 255, 0, 0, 255 };
const GLubyte green[] = { 0, 255, 0, 255 };
const GLubyte blue[] = { 0, 0, 255, 255 };
const GLubyte white[] = { 255, 255, 255, 255 };
const GLubyte yellow[] = { 0, 255, 255, 255 };
const GLubyte black[] = { 0, 0, 0, 255 };
const GLubyte orange[] = { 255, 255, 0, 255 };
const GLubyte purple[] = { 255, 0, 255, 0 };

void draw_screen(float speed)
{
    static float angle = 0.0f;          /* Our angle of rotation. */
    /* All 8 corners of the cube */
    static GLfloat v0[] = { -1.0f, -1.0f, 1.0f };
    static GLfloat v1[] = { 1.0f, -1.0f, 1.0f };
    static GLfloat v2[] = { 1.0f, 1.0f, 1.0f };
    static GLfloat v3[] = { -1.0f, 1.0f, 1.0f };
    static GLfloat v4[] = { -1.0f, -1.0f, -1.0f };
    static GLfloat v5[] = { 1.0f, -1.0f, -1.0f };
    static GLfloat v6[] = { 1.0f, 1.0f, -1.0f };
    static GLfloat v7[] = { -1.0f, 1.0f, -1.0f };
    int delay = stopwatch();            /* Measures time since last redraw. */

    /*
     * EXERCISE:
     * Replace this awful mess with vertex
     * arrays and a call to glDrawElements.
     *
     * EXERCISE:
     * After completing the above, change
     * it to use compiled vertex arrays.
     *
     * EXERCISE:
     * Verify my windings are correct here ;).
     */

    /* Clear the color and depth buffers. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* We don't want to modify the projection matrix. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0.0, 0.0, -5.0);       /* Move down the z-axis. */
    glRotatef(angle, 0.0, 1.0, 0.0);    /* Rotate. */
    if (should_rotate)
    {
        /** By varying rotation speed with delay, we keep the
         *  speed of the cube constant no matter what
         */
        angle += (delay * speed) * (360.0f / 1000.0f);
        /* Keep angle beween 0 <= angle < 360 */
        while (angle > 360.0f)
        {
            angle -= 360.0f;
        }
    }
    glBegin(GL_TRIANGLES);              /* Send our triangle data to the pipeline. */

    /* Six sides to a cube, and each side takes two triangles to draw. */
    glColor4ubv(red);
    glVertex3fv(v0);
    glColor4ubv(green);
    glVertex3fv(v1);
    glColor4ubv(blue);
    glVertex3fv(v2);

    glColor4ubv(red);
    glVertex3fv(v0);
    glColor4ubv(blue);
    glVertex3fv(v2);
    glColor4ubv(white);
    glVertex3fv(v3);

    glColor4ubv(green);
    glVertex3fv(v1);
    glColor4ubv(black);
    glVertex3fv(v5);
    glColor4ubv(orange);
    glVertex3fv(v6);

    glColor4ubv(green);
    glVertex3fv(v1);
    glColor4ubv(orange);
    glVertex3fv(v6);
    glColor4ubv(blue);
    glVertex3fv(v2);

    glColor4ubv(black);
    glVertex3fv(v5);
    glColor4ubv(yellow);
    glVertex3fv(v4);
    glColor4ubv(purple);
    glVertex3fv(v7);

    glColor4ubv(black);
    glVertex3fv(v5);
    glColor4ubv(purple);
    glVertex3fv(v7);
    glColor4ubv(orange);
    glVertex3fv(v6);

    glColor4ubv(yellow);
    glVertex3fv(v4);
    glColor4ubv(red);
    glVertex3fv(v0);
    glColor4ubv(white);
    glVertex3fv(v3);

    glColor4ubv(yellow);
    glVertex3fv(v4);
    glColor4ubv(white);
    glVertex3fv(v3);
    glColor4ubv(purple);
    glVertex3fv(v7);

    glColor4ubv(white);
    glVertex3fv(v3);
    glColor4ubv(blue);
    glVertex3fv(v2);
    glColor4ubv(orange);
    glVertex3fv(v6);

    glColor4ubv(white);
    glVertex3fv(v3);
    glColor4ubv(orange);
    glVertex3fv(v6);
    glColor4ubv(purple);
    glVertex3fv(v7);

    glColor4ubv(green);
    glVertex3fv(v1);
    glColor4ubv(red);
    glVertex3fv(v0);
    glColor4ubv(yellow);
    glVertex3fv(v4);

    glColor4ubv(green);
    glVertex3fv(v1);
    glColor4ubv(yellow);
    glVertex3fv(v4);
    glColor4ubv(black);
    glVertex3fv(v5);

    glEnd();

    /*
     * EXERCISE:
     * Draw text telling the user that 'Spc'
     * pauses the rotation and 'Esc' quits.
     * Do it using vetors and textured quads.
     */

    /*
     * Swap the buffers. This this tells the driver to
     * render the next frame from the contents of the
     * back-buffer, and to set all rendering operations
     * to occur on what was the front-buffer.
     *
     * Double buffering prevents nasty visual tearing
     * from the application drawing on areas of the
     * screen that are being updated at the same time.
     *
     * If it's not double-buffered, it will just flush
     * the pipeline to screen.
     */
    SDL_GL_SwapBuffers();
}

void setup_opengl(int width, int height)
{
    float ratio = (float)width / (float)height;

    glShadeModel(GL_SMOOTH);            /* Our shading model--Gouraud (smooth). */

    /* Culling. */
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);

    glClearColor(0, 0, 0, 0);           /* Set the clear color. */
    glViewport(0, 0, width, height);    /* Setup our viewport. */

    /*
     * Change to the projection matrix and set
     * our viewing volume.
     */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    /*
     * EXERCISE:
     * Replace this with a call to glFrustum.
     */
    gluPerspective(60.0, ratio, 1.0, 1024.0);
}

int stopwatch()
{
    static int prevtime = -1;           /* static -- like global */

    /* Current time in milliseconds
     * http://www.libsdl.org/cgi/docwiki.cgi/SDL_5fGetTicks
     */
    int curtime = SDL_GetTicks();

    if (prevtime < 0)                   /* First time this function was called */
    {
        prevtime = curtime;
        return 0;
    }
    else
    {
        int diff = curtime - prevtime;  /* calculate difference */

        prevtime = curtime;             /* reset stopwatch */
        return diff;                    /* return difference */
    }
}
