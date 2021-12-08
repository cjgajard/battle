#ifndef UNIT_H
#define UNIT_H
#include "config.hpp"

struct sprite {
	int x, y;
	int w, h;
};

struct unit {
	double dir, maxspd;
	unsigned long id;
	struct point pos;
	struct point tar;
	struct sprite spr;
	struct circle col;
	int flags;
};

#define UNIT_ALIVE (1 << 0)
#define UNIT_HEROE (1 << 1)
#define UNIT_SELECTED (1 << 2)
#define UNIT_MOVING (1 << 3)

int unit_IsUnderCursor (struct unit *that, int x, int y);
int unit_IsCollision (struct unit *that, struct unit *u, struct point d);
void unit_Close (struct unit *that);
void unit_Draw (struct unit *that);
void unit_Init (struct unit *that, unsigned long id);
void unit_Move (struct unit *that, struct point d);
void unit_NextMove (struct unit *that, struct point *d);
#endif
