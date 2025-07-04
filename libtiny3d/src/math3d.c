#include "math3d.h"
#include <math.h>

// Create a 3D vector with Cartesian coordinates
vec3_t vec3_create(float x, float y, float z)
{
    vec3_t v = {x, y, z, 0.0f, 0.0f, 0.0f};
    vec3_update_spherical(&v);
    return v;
}

// Create a 3D vector from spherical coordinates
vec3_t vec3_from_spherical(float r, float theta, float phi)
{
    vec3_t v = {0.0f, 0.0f, 0.0f, r, theta, phi};
    vec3_update_cartesian(&v);
    return v;
}

// Update spherical coordinates based on Cartesian coordinates
void vec3_update_spherical(vec3_t *v)
{
    v->r = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);

    if (v->r == 0.0f)
    {
        v->theta = 0.0f;
        v->phi = 0.0f;
    }
    else
    {
        v->theta = atan2f(v->y, v->x);
        v->phi = acosf(v->z / v->r);
    }
}

// Update Cartesian coordinates based on spherical coordinates
void vec3_update_cartesian(vec3_t *v)
{
    float sin_phi = sinf(v->phi);
    v->x = v->r * sin_phi * cosf(v->theta);
    v->y = v->r * sin_phi * sinf(v->theta);
    v->z = v->r * cosf(v->phi);
}

// Vector addition
vec3_t vec3_add(vec3_t a, vec3_t b)
{
    return vec3_create(a.x + b.x, a.y + b.y, a.z + b.z);
}

// Vector subtraction
vec3_t vec3_sub(vec3_t a, vec3_t b)
{
    return vec3_create(a.x - b.x, a.y - b.y, a.z - b.z);
}

// Vector scaling
vec3_t vec3_scale(vec3_t v, float s)
{
    return vec3_create(v.x * s, v.y * s, v.z * s);
}

// Dot product
float vec3_dot(vec3_t a, vec3_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Cross product
vec3_t vec3_cross(vec3_t a, vec3_t b)
{
    // from the equation
    return vec3_create(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x);
}

// Vector length
float vec3_length(vec3_t v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Normalize vector (accurate version)
vec3_t vec3_normalize(vec3_t v)
{
    float len = vec3_length(v);
    if (len == 0.0f)
        return v;
    return vec3_scale(v, 1.0f / len);
}

// Fast normalize using Quake's inverse square root approximation
vec3_t vec3_normalize_fast(vec3_t v)
{
    float len_sq = v.x * v.x + v.y * v.y + v.z * v.z;
    if (len_sq == 0.0f)
        return v;

    // Fast inverse square root (Quake III algorithm)
    float half_len = 0.5f * len_sq;
    int i = *(int *)&len_sq;
    i = 0x5f3759df - (i >> 1);
    len_sq = *(float *)&i;

    // after appling the f'x and fx to the eqaution this is what we get
    len_sq = len_sq * (1.5f - (half_len * len_sq * len_sq));

    return vec3_scale(v, len_sq);
}

// Spherical linear interpolation
vec3_t vec3_slerp(vec3_t a, vec3_t b, float t)
{
    // Normalize inputs
    a = vec3_normalize(a);
    b = vec3_normalize(b);

    float dot = vec3_dot(a, b);
    dot = fmaxf(-1.0f, fminf(1.0f, dot)); // Clamp to avoid numerical issues

    float theta = acosf(dot) * t;
    vec3_t relative = vec3_normalize(vec3_sub(b, vec3_scale(a, dot)));

    return vec3_add(vec3_scale(a, cosf(theta)), vec3_scale(relative, sinf(theta)));
}

// Create identity matrix
mat4_t mat4_identity()
{
    // As m is a structure and the matrix is iniside it
    mat4_t m = {{
        {1.0f, 0, 0, 0},
        {0, 1.0f, 0, 0},
        {0, 0, 1.0f, 0},
        {0, 0, 0, 1.0f},
    }};

    return m;
}

// Create translation matrix
mat4_t mat4_translate(float tx, float ty, float tz)
{
    mat4_t m = mat4_identity();
    m.m[0][3] = tx;
    m.m[1][3] = ty;
    m.m[2][3] = tz;
    return m;
}
// Create scaling matrix
mat4_t mat4_scale(float sx, float sy, float sz)
{
    mat4_t m = mat4_identity();
    m.m[0][0] = sx;
    m.m[1][1] = sy;
    m.m[2][2] = sz;

    return m;
}

// Create rotation matrix (Euler angles XYZ)
mat4_t mat4_rotate_xyz(float rx, float ry, float rz)
{
    float cx = cosf(rx), sx = sinf(rx);
    float cy = cosf(ry), sy = sinf(ry);
    float cz = cosf(rz), sz = sinf(rz);

    mat4_t m = {{{cy * cz, sx * sy * cz + cx * sz, -cx * sy * cz + sx * sz, 0},
                 {-cy * sz, -sx * sy * sz + cx * cz, cx * sy * sz + sx * cz, 0},
                 {sy, -sx * cy, cx * cy, 0},
                 {0, 0, 0, 1}}};

    return m;
}

// Create asymmetric frustum projection matrix
// implementation of the matrix
mat4_t mat4_frustum_asymmetric(float left, float right, float bottom, float top, float near, float far)

{
    mat4_t m = {{{(2 * near) / (right - left), 0, (right + left) / (right - left), 0},
                 {0, (2 * near) / (top - bottom), (top + bottom) / (top - bottom), 0},
                 {0, 0, -(far + near) / (far - near), -(2 * far * near) / (far - near)},
                 {0, 0, -1, 0}}};

    return m;
}

// Matrix multiplication
mat4_t mat4_multiply(mat4_t a, mat4_t b)
{
    mat4_t m;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            m.m[i][j] = 0;
            for (int k = 0; k < 4; k++)
            {
                m.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return m;
}

vec4_t mat4_transform_vec4(mat4_t m, vec4_t v)
{
    // multiply the vector by matrix
    vec4_t result;
    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
    return result;
}

vec3_t mat4_transform_vec3(mat4_t m, vec3_t v)
{
    vec4_t input = {v.x, v.y, v.z, 1.0f};
    vec4_t result = mat4_transform_vec4(m, input);

    // makng sure that w is not zero
    if (fabsf(result.w) > 0.0001f)
    {
        result.x /= result.w;
        result.y /= result.w;
        result.z /= result.w;
    }

    return vec3_create(result.x, result.y, result.z);
}

mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up)
{
    vec3_t z = vec3_normalize(vec3_sub(target, eye));
    vec3_t x = vec3_normalize(vec3_cross(up, z));
    vec3_t y = vec3_cross(z, x);

    mat4_t m = {{{x.x, x.y, x.z, -vec3_dot(x, eye)},
                 {y.x, y.y, y.z, -vec3_dot(y, eye)},
                 {z.x, z.y, z.z, -vec3_dot(z, eye)},
                 {0, 0, 0, 1}}};

    return m;
}