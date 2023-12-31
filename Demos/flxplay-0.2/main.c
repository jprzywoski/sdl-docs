/*      vi:set nowrap ts=2 sw=2:
 */
#define version "0.2"
/*
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 */
#include <SDL/SDL.h>
/*
 */
struct
{
  FILE *file;
  Uint8 *pMembuf;
  Uint32 membufSize;
  Uint8 *pFrame;
  Uint8 *pChunk;
  Uint16 FrameCount; /* Frame Counter */
  /*
   */
  Uint32 HeaderSize;   /* Fli file size */
  Uint16 HeaderCheck;  /* Fli header check */
  Uint16 HeaderFrames; /* Number of frames in flic */
  Uint16 HeaderWidth;  /* Fli width */
  Uint16 HeaderHeight; /* Fli heigth */
  Uint16 HeaderDepth;  /* Color depth */
  Uint16 HeaderSpeed;  /* Number of video ticks between frame */
  Uint32 FrameSize;    /* Frame size in bytes */
  Uint16 FrameCheck;   /* Frame check */
  Uint16 FrameChunks;  /* Number of chunks in frame */
  Uint32 ChunkSize;    /* Size of chunk */
  Uint16 ChunkType;    /* Type of chunk */
  /*
   */
  SDL_Surface *mainscreen;
  SDL_Color colors[256];
  int screen_w;
  int screen_h;
  int screen_depth;
  int loop;
} flc;

void SDLInit(char *header)
{ /* Initialize SDL
   */
  printf("SDL: Version %d.%d.%d.\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);

  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    fprintf(stderr, "SDL: Couldn't initialize: %s\n", SDL_GetError());
    exit(2);
  }
  else
  {
    fprintf(stdout, "SDL: Video initialization succeeded.\n");
  }
  atexit(SDL_Quit);
  /* Init screen
   */
  if ((flc.mainscreen = SDL_SetVideoMode(flc.screen_w, flc.screen_h, flc.screen_depth, (SDL_HWPALETTE))) == NULL)
  {
    fprintf(stderr, "SDL: Couldn't set video mode %dx%dx%d: %s\n", flc.screen_w, flc.screen_h, flc.screen_depth, SDL_GetError());
    exit(3);
  }

  /* Set titlebar and iconbar name
   */
  SDL_WM_SetCaption(header, header);
} /* SDLInit */

#define ReadU16(tmp1, tmp2) *tmp1 = ((Uint8) * (tmp2 + 1) << 8) + (Uint8) * (tmp2);
#define ReadU32(tmp1, tmp2) *tmp1 = (((((((Uint8) * (tmp2 + 3) << 8) + ((Uint8) * (tmp2 + 2))) << 8) + ((Uint8) * (tmp2 + 1))) << 8) + (Uint8) * (tmp2));

void FlcReadFile(Uint32 size)
{
  if (size > flc.membufSize)
  {
    if (!(flc.pMembuf = realloc(flc.pMembuf, size + 1)))
    {
      printf("Realloc failed: %d\n", size);
      exit(1);
    }
  }

  if (fread(flc.pMembuf, sizeof(Uint8), size, flc.file) == 0)
  {
    printf("Can't read flx file");
    exit(1);
  }
} /* FlcReadFile */

int FlcCheckHeader(char *filename)
{
  if ((flc.file = fopen(filename, "rb")) == NULL)
  {
    return (1);
  }

  FlcReadFile(128);

  ReadU32(&flc.HeaderSize, flc.pMembuf);
  ReadU16(&flc.HeaderCheck, flc.pMembuf + 4);
  ReadU16(&flc.HeaderFrames, flc.pMembuf + 6);
  ReadU16(&flc.HeaderWidth, flc.pMembuf + 8);
  ReadU16(&flc.HeaderHeight, flc.pMembuf + 10);
  ReadU16(&flc.HeaderDepth, flc.pMembuf + 12);
  ReadU16(&flc.HeaderSpeed, flc.pMembuf + 16);

#ifdef DEBUG
  printf("flc.HeaderSize: %d\n", flc.HeaderSize);
  printf("flc.HeaderCheck: %d\n", flc.HeaderCheck);
  printf("flc.HeaderFrames: %d\n", flc.HeaderFrames);
  printf("flc.HeaderWidth: %d\n", flc.HeaderWidth);
  printf("flc.HeaderHeight: %d\n", flc.HeaderHeight);
  printf("flc.HeaderDepth: %d\n", flc.HeaderDepth);
  printf("flc.HeaderSpeed: %d\n", flc.HeaderSpeed);
#endif

  if ((flc.HeaderCheck == 0x0AF12) || (flc.HeaderCheck == 0x0AF11))
  {
    flc.screen_w = flc.HeaderWidth;
    flc.screen_h = flc.HeaderHeight;
    flc.screen_depth = 8;
    if (flc.HeaderCheck == 0x0AF11)
    {
      flc.HeaderSpeed *= 1000 / 70;
    }
    return (0);
  }
  return (1);
} /* FlcCheckHeader */

