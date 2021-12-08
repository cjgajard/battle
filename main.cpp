#include <SDL.h>
#include <time.h>
#include "config.hpp"
#include "draw.hpp"
#include "game.hpp"

int g_width, g_height;
int g_maxfps;
unsigned int g_frametime, g_updatetime;
int g_running;

int main (int argc, char *argv[])
{
	unsigned int g_time, g_lag;
	int err;

	(void)argc;
	(void)argv;
	err = 0;
	g_width = 800;
	g_height = 600;
	g_maxfps = 60;
	g_frametime = 1000.0 / g_maxfps;
	g_updatetime = g_frametime / 2;

	srand(time(NULL));

	if (draw_Init()) {
		fprintf(stderr, "draw_Init: %s\n", SDL_GetError());
		err = 1;
		goto close;
	}
	if (game_Init()) {
		fprintf(stderr, "game_Init: %s\n", SDL_GetError());
		err = 1;
		goto close;
	}

	/* right before start */
	g_time = SDL_GetTicks();
	g_lag = 0;
	g_running = 1;

	while (g_running) {
		/* We want to do update the game state in small and stable
		 * increments for better syncronization. */
		if (g_lag >= g_updatetime) {
			game_Update();
			draw_Update();
			g_lag -= g_updatetime;
			continue;
		}
		/* Wait until a frame should render */
		{
			unsigned int d = SDL_GetTicks() - g_time;
			if (d < g_frametime) {
				SDL_Delay(g_frametime - d);
			}
		}
		/* Render receives the number and fractions of frames that was
		 * not able to be processed into the game state, thus we can do
		 * prediction of movement through extrapolation. */
		game_Draw((double)g_lag / g_updatetime);
		{
			SDL_Event e;
			while (SDL_PollEvent(&e)) {
				switch (e.type) {
				case SDL_QUIT:
					g_running = 0;
					break;
				case SDL_KEYDOWN:
					game_OnKeydown(&e.key);
					break;
				case SDL_KEYUP:
					game_OnKeyup(&e.key);
					break;
				case SDL_MOUSEBUTTONDOWN:
					game_OnClick(&e.button);
					break;
				case SDL_MOUSEBUTTONUP:
					game_OnRelease(&e.button);
					break;
				default:
					break;
				}
			}
		}
		/* We need to save how much time we spent rendering and reading
		 * input to tell Update how many state steps it must catch-up */
		{
			unsigned int now = SDL_GetTicks();
			g_lag += now - g_time;
			g_time = now;
		}
	}
close:
	game_Close();
	draw_Close();
	return err;
}
