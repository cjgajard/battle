CFLAGS = -ansi -g -Werror -Wall -Wpedantic $(shell pkg-config --cflags sdl2)
LIBS = -lm $(shell pkg-config --libs sdl2 SDL2_gfx)

a.out: main.o geometry.o draw_sdl.o command.o game.o unit.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

.PHONY: clean
clean:
	-$(RM) *.o
	-$(RM) $(OUT)

.PHONY: Makeobjects
Makeobjects:
	find . -name '*.c' -exec $(CC) -MM {} \; >$@

include Makeobjects
