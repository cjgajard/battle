#include "battle.h"
#include "draw.h"

SDL_Renderer *d_renderer;
SDL_Window *d_window;

int draw_init (void)
{
	if (SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Cannot initialize SDL\n");
		fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
		return 1;
	}

	d_window = SDL_CreateWindow(
		"Battle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		g_width, g_height, SDL_WINDOW_SHOWN
	);
	if (d_window == NULL) {
		fprintf(stderr, "Window could not be created\n");
		fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
		return 2;
	}

	d_renderer = SDL_CreateRenderer(d_window, -1, SDL_RENDERER_ACCELERATED);
	if (d_renderer == NULL) {
		fprintf(stderr, "Renderer could not be created\n");
		fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
		return 3;
	}

	ORIGIN.x = 0;
	ORIGIN.y = g_height / 2;
	return 0;
}

void draw_update (void)
{
}

void draw_close (void)
{
	SDL_DestroyRenderer(d_renderer);
	SDL_DestroyWindow(d_window);
	SDL_Quit();
}
