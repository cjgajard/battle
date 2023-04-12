#ifndef BATTLE_H
#define BATTLE_H
#include <stdio.h>

typedef unsigned long unitid_t;

typedef double angle_t;

struct point {
	double x, y;
};

struct circle {
	double x, y, r;
};

struct projection {
	struct point x;
	struct point y;
};

struct move {
	unitid_t uid;
	struct point target;
	struct point mv;
	angle_t a;
};

struct attack {
	unitid_t uid;
	unitid_t target;
	unsigned int lastatktime;
	int state;
	int outofrange;
	struct move mv;
};

enum commandtype {
	COMMAND_MOVE = 1,
	COMMAND_ATTACK
};

union commanddata {
	struct move mv;
	struct attack atk;
};

struct command {
	enum commandtype type;
	union commanddata data;
};

void command_move_init(struct command *c, unitid_t id, struct point target);
void command_attack_init(struct command *c, unitid_t id, unitid_t target);

int command_next(struct command *c);
void command_apply(struct command *c);
void command_halt(struct command *c);
extern int g_width, g_height;
extern int g_maxfps;
extern unsigned int g_frametime, g_updatetime;

int game_Init (void);
void game_Close (void);
void game_Update (void);
void game_Draw (double delta);

/* should OnEventName be moved to a event_ namespace? */
void game_OnKeydown (void *event);
void game_OnKeyup (void *event);
void game_OnClick (void *event);
void game_OnRelease (void *event);
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

angle_t      point_get_angle (struct point p);
double       point_get_value (struct point p);
struct point point_add       (struct point p, struct point other);
struct point point_sub       (struct point p, struct point other);
void         point_fprint    (FILE *f, struct point p);

angle_t angle_sub (angle_t a, angle_t other);

int          circle_collision (struct circle c, struct circle other);
struct point circle_get_point (struct circle c);

struct point projection_of (struct projection pr, struct point p);

extern struct point ORIGIN;
extern struct projection PROJ;
extern struct projection PROJ_INV;

#define GRID_LEN 16
extern unsigned int g_pause;
extern unsigned int g_time;

extern int g_unit_len;
extern unitid_t g_unit_id;

#define G_SELECTION_SIZ 8
extern unitid_t g_selection[G_SELECTION_SIZ];
extern int g_selection_len;

struct sprite {
	int x, y;
	int w, h;
};

#define UNIT_CMD_SIZ 32

struct unit {
	unitid_t id;
	unsigned int flags;
	double dir, maxspd;
	double maxturnspd;
	double hp, maxhp;
	double atkrange, atkdmg;
	unsigned int atkspeed, atkduration, atktime;

	struct point pos;
	struct sprite spr;
	struct circle body;
	struct command cmd[UNIT_CMD_SIZ];
	int cmd_len;
};

void unit_init (struct unit *u);

int unit_collision (struct unit *u, struct unit *other, struct point p);
int unit_under_cursor (struct unit *u, int x, int y);
struct unit *unit_get_enemy_closest (struct unit *u);
void unit_draw (struct unit *u);
void unit_update (struct unit *u);

void unit_move (struct unit *u, struct point delta);
struct point unit_move_next (struct unit *u, struct point target);

void unit_turn (struct unit *u, angle_t delta);
angle_t unit_turn_next (struct unit *u, struct point target);

void unit_attack (struct unit *u, struct unit *target);

void unit_select_set (struct unit *u);
void unit_select_unset (struct unit *u);
void unit_select_toggle (struct unit *u);
void unit_select_none (void);

struct command *unit_cmd (struct unit *u);
void unit_cmd_push (struct unit *u, struct command c);
void unit_cmd_pop (struct unit *u);
void unit_cmd_clear (struct unit *u);

#define UNIT_HERO      (1 << 0)
#define UNIT_ALIVE     (1 << 1)
#define UNIT_SELECTED  (1 << 2)
#define UNIT_MOVING    (1 << 3)
#define UNIT_ATTACKING (1 << 4)
#define UNIT_ACTIVE    (UNIT_MOVING | UNIT_ATTACKING)

#define G_UNIT_SIZ 256
extern struct unit g_unit[G_UNIT_SIZ];

int  draw_init (void);
void draw_close (void);
void draw_update (void);

/* DEBUG */
int is_zero (double n);
#endif