int FlcCheckFrame()
{
  flc.pFrame = flc.pMembuf + flc.FrameSize - 16;
  ReadU32(&flc.FrameSize, flc.pFrame + 0);
  ReadU16(&flc.FrameCheck, flc.pFrame + 4);
  ReadU16(&flc.FrameChunks, flc.pFrame + 6);

#ifdef DEBUG
  printf("flc.FrameSize: %d\n", flc.FrameSize);
  printf("flc.FrameCheck: %d\n", flc.FrameCheck);
  printf("flc.FrameChunks: %d\n", flc.FrameChunks);
#endif

  flc.pFrame += 16;
  if (flc.FrameCheck == 0x0f1fa)
  {
    return (0);
  }

  if (flc.FrameCheck == 0x0f100)
  {
#ifdef DEBUG
    printf("Ani info!!!\n");
#endif
    return (0);
  }

  return (1);
} /* FlcCheckFrame */

void COLORS256()
{
  Uint8 *pSrc;
  Uint16 NumColorPackets;
  Uint16 NumColors;
  Uint8 NumColorsSkip;
  int i;

  pSrc = flc.pChunk + 6;
  ReadU16(&NumColorPackets, pSrc);
  pSrc += 2;
  while (NumColorPackets--)
  {
    NumColorsSkip = *(pSrc++);
    if (!(NumColors = *(pSrc++)))
    {
      NumColors = 256;
    }
    i = 0;
    while (NumColors--)
    {
      flc.colors[i].r = *(pSrc++);
      flc.colors[i].g = *(pSrc++);
      flc.colors[i].b = *(pSrc++);
      i++;
    }
    SDL_SetColors(flc.mainscreen, flc.colors, NumColorsSkip, i);
  }
} /* COLORS256 */

void SS2()
{
  Uint8 *pSrc, *pDst, *pTmpDst;
  Sint8 CountData;
  Uint8 ColumSkip, Fill1, Fill2;
  Uint16 Lines, Count;

  pSrc = flc.pChunk + 6;
  pDst = flc.mainscreen->pixels;
  ReadU16(&Lines, pSrc);
  pSrc += 2;
  while (Lines--)
  {
    ReadU16(&Count, pSrc);
    pSrc += 2;

    while (Count & 0xc000)
    {
      /* Upper bits 11 - Lines skip
       */
      if ((Count & 0xc000) == 0xc000)
      { // 0xc000h = 1100000000000000
        pDst += (0x10000 - Count) * flc.mainscreen->pitch;
      }

      if ((Count & 0xc000) == 0x4000)
      { // 0x4000h = 0100000000000000
/* Upper bits 01 - Last pixel
 */
#ifdef DEBUG
        printf("Last pixel not implemented");
#endif
      }
      ReadU16(&Count, pSrc);
      pSrc += 2;
    }

    if ((Count & 0xc000) == 0x0000)
    { // 0xc000h = 1100000000000000
      pTmpDst = pDst;
      while (Count--)
      {
        ColumSkip = *(pSrc++);
        pTmpDst += ColumSkip;
        CountData = *(pSrc++);
        if (CountData > 0)
        {
          while (CountData--)
          {
            *(pTmpDst++) = *(pSrc++);
            *(pTmpDst++) = *(pSrc++);
          }
        }
        else
        {
          if (CountData < 0)
          {
            CountData = (0x100 - CountData);
            Fill1 = *(pSrc++);
            Fill2 = *(pSrc++);
            while (CountData--)
            {
              *(pTmpDst++) = Fill1;
              *(pTmpDst++) = Fill2;
            }
          }
        }
      }
      pDst += flc.mainscreen->pitch;
    }
  }
} /* SS2 */

