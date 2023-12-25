/*******************************************************************
 *                                                                 *
 *                        Using SDL With OpenGL                    *
 *                                                                 *
 *                    Tutorial by Kyle Foley (sdw)                 *
 *                                                                 *
 * http://gpwiki.org/index.php/SDL:Tutorials:Using_SDL_with_OpenGL *
 *                                                                 *
 *******************************************************************/

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include <stdio.h>

int main(int argc, char *argv[])
{
    SDL_Surface *screen;

    /* Slightly different SDL initialization */
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);                /* *new* */

    screen = SDL_SetVideoMode(640, 480, 16, SDL_OPENGL);        /* *changed* */
    if (!screen)
    {
        printf("Unable to set video mode: %s\n", SDL_GetError());
        return 1;
    }
    /* Set the OpenGL state after creating the context with SDL_SetVideoMode */

    glClearColor(0, 0, 0, 0);

    glEnable(GL_TEXTURE_2D);            /* Need this to display a texture */

    glViewport(0, 0, 640, 480);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, 640, 480, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* Load the OpenGL texture */

    GLuint texture;                     /* Texture object handle */
    SDL_Surface *surface;               /* Gives us the information to make the texture */

    if ((surface = SDL_LoadBMP("image.bmp")))
    {
        /* Check that the image's width is a power of 2 */
        if ((surface->w & (surface->w - 1)) != 0)
        {
            printf("warning: image.bmp's width is not a power of 2\n");
        }
        /* Also check if the height is a power of 2 */
        if ((surface->h & (surface->h - 1)) != 0)
        {
            printf("warning: image.bmp's height is not a power of 2\n");
        }
        /* Have OpenGL generate a texture object handle for us */
        glGenTextures(1, &texture);

        /* Bind the texture object */
        glBindTexture(GL_TEXTURE_2D, texture);

        /* Set the texture's stretching properties */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        /* Edit the texture object's image data using the information SDL_Surface gives us */
        glTexImage2D(GL_TEXTURE_2D, 0, 3, surface->w, surface->h, 0, GL_BGR, GL_UNSIGNED_BYTE, surface->pixels);
    }
    else
    {
        printf("SDL could not load image.bmp: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    /* Free the SDL_Surface only if it was successfully created */
    if (surface)
    {
        SDL_FreeSurface(surface);
    }
    /* Clear the screen before drawing */
    glClear(GL_COLOR_BUFFER_BIT);

    /* Bind the texture to which subsequent calls refer to */
    glBindTexture(GL_TEXTURE_2D, texture);

    glBegin(GL_QUADS);
    /* Top-left vertex (corner) */
    glTexCoord2i(0, 0);
    glVertex3f(100, 100, 0);

    /* Bottom-left vertex (corner) */
    glTexCoord2i(1, 0);
    glVertex3f(228, 100, 0);

    /* Bottom-right vertex (corner) */
    glTexCoord2i(1, 1);
    glVertex3f(228, 228, 0);

    /* Top-right vertex (corner) */
    glTexCoord2i(0, 1);
    glVertex3f(100, 228, 0);
    glEnd();

    SDL_GL_SwapBuffers();

    /* Wait for 3 seconds to give us a chance to see the image */
    SDL_Delay(3000);

    /* Now we can delete the OpenGL texture and close down SDL */
    glDeleteTextures(1, &texture);

    SDL_Quit();

    return 0;
}
