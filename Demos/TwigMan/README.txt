   TwigMan - An SDL demonstration for CSCI 321.
   Version 0.9.0
   Written by Dr. Evan L. Schemm
              Lake Superior State University
              School of Mathematics and Computer Science

   Please send questions, comments, suggestions or (gasp!) bugfixes to:
          elschemm@furball.schemm.lssu.edu

   Copyright 2007 by LSSU School of MA/CS

   Licensed under GPL.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.


   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA





  TwigMan is the third demonstration program produced as a talking point for
  CSCI 321, Computer Graphics, at Lake Superior State University.

  TwigMan started as a fairly simple jumping game meant to demonstate gravity
  and platform games.  Because of chemicals released into the building air
  ducts, it morphed into this.  

  The goal of TwigMan is for Twig to re-collect the cable testers at the 
  end of CSCI 221 (Computer Networking).  Sadly, over the course of the years,
  a number of them have 'disappeared'.  Interestingly, the students do have 
  a remarkable tendency to leave their textbooks in the lab.  And of course, 
  Dr. Terwilliger is never far from his beverage of choice.  I'm still not
  entirely certain how he convinced the University to move the vending machine
  outside the lab.

  The standard keys for TwigMan are as follows:
  5 - Insert Coin (MAME standard)
  1 - Player 1 start (MAME standard)
  Left - Move left
  Right - Move right
  Up - Jump
  F11 - Toggle Fullscreen
  P - Pause (only during normal gameplay)

  These keys can be re-mapped to any keys on the keyboard by changing the 
  default values in the file 'twigman.conf'.


  With TwigPower, TwigMan can double (and triple, ...) jump.  He is also immune
  to the FabbriCar and Falling Boger.

  You have to land on the students head to squash them (and get the points).
  No, you don't get any cable testers they stole back (already sold on ebay).

  Every five levels, there is an intermission.  The intermissions chronicle 
  the arrival of the four CS faculty in the MA/CS department.  

  There are six different level patterns, the seventh level is an identical 
  board to the first, and so on.
  
  Level 31 is the last level, when you complete it, you restart level 31 again.
  You will be treated to a random intermission each time that you complete
  level 31 (hey... it's not easy to get that far!  You deserve a reward!).

  It is possible to insert additional coins and continue from the spot that 
  you ran out of lives.  You have 9 seconds to insert additional coins and 
  press Player 1 start to do this.  It is useful if you are just short of the 
  next intermission.  

  This game contains dozens, if not 100 inside jokes to the department.  If 
  you really want to know what they all mean, come up for a visit.  You can
  find us in Sault Ste. Marie, MI!  

  This game has been successfully compiled on:
       Linux (using gcc)
       Windows (cygwin gcc)
       Windows (Dev-Cpp)
       Solaris (using gcc)
       
  If you successfully compile it for some other platform, please let us know!


  See the other SDL demonstrations at 
  http://furball.schemm.lssu.edu/SDL



AudioFiles acquired from:
// Downloaded from flashkit.com

  sounds[endgame]=Mix_LoadWAV("./audio/Terminated.wav");//Made by: Ilhan Bayrak
  sounds[evil_laugh]=Mix_LoadWAV("./audio/evil_laugh.wav");//Made by: Wiretrip
  sounds[money] = Mix_LoadWAV("./audio/money.wav");// Made by: Colin Dodd
  sounds[death] = Mix_LoadWAV("./audio/death.wav"); // Made by: Doron Add
  sounds[can_open] = Mix_LoadWAV("./audio/can_open.wav");// Made by SodaBush
  sounds[falling] = Mix_LoadWAV("./audio/falling.wav");// Made by Deflux Design
  sounds[steal] = Mix_LoadWAV("./audio/steal.wav"); // Made by Amit Patel
  sounds[intermission] = Mix_LoadWAV("./audio/cheer.wav"); // Made by A Algien
  sounds[crash] = Mix_LoadWAV("./audio/crash.wav"); // Made by Adrian Gallant
  sounds[thud] = Mix_LoadWAV("./audio/thud.wav"); // Made by Splinteh
  sounds[meow] = Mix_LoadWAV("./audio/meow.wav"); // Made by Starmanltd
  sounds[purr] = Mix_LoadWAV("./audio/purr.wav"); // Made by Richard Harding
  sounds[gun] = Mix_LoadWAV("./audio/gun.wav"); // Made by Maboroshi Tsukuru
  sounds[life] = Mix_LoadWAV("./audio/life.wav"); // Made by Crow xxx
  // The following sounds made by:  Sith Master
  sounds[appear]=Mix_LoadWAV("./audio/appear.wav");
  sounds[get_tester] = Mix_LoadWAV("./audio/get_item.wav");
  sounds[get_book] = Mix_LoadWAV("./audio/swish.wav");

 
  // Downloaded from www.grsites.com/sounds/cartoon008.shtml (cartoon148.wav)
  sounds[flower] = Mix_LoadWAV("./audio/flower.wav");
  // Downloaded from www.grsites.com/sounds/cartoon009.shtml (cartoon172.wav)
  sounds[squish] = Mix_LoadWAV("./audio/squish.wav");
  // Downloaded from www.grsites.com/sounds/horror001.html (horror004.wav)
  sounds[roar] = Mix_LoadWAV("./audio/roar.wav");
  // Downloaded from simplythebest.net/sounds/WAV/cartoon_wavs.html
  sounds[jumpping] = Mix_LoadWAV("./audio/boing_3.wav");
  // Downloaded from www.acoustica.com/mp3-audio-mixer
  sounds[drive] = Mix_LoadWAV("./audio/fabbri.wav");
  // Downloaded from freesound.iua.upf.edu
  sounds[bookdrop] = Mix_LoadWAV("./audio/oops.wav");


  // Downloaded from flashkit.com  Made by: Nemo
  musics[bg] = Mix_LoadMUS("./audio/Happ-Nemo-5891.wav");
  musics[inter1] = Mix_LoadMUS("./audio/Boss_1_T-miqal-8855.wav");


Dinosaur Pics from:
  // Dinosaur clipart in the public domain as certified by fundraw.com
  // Can be found at www.fundraw.com/clipart/categories/00000139
  // Modified by Dr. Evan L. Schemm for use in this program

Font from:
  // Font downloaded from www.1001fonts.com  
  See copyright notice in monofont.txt