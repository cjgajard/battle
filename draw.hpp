#ifndef DRAW_H
#define DRAW_H
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include "config.hpp"
#define TILESIZ 32

extern SDL_Renderer *d_renderer;
extern SDL_Window *d_window;

int draw_Init (void);
void draw_Close (void);
void draw_Update (void);
void draw_SetColor (int r, int g, int b, int a);
#endif
