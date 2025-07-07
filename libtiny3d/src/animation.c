#include "animation.h"
#include <math.h>

/**
 * @brief Calculates a point on a 3D cubic Bezier curve.
 */
vec3_t vec3_bezier(vec3_t p0, vec3_t p1, vec3_t p2, vec3_t p3, float t)
{
    // Clamp t to the valid range [0, 1]
    t = fmaxf(0.0f, fminf(1.0f, t));

    // Pre-calculate powers of t and (1-t) for the Bernstein polynomial
    float one_minus_t = 1.0f - t;
    float one_minus_t_sq = one_minus_t * one_minus_t;
    float one_minus_t_cub = one_minus_t_sq * one_minus_t;
    float t_sq = t * t;
    float t_cub = t_sq * t;

    // Apply the cubic Bezier formula:
    // B(t) = (1-t)^3 * P0 + 3(1-t)^2 * t * P1 + 3(1-t) * t^2 * P2 + t^3 * P3
    vec3_t term0 = vec3_scale(p0, one_minus_t_cub);
    vec3_t term1 = vec3_scale(p1, 3.0f * one_minus_t_sq * t);
    vec3_t term2 = vec3_scale(p2, 3.0f * one_minus_t * t_sq);
    vec3_t term3 = vec3_scale(p3, t_cub);

    // Sum the terms to get the final position
    vec3_t position = vec3_add(term0, term1);
    position = vec3_add(position, term2);
    position = vec3_add(position, term3);

    return position;
}