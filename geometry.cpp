#include <cmath>
#include "geometry.hpp"

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

angle_t::angle_t(double _a) {
	a = _a;
}

angle_t::angle_t(struct point p) {
	a = atan2(p.y, p.x);
}

angle_t angle_t::operator- (angle_t that)
{
	double d = a - that.a;
	if (d > -M_PI)
		d -= 2 * M_PI;
	if (d < -M_PI)
		d += 2 * M_PI;
	return angle_t(d);
}

double point::operator+ ()
{
	return sqrt(x * x + y * y);
}

struct point point::operator+ (struct point p)
{
	struct point dst;
	dst.x = x + p.x;
	dst.y = y + p.y;
	return dst;
}

struct point point::operator+= (struct point p)
{
	struct point dst;
	x += p.x;
	y += p.y;
	return dst;
}

struct point point::operator- (struct point p)
{
	struct point dst;
	dst.x = x - p.x;
	dst.y = y - p.y;
	return dst;
}

struct point point::operator-= (struct point p)
{
	struct point dst;
	x -= p.x;
	y -= p.y;
	return dst;
}

struct point point::operator* (struct projection p)
{
	struct point out;
	out.x = x * p.x.x + y * p.y.x;
	out.y = x * p.x.y + y * p.y.y;
	return out;
}

circle::operator point() {
	struct point dst;
	dst.x = x;
	dst.y = y;
	return dst;
}
