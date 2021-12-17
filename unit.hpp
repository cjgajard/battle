#pragma once
#include "geometry.hpp"

struct sprite {
	int x, y;
	int w, h;
};

struct unit {
	unsigned long id;
	unsigned int flags;
	double dir, maxspd;
	double maxturnspd;
	struct point pos;
	struct point tar;
	struct sprite spr;
	struct circle body;

	enum {
		HERO = (1 << 0),
		ALIVE = (1 << 1),
		SELECTED = (1 << 2),
		MOVING = (1 << 3),
	};

	bool Collision (struct unit *u, struct point d);
	bool UnderCursor (int x, int y);
	void Close (void);
	void Draw (void);
	void Init (unsigned long id);
	void Update (void);

	void Move (struct point v);
	struct point MoveStep ();

	void Turn (double delta);
	double TurnStep (struct point target);

	void Deselect (void);
	void Select (void);
	void ToggleSelect (void);
	static void DeselectAll (void);
};
