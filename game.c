#include "battle.h"
#include "draw.h"

unsigned g_pause = 0;
unsigned int g_time = 0;

struct unit g_unit[G_UNIT_SIZ];
int g_unit_len;
unitid_t g_unit_id = 0;

unitid_t g_selection[G_SELECTION_SIZ] = {0};
int g_selection_len = 0;

static struct unit *game_UnitAt (int x, int y);
static struct point projection_XY (int x, int y);

#define KMOD_MASK 0x0FFF
static unsigned short keymod = 0;

int game_Init (void)
{
	int i, unit_count;
	unit_count = 2;

	/* create heroes */
	for (i = 0; i < unit_count; i++) {
		struct unit u;
		unit_init(&u);
		u.flags |= UNIT_HERO;
		u.pos.x = (i % GRID_LEN) * 2 * TILESIZ + TILESIZ / 2;
		u.pos.y = (i / GRID_LEN) * TILESIZ + 4 * TILESIZ;
		g_unit[u.id] = u;
		g_unit_len++;
	}

	/* create dragon */
	{
		struct unit u;
		unit_init(&u);
		u.flags &= ~UNIT_HERO;
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
		u.maxturnspd = M_PI / (4 * u.body.r);

		u.hp = u.maxhp = 400;
		u.atkrange = 64;
		u.atkspeed = 1000;
		u.atkduration = 500;
		u.atkdmg = 25;

		g_unit[u.id] = u;
		g_unit_len++;
	}
	return 0;
}

void game_Close (void)
{
}

void game_Update (void)
{
	int i;
	if (g_pause) {
		return;
	}
	for (i = 0; i < g_unit_len; i++) {
		unit_update(&g_unit[i]);
	}
}

static void draw_SetColor (int r, int g, int b, int a)
{
	if (g_pause) {
		SDL_SetRenderDrawColor(d_renderer, 0xFF - r, 0xFF - g, 0xFF - b, a);
	} else {
		SDL_SetRenderDrawColor(d_renderer, r, g, b, a);
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
		a = point_add(projection_of(PROJ, a), ORIGIN);
		b = point_add(projection_of(PROJ, b), ORIGIN);
		draw_SetColor(0x40, 0x40, 0x40, 0xFF);
		SDL_RenderDrawLine(d_renderer, a.x, a.y, b.x, b.y);
	}
	for (i = 0; i <= GRID_LEN; i++) {
		struct point a, b;
		a.y = b.y = i * TILESIZ;
		b.x = GRID_LEN * TILESIZ;
		a.x = 0;
		a = point_add(projection_of(PROJ, a), ORIGIN);
		b = point_add(projection_of(PROJ, b), ORIGIN);
		draw_SetColor(0x40, 0x40, 0x40, 0xFF);
		SDL_RenderDrawLine(d_renderer, a.x, a.y, b.x, b.y);
	}
	for (i = 0; i < g_unit_len; i++) {
		unit_draw(&g_unit[i]);
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
				if (u != NULL) {
					unit_cmd_clear(u);
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
			struct unit *u;
			if (!(keymod & KMOD_CTRL)) {
				unit_select_none();
			}
			u = game_UnitAt(e->x, e->y);
			if (u != NULL) {
				unit_select_toggle(u);
			}
		}
		break;
	case SDL_BUTTON_RIGHT:
		{
			int i;
			for (i = 0; i < g_selection_len; i++) {
				struct point p;
				struct unit *u2;
				struct unit *u = &g_unit[g_selection[i]];
				/* TODO: find a path to target before moving */
				unit_cmd_clear(u);
				p = projection_XY(e->x, e->y);
				u2 = game_UnitAt(e->x, e->y);
				if (u2 == NULL || u == u2) {
					struct command cmd;
					command_move_init(&cmd, u->id, p);
					unit_cmd_push(u, cmd);
				} else {
					struct command cmd;
					command_attack_init(&cmd, u->id, u2->id);
					unit_cmd_push(u, cmd);
				}
			}
		}
		break;
	}
}

static struct point projection_XY (int x, int y)
{
	struct point tmp;
	/* TODO: interact with map to get terrain elevation */
	tmp.x = x - ORIGIN.x;
	tmp.y = y - ORIGIN.y;
	return projection_of(PROJ_INV, tmp);
}

static struct unit *game_UnitAt (int x, int y)
{
	int i;
	/* TODO: that search should be in a list sorted by Y position */
	for (i = 0; i < g_unit_len; i++) {
		struct unit *u = &g_unit[i];
		if (unit_under_cursor(u, x, y)) {
			return u;
		}
	}
	return NULL;
}