void DECODE_BRUN()
{
  Uint8 *pSrc, *pDst, *pTmpDst, Fill;
  Sint8 CountData;
  int HeightCount, PacketsCount;

  HeightCount = flc.HeaderHeight;
  pSrc = flc.pChunk + 6;
  pDst = flc.mainscreen->pixels;
  while (HeightCount--)
  {
    pTmpDst = pDst;
    PacketsCount = *(pSrc++);
    while (PacketsCount--)
    {
      CountData = *(pSrc++);
      if (CountData > 0)
      {
        Fill = *(pSrc++);
        while (CountData--)
        {
          *(pTmpDst++) = Fill;
        }
      }
      else
      {
        if (CountData < 0)
        {
          CountData = (0x100 - CountData);
          while (CountData--)
          {
            *(pTmpDst++) = *(pSrc++);
          }
        }
      }
    }
    pDst += flc.mainscreen->pitch;
  }
} /* DECODE_BRUN */

void DECODE_LC()
{
  Uint8 *pSrc, *pDst, *pTmpDst;
  Sint8 CountData;
  Uint8 CountSkip;
  Uint8 Fill;
  Uint16 Lines, tmp;
  int PacketsCount;

  pSrc = flc.pChunk + 6;
  pDst = flc.mainscreen->pixels;

  ReadU16(&tmp, pSrc);
  pSrc += 2;
  pDst += tmp * flc.mainscreen->pitch;
  ReadU16(&Lines, pSrc);
  pSrc += 2;
  while (Lines--)
  {
    pTmpDst = pDst;
    PacketsCount = *(pSrc++);
    while (PacketsCount--)
    {
      CountSkip = *(pSrc++);
      pTmpDst += CountSkip;
      CountData = *(pSrc++);
      if (CountData > 0)
      {
        while (CountData--)
        {
          *(pTmpDst++) = *(pSrc++);
        }
      }
      else
      {
        if (CountData < 0)
        {
          CountData = (0x100 - CountData);
          Fill = *(pSrc++);
          while (CountData--)
          {
            *(pTmpDst++) = Fill;
          }
        }
      }
    }
    pDst += flc.mainscreen->pitch;
  }
} /* DECODE_LC */

void DECODE_COLOR()
{
  Uint8 *pSrc;
  Uint16 NumColors, NumColorPackets;
  Uint8 NumColorsSkip;
  int i;

  pSrc = flc.pChunk + 6;
  ReadU16(&NumColorPackets, pSrc);
  pSrc += 2;
  while (NumColorPackets--)
  {
    NumColorsSkip = *(pSrc++);
    if (!(NumColors = *(pSrc++)))
    {
      NumColors = 256;
    }
    i = 0;
    while (NumColors--)
    {
      flc.colors[i].r = *(pSrc++) << 2;
      flc.colors[i].g = *(pSrc++) << 2;
      flc.colors[i].b = *(pSrc++) << 2;
      i++;
    }
    SDL_SetColors(flc.mainscreen, flc.colors, NumColorsSkip, i);
  }
} /* DECODE_COLOR  */

void DECODE_COPY()
{
  Uint8 *pSrc, *pDst;
  int Lines = flc.screen_h;
  pSrc = flc.pChunk + 6;
  pDst = flc.mainscreen->pixels;
  while (Lines-- > 0)
  {
    memcpy(pDst, pSrc, flc.screen_w);
    pSrc += flc.screen_w;
    pDst += flc.mainscreen->pitch;
  }
} /* DECODE_COPY */

void BLACK()
{
  Uint8 *pDst;
  int Lines = flc.screen_h;
  pDst = flc.mainscreen->pixels;
  while (Lines-- > 0)
  {
    memset(pDst, 0, flc.screen_w);
    pDst += flc.mainscreen->pitch;
  }
} /* BLACK */

