Moon Lander for Linux (and now, Windows!)

Standard BSD License applies to this software:

Copyright 2001 David J. Blood. All rights reserved. 

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met: 

1.Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 
2.Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. 

THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

-----

This is beta software.

Moon Lander is a 2D game of gravity.  Land your ship on the lading pad.  Don't go too fast, or you will crash.


-----

Installation (linux):

This game ships with a pre-compiled binary and source.  The binary should work for you.  If not, try:  make clean, make.

If you don't have a soundcard, add this line to gamelib.h at the top:

#define NOSOUND

and do a make clean;make

This game requires SDL 1.2, SDL_image 1.2 and SDL_mixer 1.2.  These libraries can be found at:

http://www.libsdl.org/

Local (single user):
Unpack somewhere, cd to dir, and run ./moon-lander.bin

System Wide (all users):
Unpack somewhere, cd to dir and, as root, type "make install".  This will copy everything to /usr/share/moon-lander and put the exectuable in /usr/bin.  This will allow the command "moon-lander" to be executed from any directory. 

-----

Installation (Windows):

Unzip somwhere and double click on moon-lander.exe

-----

Thanks to Garrett Banuk <mongoose@wpi.edu> for the text libs and fonts (DT_Drawtext)

comments, etc to geekd@yahoo.com

-----

Changelog:

2001-08-29 - (1.0pre1):
  Made windows port, changed all game art and sounds to Free art and sounds (geekd, rdaniels), added demo mode (mheckman), added options (geekd).  This is the last version before 1.0 (find those bugs!)
2001-07-14 - (0.9.0):
  Added logo, sounds and instructions to the between game screen.  Put in stubbs for options. Merged in BMD's fancy terrain.
2001-07-08:
  Score for each landing pad now displays during level warm-up. Unused images deleted, others changed to smaller format for a smaller download.  Implements a "make install" for system wide use.  Changed some filenames to comply with the FHS
2001-07-07:
  Added sound when you get bonus life.  Changed "congratulations" sound when you land (copyright issues).  Changed bonus life score to 10,000. Added install.sh.
2001-07-06: 
  Gravity gets stronger as game progresses.  Landing pad shows red if speed unsafe, some landing pads have higher score and lower landing speed.  New between game screens, multiple "lives", pause feature, some code cleanup.
2001-07-04: 
  Changed name to Moon Lander, various scoring changes
2001-07-03b: 
  Added BSD License, fixed glitch in backround art
2001-07-03: 
  Graphics improvements - ship, background, thrust, fonts 
2001-07-02: 
  initial release

-----

Credits:

David Blood - geekd@yahoo.com: 
Main programmer.  Any sloppy code or bugs are all my fault.

Mike Heckman - mike@heckman.net: 
Demo mode AI, Incremental gravity, variable speed landing pads, multiple lives, red pad speed warning, pause, many suggestions.

Ryan Daniels - pacmanfever@hotmail.com:
Ship, thrust and explosion images.

Brian "Mo" Degenhardt - bmd@mp3.com: 
Fancy terrain.

magigames.org


