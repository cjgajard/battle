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
	struct point m = u->MoveNext(tar);
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
	tar = _tar;
}
