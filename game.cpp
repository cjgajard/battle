#include "config.hpp"
#include "draw.hpp"
#include "game.hpp"
#include "unit.hpp"

static struct unit *game_UnitAt (int x, int y);
static struct point projection_XY (int x, int y);
static void unit_Deselect (struct unit *that);
static void unit_DeselectAll (void);
static void unit_Select (struct unit *that);
static void unit_ToggleSelect (struct unit *that);

#define G_UNIT_INITSIZ 256
static struct unit *g_unit;
static int g_unit_size, g_unit_len;
static unsigned long g_unit_id = 0;

#define G_SELECTION_SIZE 8
static unsigned long g_selection[G_SELECTION_SIZE] = {0};
static int g_selection_len = 0;

#define GRID_LEN 16

#define KMOD_MASK 0x0FFF

static unsigned short keymod = 0;
int g_pause = 0;

int game_Init (void)
{
	int i, unit_count;
	unit_count = 2;
	g_unit_size = G_UNIT_INITSIZ;
	g_unit = (struct unit*)malloc(sizeof(*g_unit) * g_unit_size);

	/* create heroes */
	for (i = 0; i < unit_count; i++) {
		unsigned long id = g_unit_id++;
		struct unit *u = &g_unit[id];
		u->Init(id);
		g_unit_len++;
		u->pos.x = (i % GRID_LEN) * TILESIZ + TILESIZ / 2;
		u->pos.y = (i / GRID_LEN) * TILESIZ + TILESIZ / 2;
	}

	/* create dragon */
	{
		unsigned long id = g_unit_id++;
		struct unit *u = &g_unit[id];
		u->id = id;
		u->pos.x = GRID_LEN * TILESIZ / 2.0;
		u->pos.y = GRID_LEN * TILESIZ / 2.0;

		u->spr.x = 0;
		u->spr.y = -24;
		u->spr.w = 100;
		u->spr.h = 100;

		u->body.x = 0;
		u->body.y = 0;
		u->body.r = 32;

		u->dir = -M_PI / 2;
		u->maxspd = 0.2;

		u->flags = unit::ALIVE;

		g_unit_len++;
	}
	return 0;
}

void game_Close (void)
{
	int i;
	for (i = 0; i < g_unit_len; i++) {
		struct unit *u;
		u = &g_unit[i];
		u->Close();
	}
	free(g_unit);
}

void game_Update (void)
{
	int i, j;
	if (g_pause) {
		return;
	}
	for (i = 0; i < g_unit_len; i++) {
		struct point m;
		struct unit *u;
		u = &g_unit[i];
		if (!(u->flags & unit::ALIVE)) {
			continue;
		}
		if (!(u->flags & unit::MOVING)) {
			continue;
		}
		m = u->MoveStep();
		/* TODO: avoid checking collision between the same pair of
		 * units twice */
		for (j = 0; j < g_unit_len; j++) {
			struct unit *u2;
			if (i == j) {
				continue;
			}
			u2 = &g_unit[j];
			if (!(u2->flags & unit::ALIVE)) {
				continue;
			}
			if (u->Collision(u2, m)) {
				u->flags &= ~unit::MOVING;
			}
		}
		if (u->flags & unit::MOVING) {
			u->Move(m);
		}
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
		a = point_Add(point_MultiplyProj(a, PROJ), ORIGIN);
		b = point_Add(point_MultiplyProj(b, PROJ), ORIGIN);
		draw_SetColor(0x40, 0x40, 0x40, 0xFF);
		SDL_RenderDrawLine(d_renderer, a.x, a.y, b.x, b.y);
	}
	for (i = 0; i <= GRID_LEN; i++) {
		struct point a, b;
		a.y = b.y = i * TILESIZ;
		b.x = GRID_LEN * TILESIZ;
		a.x = 0;
		a = point_Add(point_MultiplyProj(a, PROJ), ORIGIN);
		b = point_Add(point_MultiplyProj(b, PROJ), ORIGIN);
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
	case SDLK_SPACE:
		g_pause = !g_pause;
		break;
	case SDLK_s:
		if (keymod == KMOD_NONE) {
			int i;
			for (i = 0; i < g_selection_len; i++) {
				struct unit *u = &g_unit[g_selection[i]];
				u->flags &= ~unit::MOVING;
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
		if (!(keymod & KMOD_CTRL)) {
			unit_DeselectAll();
		}
		unit_ToggleSelect(game_UnitAt(e->x, e->y));
		break;
	case SDL_BUTTON_RIGHT:
		{
			int i;
			for (i = 0; i < g_selection_len; i++) {
				struct point p;
				struct unit *u = &g_unit[g_selection[i]];
				/* TODO: interact with map to get terrain
				 * elevation */
				/* TODO: find a path to target before moving */
				p = projection_XY(e->x, e->y);
				u->tar = p;
				u->flags |= unit::MOVING;
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
	return point_MultiplyProj(tmp, PROJ_INV);
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
	return NULL;
}

static void unit_Deselect (struct unit *that)
{
	int i, found;
	for (i = 0, found = 0; i < g_selection_len; i++) {
		if (found) {
			/* found cannot be true with i=0 */
			g_selection[i - 1] = g_selection[i];
			continue;
		}
		if (g_selection[i] == that->id) {
			g_unit[that->id].flags &= ~unit::SELECTED;
			found = 1;
		}
	}
	if (found) {
		g_selection_len--;
	}
}

static void unit_DeselectAll (void)
{
	int i;
	for (i = 0; i < g_selection_len; i++) {
		g_unit[g_selection[i]].flags &= ~unit::SELECTED;
	}
	g_selection_len = 0;
}

static void unit_Select (struct unit *that)
{
	/* assumes unit is not already selected */
	if (g_selection_len >= G_SELECTION_SIZE) {
		fprintf(stderr, "max number of selection reached\n");
		return;
	}
	g_selection[g_selection_len++] = that->id;
	that->flags |= unit::SELECTED;
}

/* NOTE: Safe to call with `that` equal to `NULL` */
static void unit_ToggleSelect (struct unit *that)
{
	if (that == NULL) {
		return;
	}
	if (that->flags & unit::SELECTED) {
		unit_Deselect(that);
		return;
	}
	unit_Select(that);
}
