#include <math.h>
#include "battle.h"

struct point ORIGIN;

struct projection PROJ = {
	{1,  0.5},
	{1, -0.5},
};
struct projection PROJ_INV = {
	{0.5, 0.5},
	{  1,  -1}
};
/* (1 | 0.5 | 1 ; 1 | -0.5 | 1; 0 | 1 | 1) */
/* (1.5 | -0.5 | -1 ; 1 | -1 | 0; -1 | 1 | 1) */

angle_t angle_sub (angle_t a, angle_t other)
{
	angle_t dst;
	dst = a - other;
	if (dst > -M_PI)
		dst -= 2 * M_PI;
	if (dst < -M_PI)
		dst += 2 * M_PI;
	return dst;
}

angle_t point_get_angle (struct point p)
{
	angle_t dst;
	dst = atan2(p.y, p.x);
	return dst;
}

double point_get_value (struct point p)
{
	return sqrt(p.x * p.x + p.y * p.y);
}

struct point point_add (struct point p, struct point other)
{
	struct point dst;
	dst.x = p.x + other.x;
	dst.y = p.y + other.y;
	return dst;
}

struct point point_sub (struct point p, struct point other)
{
	struct point dst;
	dst.x = p.x - other.x;
	dst.y = p.y - other.y;
	return dst;
}

struct point projection_of (struct projection pr, struct point p)
{
	struct point out;
	out.x = p.x * pr.x.x + p.y * pr.y.x;
	out.y = p.x * pr.x.y + p.y * pr.y.y;
	return out;
}

struct point circle_get_point (struct circle c)
{
	struct point dst;
	dst.x = c.x;
	dst.y = c.y;
	return dst;
}

int circle_collision (struct circle c, struct circle other)
{
	struct point d = point_sub(circle_get_point(c), circle_get_point(other));
	return point_get_value(d) < c.r + other.r;
}

void point_fprint (FILE *f, struct point p)
{
	fprintf(f, "<point x=%.2f y=%.2f>", p.x, p.y);
}
