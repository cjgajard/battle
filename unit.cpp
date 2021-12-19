#include "draw.hpp"
#include "state.hpp"
#include "unit.hpp"

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
	return +d < r + c->r;
}

bool unit::Collision (struct unit *u, struct point m)
{
	struct circle c = {
		pos.x + body.x + m.x,
		pos.y + body.y + m.y,
		body.r,
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
	if (!(flags & unit::ALIVE)) {
		return;
	}
	/* collision-hitbox */
	{
		struct point p, tmp, siz;

		g = 0xFF;
		if (flags & unit::SELECTED) {
			r = b = 0;
		} else {
			r = b = 0xFF;
		}
		if (g_pause) {
			r = 255 - r;
			g = 255 - g;
			b = 255 - b;
		}

		p.x = body.x;
		p.y = body.y;
		p = pos + p;
		p = p * PROJ + ORIGIN;

		/* TODO: understand reason of parameters of siz */
		tmp = PROJ.x * PROJ;
		siz.x = body.r * tmp.x;
		tmp = PROJ.y * PROJ;
		siz.y = body.r * tmp.y;
		draw_SetColor(r, g, b, 0xFF);
		aaellipseRGBA(d_renderer, p.x, p.y, siz.x, siz.y, r, g, b, 0xFF);
	}
	/* direction */
	{
		struct point start, end;

		g = 0xFF;
		if (flags & unit::SELECTED) {
			r = b = 0;
		} else {
			r = b = 0xFF;
		}

		start = pos * PROJ + ORIGIN;
		end.x = pos.x + body.r * cos(dir);
		end.y = pos.y + body.r * sin(dir);
		end = end * PROJ + ORIGIN;

		draw_SetColor(r, g, b, 0xFF);
		SDL_RenderDrawLine(d_renderer, start.x, start.y, end.x, end.y);
	}
	/* sprite box */
	{
		SDL_Rect img;
		unit_SpriteRect(this, &img);
		draw_SetColor(0x80, 0x80, 0x80, 0xFF);
		SDL_RenderDrawRect(d_renderer, &img);
	}
	/* health bar */
	{
		SDL_Rect bar;
		double x = hp / maxhp;
		unit_SpriteRect(this, &bar);
		bar.h = 4;
		bar.y -= 2 * bar.h;
		bar.w *= x;
		draw_SetColor((1 - x) * 0xFF, x * 0xFF, 0, 0xFF);
		SDL_RenderFillRect(d_renderer, &bar);
	}
}

void unit::Init (unitid_t _id)
{
	id = _id;
	pos.x = 0;
	pos.y = 0;

	spr.x = 0;
	spr.y = -16;
	spr.w = 32;
	spr.h = 48;

	maxspd = 1.0;
	maxturnspd = M_PI / 128;

	hp = maxhp = 100;

	body.x = 0;
	body.y = 0;
	body.r = 8;

	flags = unit::HERO | unit::ALIVE;
}

void unit::Update ()
{
	if (!(flags & unit::ALIVE)) {
		return;
	}
	Command *c = Cmd();
	if (c != NULL) {
		if (c->Next()) {
			c->Apply();
		} else {
			c->End();
			PopCmd();
		}
	}
	if (!(flags & unit::ACTIVE)) {
		struct unit *u = ClosestEnemy();
		if (u != NULL) {
			Turn(TurnNext(u->pos - pos));
		}
	}
}

void unit::Move (struct point mv)
{
	pos += mv;
}

struct point unit::MoveNext (struct point t)
{
	struct point d = t - pos;
	double av = +d;
	if (av > maxspd) {
		d.x *= maxspd / av;
		d.y *= maxspd / av;
	}
	return d;
}

void unit::Turn (angle_t delta)
{
	double d = dir + delta.a;
	if (d < 0)
		d += 2 * M_PI;
	if (d > 2 * M_PI)
		d -= 2 * M_PI;
	dir = d;
}

angle_t unit::TurnNext (struct point v)
{
	angle_t a = angle_t(v) - angle_t(dir);
	if (a.a > maxturnspd) {
		a.a = maxturnspd;
	}
	if (a.a < -maxturnspd) {
		a.a = -maxturnspd;
	}
	return a;
}

void unit::Deselect ()
{
	bool found = false;
	for (int i = 0; i < g_selection_len; i++) {
		if (found) {
			/* found cannot be true with i=0 */
			g_selection[i - 1] = g_selection[i];
			continue;
		}
		if (g_selection[i] == id) {
			g_unit[id].flags &= ~unit::SELECTED;
			found = true;
		}
	}
	if (found) {
		g_selection_len--;
	}
}

void unit::DeselectAll (void)
{
	int i;
	for (i = 0; i < g_selection_len; i++) {
		g_unit[g_selection[i]].flags &= ~unit::SELECTED;
	}
	g_selection_len = 0;
}

void unit::Select ()
{
	/* assumes unit is not already selected */
	if (g_selection_len >= G_SELECTION_SIZE) {
		fprintf(stderr, "max number of selection reached\n");
		return;
	}
	g_selection[g_selection_len++] = id;
	flags |= unit::SELECTED;
}

void unit::ToggleSelect ()
{
	if (flags & unit::SELECTED) {
		Deselect();
		return;
	}
	Select();
}

struct unit *unit::ClosestEnemy ()
{
	struct unit *current = NULL;
	double min = 0;
	for (int i = 0; i < g_unit_len; i++) {
		struct unit *u = &g_unit[i];
		if (this == u) {
			continue;
		}
		if (!(u->flags & unit::ALIVE)) {
			continue;
		}
		if (!((flags & unit::HERO) ^ (u->flags & unit::HERO))) {
			continue;
		}
		double d = abs(+(u->pos - pos));
		if (current == NULL || d < min) {
			min = d;
			current = u;
		}
	}
	return current;
}

Command *unit::Cmd (void)
{
	if (cmd_len > 0 && cmd_len <= unit_CMD_SIZ) {
		return cmd[cmd_len - 1];
	}
	return NULL;
}

void unit::PushCmd (Command *c)
{
	if (cmd_len >= unit_CMD_SIZ) {
		return;
	}
	cmd[cmd_len++] = c;
}

void unit::PopCmd (void)
{
	if (cmd_len == 0) {
		return;
	}
	Command *c = cmd[0];
	delete c;
	cmd_len--;
}

void unit::ClearCmd (void)
{
	// Command *c;
	// while ((c = Cmd()) != NULL)
	while (cmd_len > 0) {
		Command *c = Cmd();
		if (c != NULL) {
			c->Halt();
		}
		PopCmd();
	}
}
