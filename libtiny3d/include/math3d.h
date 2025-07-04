#ifndef MATH3D_H
#define MATH3D_H

#include <math.h>
#include <stdbool.h>

// 3D Vector structure with Cartesian and spherical coordinates
typedef struct
{
    // Cartesian coordinates
    float x, y, z;

    // Spherical coordinates
    float r;     // radius
    float theta; // azimuthal angle in XY plane from X-axis (0 to 2π)
    float phi;   // polar angle from Z-axis (0 to π)
} vec3_t;

typedef struct
{
    float x, y, z, w;
} vec4_t;

// 4x4 Matrix structure (column-major order)
typedef struct
{
    // float m[16];
    float m[4][4];
} mat4_t;

// Vector operations
vec3_t vec3_create(float x, float y, float z);
vec3_t vec3_from_spherical(float r, float theta, float phi);
void vec3_update_spherical(vec3_t *v);
void vec3_update_cartesian(vec3_t *v);
vec3_t vec3_add(vec3_t a, vec3_t b);
vec3_t vec3_sub(vec3_t a, vec3_t b);
vec3_t vec3_scale(vec3_t v, float s);
float vec3_dot(vec3_t a, vec3_t b);
vec3_t vec3_cross(vec3_t a, vec3_t b);
float vec3_length(vec3_t v);
vec3_t vec3_normalize(vec3_t v);
vec3_t vec3_normalize_fast(vec3_t v);
vec3_t vec3_slerp(vec3_t a, vec3_t b, float t);
vec4_t mat4_transform_vec4(mat4_t m, vec4_t v);

// Matrix operations
mat4_t mat4_identity();
mat4_t mat4_translate(float tx, float ty, float tz);
mat4_t mat4_scale(float sx, float sy, float sz);
mat4_t mat4_rotate_xyz(float rx, float ry, float rz);
mat4_t mat4_frustum_asymmetric(float left, float right, float bottom, float top, float near, float far);
mat4_t mat4_multiply(mat4_t a, mat4_t b);
vec3_t mat4_transform_vec3(mat4_t m, vec3_t v);
mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up);

#endif // MATH3D_H