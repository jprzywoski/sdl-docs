Modest Bricks
written February 1, 2005
last updated April 29, 2019

Greg Kennedy
kennedy.greg@gmail.com
https://greg-kennedy.com

---

This is a pretty straightforward Tetris clone written in C using SDL 1.2 and SDL_Mixer.  I went ahead and tried to make this a pretty big deal - as expansive a game as I've ever written.  Not saying much, I know, but it's got all the features a basic Tetris game should have:

* Scoring and high scores
* Saves game options
* Piece lookahead
* Attractive graphics (ha ha)
* Custom music and sound

Every 10 lines cleared will advance the level, up to a maximum of level 9.

Scoring works like this:
* 1 line = 1 point
* 2 lines = 3 points
* 3 lines = 6 points
* 4 lines = 10 points

---

Tetris clones can get pretty advanced these days: there are algorithms for generating "fair" pieces e.g. 7-bag, special moves (T spin and wall kick), hard drop, "ghost" piece images, and so on.  In the interest of simplicity, my clone does not bother with any of that.

The source code is included so you can check it out.  There might be some useful bits in there for learning SDL 1.2.

If you like the music enough, just ask me and I'll probably let you stick it in your own projects.

This was originally written for the now-defunct University of Arkansas Game Development Club.

Thanks for trying my game - I'd love to know what you think.

-Greg

PS - You can get SDL and SDL_Mixer at http://www.libsdl.org - it's a great library and I wholeheartedly recommend it!