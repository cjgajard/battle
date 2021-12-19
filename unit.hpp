#pragma once
#include "config.hpp"
#include "geometry.hpp"
#include "command.hpp"

struct sprite {
	int x, y;
	int w, h;
};

#define unit_CMD_SIZ 32

struct unit {
	unitid_t id;
	unsigned int flags;
	double dir, maxspd;
	double maxturnspd;
	double hp, maxhp;
	double atkrange, atkspd, atkanimation, dmg;
	struct point pos;
	struct sprite spr;
	struct circle body;
	struct command *cmd[unit_CMD_SIZ];
	int cmd_len = 0;

	unit ();

	bool Collision (struct unit *u, struct point d);
	bool UnderCursor (int x, int y);
	struct unit *ClosestEnemy (void);
	void Draw (void);
	void Update (void);

	void Move (struct point v);
	struct point MoveNext (struct point t);
	void Turn (angle_t delta);
	angle_t TurnNext (struct point t);
	void Attack (struct unit *u);

	void Deselect (void);
	void Select (void);
	void ToggleSelect (void);
	static void DeselectAll (void);

	struct command *Cmd (void);
	void PushCmd (struct command *c);
	void PopCmd (void);
	void ClearCmd (void);

	enum {
		HERO      = 1 << 0,
		ALIVE     = 1 << 1,
		SELECTED  = 1 << 2,
		MOVING    = 1 << 3,
		ATTACKING = 1 << 4,
		ACTIVE = MOVING | ATTACKING,
	};
};
