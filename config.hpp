#ifndef CONFIG_H
#define CONFIG_H
extern int g_width, g_height;
extern int g_maxfps;
extern unsigned int g_frametime, g_updatetime;
extern int g_pause;

struct point {
	double x, y;

	double operator+ ();
	struct point operator+ (struct point p);
	struct point operator- (struct point p);
	struct point operator* (struct projection p);
};

struct circle {
	double x, y, r;

	bool Collision (struct circle *c);

	operator point();
};

struct projection {
	struct point x;
	struct point y;
};
extern struct point ORIGIN;
extern struct projection PROJ;
extern struct projection PROJ_INV;
#endif
