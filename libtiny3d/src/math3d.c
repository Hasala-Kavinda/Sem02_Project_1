#include "math3d.h"
#include <math.h>
#include <stdint.h>

// Fast inverse square root (Quake III algorithm)
float Q_rsqrt(float number)
{
    union
    {
        float f;
        uint32_t i;
    } conv = {.f = number};

    conv.i = 0x5f3759df - (conv.i >> 1);
    conv.f *= 1.5F - (number * 0.5F * conv.f * conv.f);
    return conv.f;
}

vec3_t vec3_from_cartesian(float x, float y, float z)
{
    vec3_t v = {
        .x = x, .y = y, .z = z, .cartesian_dirty = false, .spherical_dirty = true};
    return v;
}

vec3_t vec3_from_spherical(float r, float theta, float phi)
{
    vec3_t v = {
        .r = r, .theta = theta, .phi = phi, .cartesian_dirty = true, .spherical_dirty = false};
    return v;
}

void update_spherical(vec3_t *v)
{
    if (!v->spherical_dirty)
        return;

    v->r = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
    v->theta = atan2f(v->y, v->x);
    v->phi = acosf(v->z / v->r);
    v->spherical_dirty = false;
}

void update_cartesian(vec3_t *v)
{
    if (!v->cartesian_dirty)
        return;

    float sin_theta = sinf(v->theta);
    float cos_theta = cosf(v->theta);
    float sin_phi = sinf(v->phi);
    float cos_phi = cosf(v->phi);

    v->x = v->r * cos_theta * sin_phi;
    v->y = v->r * sin_theta * sin_phi;
    v->z = v->r * cos_phi;
    v->cartesian_dirty = false;
}

vec3_t vec3_normalize_fast(vec3_t v)
{
    update_cartesian(&v);
    float inv_norm = Q_rsqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    v.x *= inv_norm;
    v.y *= inv_norm;
    v.z *= inv_norm;
    v.spherical_dirty = true;
    return v;
}

vec3_t vec3_slerp(vec3_t a, vec3_t b, float t)
{
    update_cartesian(&a);
    update_cartesian(&b);

    // Normalize vectors
    a = vec3_normalize_fast(a);
    b = vec3_normalize_fast(b);

    float dot = a.x * b.x + a.y * b.y + a.z * b.z;
    dot = fmaxf(-1.0f, fminf(1.0f, dot)); // Clamp to avoid numerical issues

    float theta = acosf(dot) * t;
    vec3_t relative = vec3_from_cartesian(
        b.x - a.x * dot,
        b.y - a.y * dot,
        b.z - a.z * dot);
    relative = vec3_normalize_fast(relative);

    float sin_theta = sinf(theta);
    float cos_theta = cosf(theta);

    vec3_t result = vec3_from_cartesian(
        a.x * cos_theta + relative.x * sin_theta,
        a.y * cos_theta + relative.y * sin_theta,
        a.z * cos_theta + relative.z * sin_theta);

    return result;
}

mat4_t mat4_identity()
{
    mat4_t m = {{1, 0, 0, 0,
                 0, 1, 0, 0,
                 0, 0, 1, 0,
                 0, 0, 0, 1}};
    return m;
}

mat4_t mat4_translate(float tx, float ty, float tz)
{
    mat4_t m = mat4_identity();
    m.m[12] = tx;
    m.m[13] = ty;
    m.m[14] = tz;
    return m;
}

mat4_t mat4_scale(float sx, float sy, float sz)
{
    mat4_t m = mat4_identity();
    m.m[0] = sx;
    m.m[5] = sy;
    m.m[10] = sz;
    return m;
}

mat4_t mat4_rotate_xyz(float rx, float ry, float rz)
{
    mat4_t m = mat4_identity();

    float cx = cosf(rx), sx = sinf(rx);
    float cy = cosf(ry), sy = sinf(ry);
    float cz = cosf(rz), sz = sinf(rz);

    // Rotation around X
    mat4_t rot_x = mat4_identity();
    rot_x.m[5] = cx;
    rot_x.m[6] = -sx;
    rot_x.m[9] = sx;
    rot_x.m[10] = cx;

    // Rotation around Y
    mat4_t rot_y = mat4_identity();
    rot_y.m[0] = cy;
    rot_y.m[2] = sy;
    rot_y.m[8] = -sy;
    rot_y.m[10] = cy;

    // Rotation around Z
    mat4_t rot_z = mat4_identity();
    rot_z.m[0] = cz;
    rot_z.m[1] = -sz;
    rot_z.m[4] = sz;
    rot_z.m[5] = cz;

    // Combine rotations: Z * Y * X
    // (Note: Matrix multiplication implementation needed)
    return m;
}

mat4_t mat4_frustum_asymmetric(float left, float right, float bottom, float top, float near, float far)
{
    mat4_t m = {{2 * near / (right - left), 0, (right + left) / (right - left), 0,
                 0, 2 * near / (top - bottom), (top + bottom) / (top - bottom), 0,
                 0, 0, -(far + near) / (far - near), -2 * far * near / (far - near),
                 0, 0, -1, 0}};
    return m;
}

mat4_t mat4_mul(mat4_t a, mat4_t b)
{
    mat4_t result;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            result.m[i + j * 4] = 0.0f;
            for (int k = 0; k < 4; ++k)
            {
                result.m[i + j * 4] += a.m[i + k * 4] * b.m[k + j * 4];
            }
        }
    }
    return result;
}

vec3_t mat4_mul_vec3(mat4_t m, vec3_t v)
{
    update_cartesian(&v);

    float x = m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z + m.m[12];
    float y = m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z + m.m[13];
    float z = m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14];
    float w = m.m[3] * v.x + m.m[7] * v.y + m.m[11] * v.z + m.m[15];

    if (w != 0.0f)
    {
        x /= w;
        y /= w;
        z /= w;
    }

    return vec3_from_cartesian(x, y, z);
}
