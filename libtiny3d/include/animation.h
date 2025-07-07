#ifndef ANIMATION_H
#define ANIMATION_H

#include "math3d.h"

//  * A cubic Bezier curve is defined by four control points: a start point (p0),
//  * an end point (p3), and two control points (p1, p2) that determine the curve's shape.
//  *
//  *  p0 The starting control point.
//  *  p1 The first intermediate control point.
//  * p2 The second intermediate control point.
//  *  p3 The ending control point.
//  * t The interpolation parameter, typically in the range [0, 1].
//  *  The interpolated position vector on the curve for the given t.

vec3_t vec3_bezier(vec3_t p0, vec3_t p1, vec3_t p2, vec3_t p3, float t);

#endif // ANIMATION_H
