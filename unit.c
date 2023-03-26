#include "battle.h"
#include "draw.h"

static struct point unit_Projection (struct unit *that)
{
	return point_add(projection_of(PROJ, that->pos), ORIGIN);
}

static void unit_get_rect_sprite (struct unit *that, SDL_Rect *dst)
{
	struct point p = unit_Projection(that);
	dst->x = p.x - that->spr.w / 2 + that->spr.x;
	dst->y = p.y - that->spr.h / 2 + that->spr.y;
	dst->w = that->spr.w;
	dst->h = that->spr.h;
}

int unit_collision (struct unit *u, struct unit *other, struct point m)
{
	struct circle c, c2;
	c2.x = u->pos.x + u->body.x + m.x;
	c2.y = u->pos.y + u->body.y + m.y;
	c2.r = u->body.r;

	c2.x = other->pos.x + other->body.x;
	c2.y = other->pos.y + other->body.y;
	c2.r = other->body.r;
	return circle_collision(c, c2);
}

int unit_under_cursor (struct unit *u, int x, int y)
{
	SDL_Rect r;
	unit_get_rect_sprite(u, &r);
	return (x >= r.x && x < r.x+r.w) && (y >= r.y && y < r.y+r.h);
}

void unit_draw (struct unit *u)
{
	unsigned char r, g, b;
	if (!(u->flags & UNIT_ALIVE)) {
		return;
	}
	/* collision-hitbox */
	{
		struct point p, tmp, siz;
		g = 0xFF;
		if (u->flags & UNIT_SELECTED) {
			r = b = 0;
		} else {
			r = b = 0xFF;
		}
		if (g_pause) {
			r = 255 - r;
			g = 255 - g;
			b = 255 - b;
		}

		p.x = u->body.x;
		p.y = u->body.y;
		p = point_add(u->pos, p);
		p = point_add(projection_of(PROJ, p), ORIGIN);

		/* TODO: understand reason of parameters of siz */
		tmp = projection_of(PROJ, PROJ.x);
		siz.x = u->body.r * tmp.x;
		tmp = projection_of(PROJ, PROJ.y);
		siz.y = u->body.r * tmp.y;
		/* draw_SetColor(r, g, b, 0xFF); */
		SDL_SetRenderDrawColor(d_renderer, r, g, b, 0xFF);
		aaellipseRGBA(d_renderer, p.x, p.y, siz.x, siz.y, r, g, b, 0xFF);
	}
	/* direction */
	{
		struct point start, end;

		g = 0xFF;
		if (u->flags & UNIT_SELECTED) {
			r = b = 0;
		} else {
			r = b = 0xFF;
		}

		start = point_add(projection_of(PROJ, u->pos), ORIGIN);
		end.x = u->pos.x + u->body.r * cos(u->dir);
		end.y = u->pos.y + u->body.r * sin(u->dir);
		end = point_add(projection_of(PROJ, end), ORIGIN);

		/* draw_SetColor(r, g, b, 0xFF); */
		SDL_SetRenderDrawColor(d_renderer, r, g, b, 0xFF);
		SDL_RenderDrawLine(d_renderer, start.x, start.y, end.x, end.y);
	}
	/* sprite box */
	{
		SDL_Rect img;
		unit_get_rect_sprite(u, &img);
		if (u->atktime) {
			g = b = 0;
			r = 0xFF;
		} else {
			r = g = b = 0x80;
		}
		/* draw_SetColor(r, g, b, 0xFF); */
		SDL_SetRenderDrawColor(d_renderer, r, g, b, 0xFF);
		SDL_RenderDrawRect(d_renderer, &img);
	}
	/* health bar */
	{
		SDL_Rect bar;
		double x = u->hp / u->maxhp;
		unit_get_rect_sprite(u, &bar);
		bar.h = 4;
		bar.y -= 2 * bar.h;
		bar.w *= x;
		/* draw_SetColor((1 - x) * 0xFF, x * 0xFF, 0, 0xFF); */
		SDL_SetRenderDrawColor(d_renderer, (1-x) * 0xFF, x * 0xFF, 0, 0xFF);
		SDL_RenderFillRect(d_renderer, &bar);
	}
}

void unit_init (struct unit *u)
{
	u->id = g_unit_id++;
	u->pos.x = 0;
	u->pos.y = 0;

	u->spr.x = 0;
	u->spr.y = -16;
	u->spr.w = 32;
	u->spr.h = 48;

	u->body.x = 0;
	u->body.y = 0;
	u->body.r = 8;

	u->maxspd = 1.0;
	u->maxturnspd = M_PI / (4 * u->body.r);

	u->atkrange = 8;
	u->atkspeed = 500;
	u->atkduration = 200;
	u->atkdmg = 10;
	u->atktime = 0;

	u->hp = u->maxhp = 100;

	u->flags = UNIT_ALIVE;
}

