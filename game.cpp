#include "command.hpp"
#include "config.hpp"
#include "draw.hpp"
#include "game.hpp"
#include "geometry.hpp"
#include "state.hpp"
#include "unit.hpp"

static struct unit *game_UnitAt (int x, int y);
static struct point projection_XY (int x, int y);

#define KMOD_MASK 0x0FFF
static unsigned short keymod = 0;

int game_Init (void)
{
	int i, unit_count;
	unit_count = 2;
	g_unit_size = G_UNIT_INITSIZ;
	g_unit = (struct unit*)malloc(sizeof(*g_unit) * g_unit_size);

	/* create heroes */
	for (i = 0; i < unit_count; i++) {
		struct unit u = unit();
		u.flags |= unit::HERO;
		u.pos.x = (i % GRID_LEN) * TILESIZ + TILESIZ / 2;
		u.pos.y = (i / GRID_LEN) * TILESIZ + TILESIZ / 2;
		g_unit[u.id] = u;
		g_unit_len++;
	}

	/* create dragon */
	{
		struct unit u = unit();
		u.flags &= ~unit::HERO;
		u.pos.x = GRID_LEN * TILESIZ / 2.0;
		u.pos.y = GRID_LEN * TILESIZ / 2.0;

		u.spr.x = 0;
		u.spr.y = -24;
		u.spr.w = 100;
		u.spr.h = 100;

		u.body.x = 0;
		u.body.y = 0;
		u.body.r = 32;

		u.dir = -M_PI / 2;
		u.maxspd = 0.2;
		u.maxturnspd = M_PI / 128;

		u.hp = u.maxhp = 400;
		u.atkrange = 64;
		u.atkspd = 1000;
		u.atkanimation = 500;
		u.dmg = 25;

		g_unit[u.id] = u;
		g_unit_len++;
	}
	return 0;
}

void game_Close (void)
{
	free(g_unit);
}

void game_Update (void)
{
	if (g_pause) {
		return;
	}
	for (int i = 0; i < g_unit_len; i++) {
		g_unit[i].Update();
	}
}

void game_Draw (double delta)
{
	int i;

	SDL_SetRenderDrawBlendMode(d_renderer, SDL_BLENDMODE_NONE);
	draw_SetColor(0, 0, 0, 0xFF);
	SDL_RenderClear(d_renderer);

	for (i = 0; i <= GRID_LEN; i++) {
		struct point a, b;
		a.x = b.x = i * TILESIZ;
		b.y = GRID_LEN * TILESIZ;
		a.y = 0;
		a = a * PROJ + ORIGIN;
		b = b * PROJ + ORIGIN;
		draw_SetColor(0x40, 0x40, 0x40, 0xFF);
		SDL_RenderDrawLine(d_renderer, a.x, a.y, b.x, b.y);
	}
	for (i = 0; i <= GRID_LEN; i++) {
		struct point a, b;
		a.y = b.y = i * TILESIZ;
		b.x = GRID_LEN * TILESIZ;
		a.x = 0;
		a = a * PROJ + ORIGIN;
		b = b * PROJ + ORIGIN;
		draw_SetColor(0x40, 0x40, 0x40, 0xFF);
		SDL_RenderDrawLine(d_renderer, a.x, a.y, b.x, b.y);
	}

	for (i = 0; i < g_unit_len; i++) {
		g_unit[i].Draw();
	}

	SDL_RenderPresent(d_renderer);
}

void game_OnKeydown (void *event)
{
	SDL_KeyboardEvent *e = (SDL_KeyboardEvent *)event;
	keymod = e->keysym.mod & KMOD_MASK;
	/* TODO: add camera acceration */
	/* TODO: allow oblique camera movement */
	switch (e->keysym.sym) {
	case SDLK_DOWN:
		ORIGIN.y -= 10;
		break;
	case SDLK_UP:
		ORIGIN.y += 10;
		break;
	case SDLK_RIGHT:
		ORIGIN.x -= 10;
		break;
	case SDLK_LEFT:
		ORIGIN.x += 10;
		break;
	}
}

void game_OnKeyup (void *event)
{
	SDL_KeyboardEvent *e = (SDL_KeyboardEvent *)event;
	keymod = e->keysym.mod & KMOD_MASK;

	switch (e->keysym.sym) {
	case SDLK_p:
		g_pause = !g_pause;
		break;
	case SDLK_s:
		if (keymod == KMOD_NONE) {
			int i;
			for (i = 0; i < g_selection_len; i++) {
				struct unit *u = &g_unit[g_selection[i]];
				if (u != nullptr) {
					u->ClearCmd();
				}
			}
		}
		break;
	}
}

void game_OnClick (void *event)
{
}

void game_OnRelease (void *event)
{
	SDL_MouseButtonEvent *e = (SDL_MouseButtonEvent *)event;
	switch (e->button) {
	case SDL_BUTTON_LEFT:
		{
			if (!(keymod & KMOD_CTRL)) {
				unit::DeselectAll();
			}
			struct unit *u = game_UnitAt(e->x, e->y);
			if (u != nullptr) {
				u->ToggleSelect();
			}
		}
		break;
	case SDL_BUTTON_RIGHT:
		{
			int i;
			for (i = 0; i < g_selection_len; i++) {
				struct unit *u = &g_unit[g_selection[i]];
				/* TODO: interact with map to get terrain
				 * elevation */
				/* TODO: find a path to target before moving */
				if (u == nullptr) {
					continue;
				}
				u->ClearCmd();
				struct point p = projection_XY(e->x, e->y);
				struct unit *u2 = game_UnitAt(e->x, e->y);
				if (u2 == nullptr || u == u2) {
					u->PushCmd(new Move(u->id, p));
				} else {
					u->PushCmd(new Attack(u->id, u2->id));
				}
			}
		}
		break;
	}
}

static struct point projection_XY (int x, int y)
{
	struct point tmp;
	tmp.x = x - ORIGIN.x;
	tmp.y = y - ORIGIN.y;
	return tmp * PROJ_INV;
}

static struct unit *game_UnitAt (int x, int y)
{
	int i;
	/* TODO: that search should be in a list sorted by Y position */
	for (i = 0; i < g_unit_len; i++) {
		struct unit *u = &g_unit[i];
		if (u->UnderCursor(x, y)) {
			return u;
		}
	}
	return nullptr;
}
