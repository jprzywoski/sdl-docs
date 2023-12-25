/*
 *  Copyright (C) 1999 Optimum
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <stdlib.h>
#include "fps.h"

unsigned char optfps = 0;
#ifdef X11
static struct timeval time_begin;
static struct timeval time_end;
#else
#ifdef WIN32
#include <windows.h>
#endif
static Uint32 time_begin;
static Uint32 time_end;
#endif

static unsigned int loops;

void init_fps () {

#ifdef X11
  gettimeofday ( &time_begin , NULL );
#else
  time_begin = SDL_GetTicks();
#endif
  loops = 0;

}

void next_fps () {

  loops++;
  
}

void aff_fps (char *argv0) {

  unsigned long duration;

  double fps;

  double mem;
  char os_name[32];
  char os_vers[32];
  char cpu[64];
  char freq[32];

  FILE *cpuinfo;
  char txt[256];

#ifdef X11
  gettimeofday ( &time_end , NULL );
  duration =
    (time_end.tv_sec  - time_begin.tv_sec )*1000 +
    (time_end.tv_usec - time_begin.tv_usec)/1000;
#else
  time_end = SDL_GetTicks();
  duration = (time_end - time_begin);
#endif
  fps = (1000.0*loops)/duration;
  
  /* OS */

#if defined(linux) || defined(__BEOS__)
  {
    struct utsname kernel;
    if (uname (&kernel) < 0) {
      strcpy ( os_name , "Unknown" );
      strcpy ( os_vers , "" );
    }
    else {
      strncpy ( os_name , kernel.sysname , 32 );
      strncpy ( os_vers , kernel.release , 32 );
    }
  }
#elif defined(WIN32)
  {
    OSVERSIONINFO os_info;
    os_info.dwOSVersionInfoSize = sizeof(os_info);
    GetVersionEx(&os_info);
    switch (os_info.dwPlatformId) {
      case VER_PLATFORM_WIN32s:
        strncpy ( os_name , "Win32s" , 32 );
        break;
      case VER_PLATFORM_WIN32_WINDOWS:
        if ( os_info.dwMinorVersion == 0 ) {
          strncpy ( os_name , "Win95" , 32 );
        } else {
          strncpy ( os_name , "Win98" , 32 );
        }
        break;
      case VER_PLATFORM_WIN32_NT:
        strncpy ( os_name , "WinNT" , 32 );
        break;
      default:
        strncpy ( os_name , "Win32" , 32 );
        break;
    }
    sprintf (os_vers, "%d.%d", os_info.dwMajorVersion, os_info.dwMinorVersion);
  }
#else
  {
    strncpy ( os_name , "???" , 32 );
    strncpy ( os_vers , "???" , 32 );
  }
#endif
  
  /* MEM */

#if defined(linux)
  {
    struct stat statmem;
    stat ( "/proc/kcore" , &statmem );
    mem = ((float)statmem.st_size)/1024/1024;
  }
#elif defined(WIN32)
  {
    MEMORYSTATUS meminfo;
    GlobalMemoryStatus(&meminfo);
    mem = ((float)meminfo.dwTotalPhys)/1024/1024;
  }
#else
  mem = 0.0;
#endif

  /* CPU & FREQ */

  strcpy ( cpu , "Unknown\n" );
  strcpy ( freq , "Unknown" );

#if defined(linux)

  cpuinfo = fopen ( "/proc/cpuinfo" , "r" );
  if ( cpuinfo != NULL )
    while ( fgets ( txt , 255 , cpuinfo ) ) {

      if ( !strncmp ( txt , "model" , 5 ) ) {

	int i=0;

	while (txt[i]!=':')
	  i++;
	i += 2;
	strncpy ( cpu , txt+i , 64 );

      }	

      if ( !strncmp ( txt , "cpu MHz" , 7 ) ) {

	int i=0;

	while (txt[i]!=':')
	  i++;
	i += 2;
	sprintf ( freq , "%d" , atoi(txt+i) );

      }	

    }      

#endif /* linux */
      
  /* RESULTS */

  printf ( "Please, mail us (optimum@citeweb.net) this info :\n" );
#ifdef X11
  printf ( "   Prog   : %s (X11)\n" , argv0 );
#else
  printf ( "   Prog   : %s (SDL)\n" , argv0 );
#endif
  printf ( "   Size   : %dx%d\n" , width , height );
  printf ( "   Depth  : %d\n" , depth );
  printf ( "   OS     : %s %s\n" , os_name , os_vers );
  printf ( "   CPU    : %s" , cpu );
  printf ( "   FREQ   : %s Mhz\n" , freq );
  printf ( "   MEM    : %.0f MB\n" , mem );
#ifdef __GNUC__
  printf ( "   GCC    : %d.%d\n" , __GNUC__, __GNUC_MINOR__ );
#endif
  printf ( "   GFX    : (write here the name of your graphic card)\n" );
  printf ( "   FPS    : %g\n" , fps );
  
}