void unit_update (struct unit *u)
{
	struct command *c;
	if (!(u->flags & UNIT_ALIVE)) {
		return;
	}
	c = unit_cmd(u);
	if (c != NULL) {
		if (command_next(c)) {
			command_halt(c);
			unit_cmd_pop(u);
		} else {
			command_apply(c);
		}
	}
	if (!(u->flags & UNIT_ACTIVE)) {
		struct unit *enemy = unit_get_enemy_closest(u);
		if (enemy != NULL) {
			struct point v;
			v = point_sub(enemy->pos, u->pos);
			unit_turn(u, unit_turn_next(u, v));
		}
	}
	if (u->hp <= 0) {
		u->flags &= ~UNIT_ALIVE;
	}
}

void unit_move (struct unit *u, struct point mv)
{
	u->pos = point_add(u->pos, mv);
}

struct point unit_move_next (struct unit *u, struct point t)
{
	struct point d = point_sub(t , u->pos);
	double av = point_get_value(d);
	if (av > u->maxspd) {
		d.x *= u->maxspd / av;
		d.y *= u->maxspd / av;
	}
	return d;
}

void unit_turn (struct unit *u, angle_t delta)
{
	double d = u->dir + delta;
	if (d < 0)
		d += 2 * M_PI;
	if (d > 2 * M_PI)
		d -= 2 * M_PI;
	u->dir = d;
}

angle_t unit_turn_next (struct unit *u, struct point v)
{
	angle_t a = angle_sub(point_get_angle(v), u->dir);
	if (a > u->maxturnspd) {
		a = u->maxturnspd;
	}
	if (a < -u->maxturnspd) {
		a = -u->maxturnspd;
	}
	return a;
}

void unit_attack (struct unit *u, struct unit *target)
{
	if (u == NULL) {
		return;
	}
	target->hp -= u->atkdmg;
}

void unit_select_unset (struct unit *u)
{
	int i, found = 0;
	for (i = 0; i < g_selection_len; i++) {
		if (found) {
			/* found cannot be true with i=0 */
			g_selection[i - 1] = g_selection[i];
			continue;
		}
		if (g_selection[i] == u->id) {
			g_unit[u->id].flags &= ~UNIT_SELECTED;
			found = 1;
		}
	}
	if (found) {
		g_selection_len--;
	}
}

void unit_select_none (void)
{
	int i;
	for (i = 0; i < g_selection_len; i++) {
		g_unit[g_selection[i]].flags &= ~UNIT_SELECTED;
	}
	g_selection_len = 0;
}

void unit_select_set (struct unit *u)
{
	/* assumes unit is not already selected */
	if (g_selection_len >= G_SELECTION_SIZ) {
		fprintf(stderr, "max number of selection reached\n");
		return;
	}
	g_selection[g_selection_len++] = u->id;
	u->flags |= UNIT_SELECTED;
}

void unit_select_toggle (struct unit *u)
{
	if (u->flags & UNIT_SELECTED) {
		unit_select_unset(u);
	} else {
		unit_select_set(u);
	}
}

struct unit *unit_get_enemy_closest (struct unit *u)
{
	int i;
	struct unit *current = NULL;
	double min = DBL_MAX;
	for (i = 0; i < g_unit_len; i++) {
		double d;
		struct unit *enemy = &g_unit[i];
		if (u == enemy || u->id == enemy->id) {
			continue;
		}
		if (!(enemy->flags & UNIT_ALIVE)) {
			continue;
		}
		if (!((u->flags & UNIT_HERO) ^ (enemy->flags & UNIT_HERO))) {
			continue;
		}
		d = abs(point_get_value(point_sub(enemy->pos, u->pos)));
		if (current == NULL || d < min) {
			min = d;
			current = enemy;
		}
	}
	return current;
}

struct command *unit_cmd (struct unit *u)
{
	if (u->cmd_len > 0 && u->cmd_len <= UNIT_CMD_SIZ) {
		return &u->cmd[u->cmd_len - 1];
	}
	return NULL;
}

void unit_cmd_push (struct unit *u, struct command c)
{
	if (u->cmd_len >= UNIT_CMD_SIZ) {
		return;
	}
	u->cmd[u->cmd_len++] = c;
}

void unit_cmd_pop (struct unit *u)
{
	u->cmd_len--;
}

void unit_cmd_clear (struct unit *u)
{
	u->cmd_len = 0;
}
