#include "command.hpp"
#include "state.hpp"
#include "unit.hpp"

inline bool isZero (double n)
{
	const double e /* epsilon */ = 1e-5;
	return n >= -e && n <= e;
}

inline bool isZero (struct point p)
{
	return isZero(p.x) && isZero(p.y);
}

bool Move::Next ()
{
	struct unit *u = &g_unit[uid];
	struct point m = u->MoveNext(target);
	if (isZero(m)) {
		return false;
	}

	/* TODO: avoid checking collision between the same pair of
	 * units twice */
	for (int j = 0; j < g_unit_len; j++) {
		struct unit *u2 = &g_unit[j];
		if (u == u2) {
			continue;
		}
		if (!(u2->flags & unit::ALIVE)) {
			continue;
		}
		if (u->Collision(u2, m)) {
			return false;
		}
	}
	mv = m;
	a = u->TurnNext(mv);
	return true;
}

void Move::Apply ()
{
	struct unit *u = &g_unit[uid];
	u->flags |= unit::MOVING;
	u->Turn(a);
	u->Move(mv);
}

void Move::Halt ()
{
	struct unit *u = &g_unit[uid];
	u->flags &= ~unit::MOVING;
}

Move::Move (unitid_t _uid, struct point _tar)
{
	uid = _uid;
	target = _tar;
}

bool Attack::Next (void)
{
	struct unit *u = &g_unit[uid];
	struct unit *u2 = &g_unit[target];
	if (u == nullptr || u2 == nullptr) {
		return false;
	}
	if (u == u2) {
		return false;
	}
	if (!(u2->flags & unit::ALIVE)) {
		return false;
	}
	double distance = +(u2->pos - u->pos) - u->body.r - u2->body.r;
	if ((outofrange = distance >= u->atkrange)) {
		mv.target = u2->pos;
		return mv.Next();
	}
	return true;
}

void Attack::Apply (void)
{
	struct unit *u = &g_unit[uid];
	struct unit *u2 = &g_unit[target];
	if (u == nullptr || u2 == nullptr) {
		return;
	}
	if (outofrange) {
		u->flags &= ~unit::ATTACKING;
		mv.Apply();
		return;
	}
	if (g_time - lastatktime >= u->atkspd) {
		u->flags |= unit::ATTACKING;
		lastatktime = g_time;
	}
	if ((u->flags & unit::ATTACKING) && (g_time - lastatktime >= u->atkanimation)) {
		u->Attack(u2);
		u->flags &= ~unit::ATTACKING;
	}
}

void Attack::Halt (void)
{
	struct unit *u = &g_unit[uid];
	u->flags &= ~unit::ATTACKING;
	u->flags &= ~unit::MOVING;
}

Attack::Attack (unitid_t a, unitid_t b)
{
	uid = a;
	target = b;
	lastatktime = g_time;
	mv.uid = uid;
}