void FlcDoOneFrame()
{
  int ChunkCount;
  ChunkCount = flc.FrameChunks;
  flc.pChunk = flc.pMembuf;
  if (SDL_LockSurface(flc.mainscreen) < 0)
    return;
  while (ChunkCount--)
  {
    ReadU32(&flc.ChunkSize, flc.pChunk + 0);
    ReadU16(&flc.ChunkType, flc.pChunk + 4);

#ifdef DEBUG
    printf("flc.ChunkSize: %d\n", flc.ChunkSize);
    printf("flc.ChunkType: %d\n", flc.ChunkType);
#endif

    switch (flc.ChunkType)
    {
    case 4:
      COLORS256();
      break;
    case 7:
      SS2();
      break;
    case 11:
      DECODE_COLOR();
      break;
    case 12:
      DECODE_LC();
      break;
    case 13:
      BLACK();
      break;
    case 15:
      DECODE_BRUN();
      break;
    case 16:
      DECODE_COPY();
      break;
    case 18:
#ifdef DEBUG
      printf("Chunk 18 not yet done.\n");
#endif
      break;
    default:
      printf("Ieek an non implemented chunk type: %d\n", flc.ChunkType);
    }
    flc.pChunk += flc.ChunkSize;
  }
  SDL_UnlockSurface(flc.mainscreen);
} /* FlcDoOneFrame */

void SDLWaitFrame(void)
{
  static Uint32 oldTick = 0;
  Uint32 currentTick;
  Sint32 waitTicks;

  currentTick = SDL_GetTicks();
  waitTicks = (oldTick += (flc.HeaderSpeed)) - currentTick;

  if (waitTicks > 0)
  {
    SDL_Delay(waitTicks);
  }
} /* SDLWaitFrame */

void FlcInitFirstFrame()
{
  flc.FrameSize = 16;
  flc.FrameCount = 0;
  if (fseek(flc.file, 128, SEEK_SET))
  {
    printf("Fseek read failed\n");
    exit(1);
  };
  FlcReadFile(flc.FrameSize);
} /* FlcInitFirstFrame */

void FlcInit(char *filename)
{
  flc.pMembuf = NULL;
  flc.membufSize = 0;

  if (FlcCheckHeader(filename))
  {
    printf("Wrong header\n");
    exit(1);
  }
  SDLInit(filename);
} /* FlcInit */

void FlcDeInit()
{
  fclose(flc.file);
  free(flc.pMembuf);
} /* FlcDeInit */

void FlcMain()
{
  int quit = 0;
  SDL_Event event;
  FlcInitFirstFrame();
  while (!quit)
  {
    flc.FrameCount++;
    if (FlcCheckFrame())
    {
      if (flc.FrameCount <= flc.HeaderFrames)
      {
        printf("Frame failure -- corrupt file?\n");
        exit(1);
      }
      else
      {
        if (flc.loop)
          FlcInitFirstFrame();
        else
        {
          SDL_Delay(1000);
          quit = 1;
        }
        continue;
      }
    }

    FlcReadFile(flc.FrameSize);

    if (flc.FrameCheck != 0x0f100)
    {
      FlcDoOneFrame();
      SDLWaitFrame();
      /* TODO: Track which rectangles have really changed */
      SDL_UpdateRect(flc.mainscreen, 0, 0, 0, 0);
    }

    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
      case SDL_KEYDOWN:
        quit = 1;
        break;
      case SDL_QUIT:
        quit = 1;
      default:
        break;
      }
    }
  }
} /* FlcMain */

void FlxplayHelp()
{
  printf("FLX player (%s) with SDL output (jasper@il.fontys.nl)\n", version);
  printf("View readme file for more information\n\n");
  printf("flxplay [-l] [filename]\n");
  exit(1);
} /* FlxplayHelp */

main(int argc, char **argv)
{
  int c;

  flc.loop = 0;
  for (c = 1; argv[c] && (argv[c][0] == '-'); ++c)
  {
    if (strcmp(argv[c], "-l") == 0)
    {
      printf("Looping mode\n");
      flc.loop = 1;
    }
    else
    {
      FlxplayHelp();
    }
  }
  if (!argv[c])
  {
    FlxplayHelp();
  }

  FlcInit(argv[c]);
  FlcMain();
  FlcDeInit();
  exit(0);
} /* main */
