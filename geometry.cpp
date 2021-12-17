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

angle_t::angle_t(double a) {
	this->a = a;
}

angle_t::angle_t(struct point p) {
	this->a = atan2(p.y, p.x);
}

angle_t angle_t::operator- (angle_t that)
{
	double a = this->a - that.a;
	if (a > -M_PI)
		a -= 2 * M_PI;
	if (a < -M_PI)
		a += 2 * M_PI;
	return angle_t(a);
}

double point::operator+ ()
{
	return sqrt(this->x * this->x + this->y * this->y);
}

struct point point::operator+ (struct point p)
{
	struct point dst;
	dst.x = this->x + p.x;
	dst.y = this->y + p.y;
	return dst;
}

struct point point::operator- (struct point p)
{
	struct point dst;
	dst.x = this->x - p.x;
	dst.y = this->y - p.y;
	return dst;
}

struct point point::operator* (struct projection p)
{
	struct point out;
	out.x = this->x * p.x.x + this->y * p.y.x;
	out.y = this->x * p.x.y + this->y * p.y.y;
	return out;
}

circle::operator point() {
	struct point dst;
	dst.x = this->x;
	dst.y = this->y;
	return dst;
}
