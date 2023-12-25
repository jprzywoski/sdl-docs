#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "fcntl.h"
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#ifdef _WIN32
#include "io.h"
#endif

/* Function prototypes */
void DestroySDL();
void InitSDL();
void InitVideo();
void InitAudio();

SDL_Surface *LoadBitmap(char *resourcefilename, char *bitmapfilename);
Mix_Chunk *LoadSound(char *resourcefilename, char *soundfilename);
char *GetBufferFromResource(char *resourcefilename, char *resourcename, int *filesize);

void DisplayBitmap(SDL_Surface *image, int x, int y);
void PlaySound(Mix_Chunk *sound);

SDL_Surface
*screen;                    /* The screen object to which we blit images (I know, I know... mommy always told you not to use globals!) */

int main(int argc, char *argv[])
{
    /* Perform some SDL initialization steps */
    InitSDL();
    InitVideo();
    InitAudio();

    /* Call our resource loaders to fill our sound and image pointers */
    SDL_Surface *image = LoadBitmap("resource.dat", "image.bmp");
    Mix_Chunk *sound = LoadSound("resource.dat", "sound.wav");

    /* Display the bitmap we loaded from the resource file */
    DisplayBitmap(image, 110, 70);

    /* Play the sound we loaded from the resource file */
    PlaySound(sound);

    /* Pause for 2 seconds so we can see the bitmap */
    SDL_Delay(2000);

    /* Don't forget to clean up! */
    Mix_FreeChunk(sound);
    SDL_FreeSurface(image);
    return 0;
}

void InitSDL()
{
    /* Init SDL, and check for errors */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    /* Call quit when this program exits */
    atexit(DestroySDL);
}

void InitVideo()
{
    /* Initialize SDL video, with some reasonable settings */
    screen = SDL_SetVideoMode(320, 240, 16, SDL_DOUBLEBUF);
    if (screen == NULL)
    {
        printf("Unable to set video mode: %s\n", SDL_GetError());
        exit(1);
    }
}

void InitAudio()
{
    /* Initialize SDL audio, with some reasonable settings */
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
    {
        printf("Unable to open audio: %s\n", Mix_GetError());
        exit(1);
    }
}

SDL_Surface *LoadBitmap(char *resourcefilename, char *bitmapfilename)
{
    /* Get the bitmap's buffer and size from the resource file */
    int filesize = 0;
    char *buffer = GetBufferFromResource(resourcefilename, bitmapfilename, &filesize);

    /* Load the buffer into a surface using RWops */
    SDL_RWops *rw = SDL_RWFromMem(buffer, filesize);
    SDL_Surface *temp = SDL_LoadBMP_RW(rw, 1);

    /* Release the bitmap buffer memory */
    free(buffer);
    /* Were we able to load the bitmap? */
    if (temp == NULL)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
        exit(1);
    }

    /* Convert the image to optimal display format */
    SDL_Surface *image;

    image = SDL_DisplayFormat(temp);

    /* Free the temporary surface */
    SDL_FreeSurface(temp);

    /* Return our loaded image */
    return image;
}

Mix_Chunk *LoadSound(char *resourcefilename, char *soundfilename)
{
    /* Get the sound's buffer and size from the resource file */
    int filesize = 0;
    char *buffer = GetBufferFromResource(resourcefilename, soundfilename, &filesize);

    /* Load the buffer into a surface using RWops */
    SDL_RWops *rw = SDL_RWFromMem(buffer, filesize);
    Mix_Chunk *sound = Mix_LoadWAV_RW(rw, 1);

    /* Release the buffer memory */
    free(buffer);

    /* Return the sound */
    return sound;
}

char *GetBufferFromResource(char *resourcefilename, char *resourcename, int *filesize)
{
    /* Try to open the resource file in question */
    int fd = open(resourcefilename, O_RDONLY);

    if (fd < 0)
    {
        perror("Error opening resource file");
        exit(1);
    }
    /* Make sure we're at the beginning of the file */
    lseek(fd, 0, SEEK_SET);

    /* Read the first INT, which will tell us how many files are in this resource */
    int numfiles;

    read(fd, &numfiles, sizeof (int));

    /* Get the pointers to the stored files */
    int *filestart = (int *)malloc(numfiles);

    read(fd, filestart, sizeof (int) * numfiles);

    /* Loop through the files, looking for the file in question */
    int filenamesize;
    char *buffer;
    int i;

    for (i = 0; i < numfiles; i++)
    {
        char *filename;

        /* Seek to the location */
        lseek(fd, filestart[i], SEEK_SET);
        /* Get the filesize value */
        read(fd, filesize, sizeof (int));
        /* Get the size of the filename string */
        read(fd, &filenamesize, sizeof (int));
        /* Size the buffer and read the filename */
        filename = (char *)malloc(filenamesize + 1);
        read(fd, filename, filenamesize);
        /* Remember to terminate the string properly! */
        filename[filenamesize] = '\0';
        /* Compare to the string we're looking for */
        if (strcmp(filename, resourcename) == 0)
        {
            /* Get the contents of the file */
            buffer = (char *)malloc(*filesize);
            read(fd, buffer, *filesize);
            free(filename);
            break;
        }
        /* Free the filename buffer */
        free(filename);
    }
    /* Release memory */
    free(filestart);

    /* Close the resource file! */
    close(fd);
    /* Did we find the file within the resource that we were looking for? */
    if (buffer == NULL)
    {
        printf("Unable to find '%s' in the resource file!\n", resourcename);
        exit(1);
    }
    /* Return the buffer */
    return buffer;
}

void DisplayBitmap(SDL_Surface *image, int x, int y)
{
    /* Define the source and destination rectangles for our blit */
    SDL_Rect src, dest;

    src.x = 0;
    src.y = 0;
    src.w = image->w;
    src.h = image->h;
    dest.x = x;
    dest.y = y;
    dest.w = x + image->w;
    dest.h = y + image->h;

    /* Blit the image, and update the screen */
    SDL_BlitSurface(image, &src, screen, &dest);
    SDL_UpdateRect(screen, 0, 0, 0, 0);
}

void PlaySound(Mix_Chunk *sound)
{
    /* Play the sound on the first channel available, with no looping */
    if (Mix_PlayChannel(-1, sound, 0) == -1)
    {
        /* Must've been an error playing the sound! */
        printf("Mix_PlayChannel: %s\n", Mix_GetError());
    }
}

void DestroySDL()
{
    /* Kill SDL_mixer */
    Mix_CloseAudio();

    /* Kill the rest of SDL */
    SDL_Quit();
}
