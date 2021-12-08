#include "config.hpp"
#include "draw.hpp"

SDL_Renderer *d_renderer;
SDL_Window *d_window;

struct point ORIGIN;
struct projection PROJ = {
	{1,  0.5},
	{1, -0.5},
};
struct projection PROJ_INV = {
	{0.5, 0.5},
	{  1,  -1}
};
/* (1 | 0.5 | 1 ; 1 | -0.5 | 1; 0 | 1 | 1) */
/* (1.5 | -0.5 | -1 ; 1 | -1 | 0; -1 | 1 | 1) */

int draw_Init (void)
{
	if (SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Cannot initialize SDL\n");
		fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
		return 1;
	}

	d_window = SDL_CreateWindow(
		"Title", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		g_width, g_height, SDL_WINDOW_SHOWN
	);
	if (!d_window) {
		fprintf(stderr, "Window could not be created\n");
		fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
		return 2;
	}

	d_renderer = SDL_CreateRenderer(d_window, -1, SDL_RENDERER_ACCELERATED);
	if (!d_renderer) {
		fprintf(stderr, "Renderer could not be created\n");
		fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
		return 3;
	}

	ORIGIN.x = 0;
	ORIGIN.y = g_height / 2;
	return 0;
}

void draw_Update (void)
{
}

void draw_Close (void)
{
	SDL_DestroyRenderer(d_renderer);
	SDL_DestroyWindow(d_window);
	SDL_Quit();
}

struct point point_Add (struct point that, struct point p)
{
	that.x += p.x;
	that.y += p.y;
	return that;
}

struct point point_Sub (struct point that, struct point p)
{
	that.x -= p.x;
	that.y -= p.y;
	return that;
}

struct point point_MultiplyProj (struct point that, struct projection p)
{
	struct point out;
	out.x = that.x * p.x.x + that.y * p.y.x;
	out.y = that.x * p.x.y + that.y * p.y.y;
	return out;
}

void draw_SetColor (int r, int g, int b, int a)
{
	if (!g_pause) {
		SDL_SetRenderDrawColor(d_renderer, r, g, b, a);
		return;
	}
	SDL_SetRenderDrawColor(d_renderer, 0xFF - r, 0xFF - g, 0xFF - b, a);
}
