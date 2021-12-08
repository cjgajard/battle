#include "unit.hpp"
#include "draw.hpp"
static void unit_SpriteRect (struct unit *that, SDL_Rect *dst);
static struct point unit_Projection (struct unit *that);

void unit_Init (struct unit *that, unsigned long id)
{
	that->id = id;
	that->pos.x = 0;
	that->pos.y = 0;

	that->spr.x = 0;
	that->spr.y = -16;
	that->spr.w = 32;
	that->spr.h = 48;

	that->maxspd = 1.0;

	that->col.x = 0;
	that->col.y = 0;
	that->col.r = 8;

	that->flags = UNIT_ALIVE;
}

void unit_Close (struct unit *that)
{
}

void unit_Draw (struct unit *that)
{
	unsigned char r, g, b;
	if (!(that->flags & UNIT_ALIVE)) {
		return;
	}
	/* sprite box */
	{
		SDL_Rect img;
		unit_SpriteRect(that, &img);
		draw_SetColor(0x80, 0x80, 0x80, 0xFF);
		SDL_RenderDrawRect(d_renderer, &img);
	}
	/* collision-hitbox */
	{
		struct point p, tmp, siz;

		g = 0xFF;
		if (that->flags & UNIT_SELECTED) {
			r = b = 0;
		} else {
			r = b = 0xFF;
		}
		if (g_pause) {
			r = 255 - r;
			g = 255 - g;
			b = 255 - b;
		}

		p.x = that->col.x;
		p.y = that->col.y;
		p = point_Add(that->pos, p);
		p = point_Add(point_MultiplyProj(p, PROJ), ORIGIN);

		/* TODO: understand reason of parameters of siz */
		tmp = point_MultiplyProj(PROJ.x, PROJ);
		siz.x = that->col.r * tmp.x;
		tmp = point_MultiplyProj(PROJ.y, PROJ);
		siz.y = that->col.r * tmp.y;
		draw_SetColor(r, g, b, 0xFF);
		aaellipseRGBA(d_renderer, p.x, p.y, siz.x, siz.y, r, g, b, 0xFF);
	}
	/* direction */
	{
		struct point start, end;

		g = 0xFF;
		if (that->flags & UNIT_SELECTED) {
			r = b = 0;
		} else {
			r = b = 0xFF;
		}

		start = point_Add(point_MultiplyProj(that->pos, PROJ), ORIGIN);
		end.x = that->pos.x + that->col.r * cos(that->dir);
		end.y = that->pos.y + that->col.r * sin(that->dir);
		end = point_Add(point_MultiplyProj(end, PROJ), ORIGIN);

		draw_SetColor(r, g, b, 0xFF);
		SDL_RenderDrawLine(d_renderer, start.x, start.y, end.x, end.y);
	}
}

static struct point unit_Projection (struct unit *that)
{
	return point_Add(point_MultiplyProj(that->pos, PROJ), ORIGIN);
}

static void unit_SpriteRect (struct unit *that, SDL_Rect *dst)
{
	struct point p = unit_Projection(that);
	dst->x = p.x - that->spr.w / 2 + that->spr.x;
	dst->y = p.y - that->spr.h / 2 + that->spr.y;
	dst->w = that->spr.w;
	dst->h = that->spr.h;
}

int unit_IsUnderCursor (struct unit *that, int x, int y)
{
	SDL_Rect r;
	unit_SpriteRect(that, &r);
	return (x >= r.x && x < r.x+r.w) && (y >= r.y && y < r.y+r.h);
}

void unit_NextMove (struct unit *that, struct point *d)
{
	double av;
	d->x = that->tar.x - that->pos.x;
	d->y = that->tar.y - that->pos.y;
	av = sqrt(d->x * d->x + d->y * d->y);
	if (av > that->maxspd) {
		d->x *= that->maxspd / av;
		d->y *= that->maxspd / av;
	}
}

void unit_Move (struct unit *that, struct point d)
{
	if (!d.x || !d.y) {
		that->flags &= ~UNIT_MOVING;
		return;
	}
	that->dir = atan2(d.y, d.x);
	that->pos.x += d.x;
	that->pos.y += d.y;
}

int unit_IsCollision (struct unit *that, struct unit *u, struct point d)
{
	double dx, dy;
	dx = (that->pos.x + that->col.x + d.x) - (u->pos.x + u->col.x);
	dy = (that->pos.y + that->col.y + d.y) - (u->pos.y + u->col.y);
	return sqrt(dx * dx + dy * dy) < that->col.r + u->col.r;
}
