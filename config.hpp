#ifndef CONFIG_H
#define CONFIG_H
extern int g_width, g_height;
extern int g_maxfps;
extern unsigned int g_frametime, g_updatetime;
extern int g_pause;

struct point {
	double x, y;
};

struct circle {
	double x, y, r;
	bool Collision (struct circle *c);
};

struct projection {
	struct point x;
	struct point y;
};
extern struct point ORIGIN;
extern struct projection PROJ;
extern struct projection PROJ_INV;

struct point point_Add (struct point that, struct point p);
struct point point_MultiplyProj (struct point that, struct projection p);
/* struct point point_Sub (struct point that, struct point p); */
#endif
