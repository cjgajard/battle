#pragma once
struct angle_t {
	double a;

	angle_t(double n);
	angle_t(struct point p);
	angle_t operator- (angle_t a);
};

struct point {
	double x, y;

	double operator+ ();
	struct point operator+ (struct point p);
	struct point operator- (struct point p);
	struct point operator* (struct projection p);
	operator angle_t();
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
