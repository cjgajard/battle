#include "unit.hpp"
#include "draw.hpp"

static struct point unit_Projection (struct unit *that)
{
	return that->pos * PROJ + ORIGIN;
}

static void unit_SpriteRect (struct unit *that, SDL_Rect *dst)
{
	struct point p = unit_Projection(that);
	dst->x = p.x - that->spr.w / 2 + that->spr.x;
	dst->y = p.y - that->spr.h / 2 + that->spr.y;
	dst->w = that->spr.w;
	dst->h = that->spr.h;
}

bool circle::Collision (struct circle *c)
{
	struct point d = (struct point)*this - (struct point)*c;
	return +d < this->r + c->r;
}

bool unit::Collision (struct unit *u, struct point m)
{
	struct circle c = {
		this->pos.x + this->body.x + m.x,
		this->pos.y + this->body.y + m.y,
		this->body.r,
	};

	struct circle c2 = {
		u->pos.x + u->body.x,
		u->pos.y + u->body.y,
		u->body.r,
	};

	return c.Collision(&c2);
}

bool unit::UnderCursor (int x, int y)
{
	SDL_Rect r;
	unit_SpriteRect(this, &r);
	return (x >= r.x && x < r.x+r.w) && (y >= r.y && y < r.y+r.h);
}

void unit::Close ()
{
}

void unit::Draw ()
{
	unsigned char r, g, b;
	if (!(this->flags & unit::ALIVE)) {
		return;
	}
	/* sprite box */
	{
		SDL_Rect img;
		unit_SpriteRect(this, &img);
		draw_SetColor(0x80, 0x80, 0x80, 0xFF);
		SDL_RenderDrawRect(d_renderer, &img);
	}
	/* collision-hitbox */
	{
		struct point p, tmp, siz;

		g = 0xFF;
		if (this->flags & unit::SELECTED) {
			r = b = 0;
		} else {
			r = b = 0xFF;
		}
		if (g_pause) {
			r = 255 - r;
			g = 255 - g;
			b = 255 - b;
		}

		p.x = this->body.x;
		p.y = this->body.y;
		p = this->pos + p;
		p = p * PROJ + ORIGIN;

		/* TODO: understand reason of parameters of siz */
		tmp = PROJ.x * PROJ;
		siz.x = this->body.r * tmp.x;
		tmp = PROJ.y * PROJ;
		siz.y = this->body.r * tmp.y;
		draw_SetColor(r, g, b, 0xFF);
		aaellipseRGBA(d_renderer, p.x, p.y, siz.x, siz.y, r, g, b, 0xFF);
	}
	/* direction */
	{
		struct point start, end;

		g = 0xFF;
		if (this->flags & unit::SELECTED) {
			r = b = 0;
		} else {
			r = b = 0xFF;
		}

		start = this->pos * PROJ + ORIGIN;
		end.x = this->pos.x + this->body.r * cos(this->dir);
		end.y = this->pos.y + this->body.r * sin(this->dir);
		end = end * PROJ + ORIGIN;

		draw_SetColor(r, g, b, 0xFF);
		SDL_RenderDrawLine(d_renderer, start.x, start.y, end.x, end.y);
	}
}

void unit::Init (unsigned long id)
{
	this->id = id;
	this->pos.x = 0;
	this->pos.y = 0;

	this->spr.x = 0;
	this->spr.y = -16;
	this->spr.w = 32;
	this->spr.h = 48;

	this->maxspd = 1.0;
	this->maxturnspd = M_PI / 128;

	this->body.x = 0;
	this->body.y = 0;
	this->body.r = 8;

	this->flags = unit::ALIVE;
}

void unit::Move (struct point v)
{
	if (!v.x || !v.y) {
		this->flags &= ~unit::MOVING;
		return;
	}
	this->pos.x += v.x;
	this->pos.y += v.y;
}

void unit::Turn (double delta)
{
	double dir = this->dir + delta;
	if (dir < 0)
		dir += 2 * M_PI;
	if (dir > 2 * M_PI)
		dir -= 2 * M_PI;
	this->dir = dir;
}

double unit::TurnStep (struct point v)
{
	angle_t a = angle_t(v) - angle_t(this->dir);
	if (a.a > this->maxturnspd) {
		a = angle_t(this->maxturnspd);
	}
	if (a.a < -this->maxturnspd) {
		a = angle_t(-this->maxturnspd);
	}
	return a.a;
}

struct point unit::MoveStep ()
{
	struct point d = this->tar - this->pos;
	double av = +d;
	if (av > this->maxspd) {
		d.x *= this->maxspd / av;
		d.y *= this->maxspd / av;
	}
	return d;
}
