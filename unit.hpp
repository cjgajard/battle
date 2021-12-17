#pragma once
#include "geometry.hpp"

struct sprite {
	int x, y;
	int w, h;
};

struct unit {
	double dir, maxspd;
	double maxturnspd;
	unsigned long id;
	struct point pos;
	struct point tar;
	struct sprite spr;
	struct circle body;
	unsigned int flags;

	enum {
		ALIVE = (1 << 0),
		HEROE = (1 << 1),
		SELECTED = (1 << 2),
		MOVING = (1 << 3),
	};

	bool Collision (struct unit *u, struct point d);
	bool UnderCursor (int x, int y);
	void Close (void);
	void Draw (void);
	void Init (unsigned long id);
	void Move (struct point v);
	void Turn (double delta);
	void Update (void);

	double TurnStep (struct point target);
	struct point MoveStep ();
};
