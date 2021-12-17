CXXFLAGS = -std=c++17 -Werror -Wall -Wpedantic $(shell pkg-config --cflags sdl2)
LIBS = -lm $(shell pkg-config --libs sdl2 SDL2_gfx)
OUT = $(shell basename $(CURDIR))
OBJ = main.o geometry.o draw.o game.o unit.o

.PHONY: clean Makeobjects

$(OUT): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	-$(RM) *.o
	-$(RM) $(OUT)

Makeobjects:
	find . -name '*.cpp' -exec $(CXX) -MM {} \; >$@

include Makeobjects
