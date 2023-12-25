CFLAGS=-g -O2 -Wall $(shell sdl-config --cflags)
LDFLAGS=-g -O2 -Wall $(shell sdl-config --libs) -lSDL_ttf

PROGRAMS=$(basename $(wildcard *.c))

all: $(PROGRAMS) freefont-ttf

freefont-ttf: freefont-ttf.tar.gz
	rm -rf freefont-ttf
	gzip -dc freefont-ttf.tar.gz | tar -xv

clean:
	rm -rf $(PROGRAMS) *.o freefont-ttf

new: clean all
