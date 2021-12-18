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
	struct point pos;
	struct sprite spr;
	struct circle body;

	Command *cmd[unit_CMD_SIZ];
	int cmd_len = 0;

	enum {
		HERO     = 1 << 0,
		ALIVE    = 1 << 1,
		SELECTED = 1 << 2,
		MOVING   = 1 << 3,
	};
	static const int ACTIVE = MOVING;

	bool Collision (struct unit *u, struct point d);
	bool UnderCursor (int x, int y);
	void Close (void);
	void Draw (void);
	void Init (unitid_t id);
	void Update (void);
	struct unit *ClosestEnemy (void);

	void Move (struct point v);
	struct point MoveNext (struct point t);

	void Turn (angle_t delta);
	angle_t TurnNext (struct point t);

	void Deselect (void);
	void Select (void);
	void ToggleSelect (void);
	static void DeselectAll (void);

	Command *Cmd (void);
	void PushCmd (Command *c);
	void PopCmd (void);
	void ClearCmd (void);
};
