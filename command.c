#include <stdlib.h>
#include "battle.h"

int is_zero (double n)
{
	const double e /* epsilon */ = 1e-5;
	return n >= -e && n <= e;
}

static int point_is_zero (struct point p)
{
	return is_zero(p.x) && is_zero(p.y);
}

int move_next (struct move *cmd)
{
	int j;
	struct unit *u = &g_unit[cmd->uid];
	struct point m = unit_move_next(u, cmd->target);
	if (point_is_zero(m)) {
		return 1;
	}

	/* TODO: avoid checking collision between the same pair of
	 * units twice */
	for (j = 0; j < g_unit_len; j++) {
		struct unit *u2 = &g_unit[j];
		if (u == u2) {
			continue;
		}
		if (!(u2->flags & UNIT_ALIVE)) {
			continue;
		}
		if (unit_collision(u, u2, m)) {
			return 1;
		}
	}
	cmd->mv = m;
	cmd->a = unit_turn_next(u, m);
	return 0;
}

void move_apply (struct move *cmd)
{
	struct unit *u = &g_unit[cmd->uid];
	u->flags |= UNIT_MOVING;
	unit_turn(u, cmd->a);
	unit_move(u, cmd->mv);
}

void move_halt (struct move *cmd)
{
	struct unit *u = &g_unit[cmd->uid];
	u->flags &= ~UNIT_MOVING;
}

void command_move_init (struct command *c, unitid_t uid, struct point target)
{
	c->type = COMMAND_MOVE;
	c->data.mv.uid = uid;
	c->data.mv.target = target;
}

int attack_next (struct attack *cmd)
{
	double distance;
	struct unit *u = &g_unit[cmd->uid];
	struct unit *u2 = &g_unit[cmd->target];
	if (u == NULL || u2 == NULL) {
		return 1;
	}
	if (u == u2) {
		return 1;
	}
	if (!(u2->flags & UNIT_ALIVE)) {
		return 1;
	}
	distance = point_get_value(point_sub(u2->pos, u->pos)) - u->body.r - u2->body.r;
	if ((cmd->outofrange = distance >= u->atkrange)) {
		cmd->mv.target = u2->pos;
		return move_next(&cmd->mv);
	}
	return 0;
}

void attack_apply (struct attack *cmd)
{
	struct unit *u = &g_unit[cmd->uid];
	struct unit *u2 = &g_unit[cmd->target];
	if (u == NULL || u2 == NULL) {
		return;
	}
	if (cmd->outofrange) {
		u->flags &= ~UNIT_ATTACKING;
		move_apply(&cmd->mv);
		return;
	}
	u->flags |= UNIT_ATTACKING;
	unit_turn(u, unit_turn_next(u, point_sub(u2->pos , u->pos)));
	switch (cmd->state) {
	case 0: /* animation */
		u->atktime = g_time - cmd->lastatktime;
		if (u->atktime >= u->atkduration) {
			cmd->state = 1;
		}
		break;
	case 1: /* attack */
		unit_attack(u,u2);
		u->atktime = 0;
		cmd->state = 2;
		break;
	case 2: /* cooldown */
		if (g_time - cmd->lastatktime >= u->atkspeed) {
			cmd->lastatktime = g_time;
			cmd->state = 0;
		}
		break;
	}
}

void attack_halt (struct attack *cmd)
{
	struct unit *u = &g_unit[cmd->uid];
	u->flags &= ~UNIT_ATTACKING;
	u->flags &= ~UNIT_MOVING;
	u->atktime = 0;
}

void command_attack_init (struct command *c, unitid_t a, unitid_t b)
{
	c->type = COMMAND_ATTACK;
	c->data.atk.uid = c->data.atk.mv.uid = a;
	c->data.atk.target = b;
	c->data.atk.lastatktime = g_time;
	c->data.atk.state = 0;
}

int command_next (struct command *c)
{
	switch (c->type) {
	case COMMAND_MOVE: return move_next(&c->data.mv);
	case COMMAND_ATTACK: return attack_next(&c->data.atk);
	default: return 1;
	}
}

void command_apply (struct command *c)
{
	switch (c->type) {
	case COMMAND_MOVE:
		move_apply(&c->data.mv);
		break;
	case COMMAND_ATTACK:
		attack_apply(&c->data.atk);
		break;
	default:
		break;
	}
}

void command_halt (struct command *c)
{
	switch (c->type) {
	case COMMAND_MOVE:
		move_halt(&c->data.mv);
		break;
	case COMMAND_ATTACK:
		attack_halt(&c->data.atk);
		break;
	default:
		break;
	}
}
