#include "math3d.h"
#include <math.h>

// Create a 3D vector with Cartesian coordinates
vec3_t vec3_create(float x, float y, float z)
{
    // set the x,y, z form the input values
    vec3_t v = {x, y, z, 0.0f, 0.0f, 0.0f};

    // call the update function to update the spherical values according to cartesian cordinates
    vec3_update_spherical(&v);
    return v;
}

// Create a 3D vector from spherical coordinates
vec3_t vec3_from_spherical(float r, float theta, float phi)
{
    // define using spherical coordinates
    vec3_t v = {0.0f, 0.0f, 0.0f, r, theta, phi};
    // update the cartesian according to the spherical values
    vec3_update_cartesian(&v);
    return v;
}

// Update spherical coordinates based on Cartesian coordinates
void vec3_update_spherical(vec3_t *v)
{
    //
    v->r = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);

    if (v->r == 0.0f)
    {
        v->theta = 0.0f;
        v->phi = 0.0f;
    }
    else
    {
        // theta = tan inverse of (y/x)
        v->theta = atan2f(v->y, v->x);
        // theta = cos inverse of (z/r)
        v->phi = acosf(v->z / v->r);
    }
}

// Update Cartesian coordinates based on spherical coordinates
void vec3_update_cartesian(vec3_t *v)
{
    // phi in degrees
    float sin_phi = sinf(v->phi);
    // x = r * sin(φ)cos(θ)
    v->x = v->r * sin_phi * cosf(v->theta);
    // x = r * sin(φ)sin(θ)
    v->y = v->r * sin_phi * sinf(v->theta);
    // x = r * cos(φ)
    v->z = v->r * cosf(v->phi);
}

// Vector addition
vec3_t vec3_add(vec3_t a, vec3_t b)
{
    // add x,y and z coordinates
    return vec3_create(a.x + b.x, a.y + b.y, a.z + b.z);
}

// Vector subtraction
vec3_t vec3_sub(vec3_t a, vec3_t b)
{
    // sub x,y and z coordinates
    return vec3_create(a.x - b.x, a.y - b.y, a.z - b.z);
}

// Vector scaling
vec3_t vec3_scale(vec3_t v, float s)
{
    // multiply by x,y,z by scalar
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
    // a × b = <(a₂b₃ - a₃b₂), (a₃b₁ - a₁b₃), (a₁b₂ - a₂b₁)>
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
    // check if the length is 0 and return 0
    float len = vec3_length(v);
    if (len == 0.0f)
        return v;
    // give the staled down length
    return vec3_scale(v, 1.0f / len);
}

// Fast normalize using Quake's inverse square root approximation
vec3_t vec3_normalize_fast(vec3_t v)
{
    float len_sq = v.x * v.x + v.y * v.y + v.z * v.z;
    // if lenth 0 just return v
    if (len_sq == 0.0f)
        return v;

    // Fast inverse square root (Quake III algorithm)
    float half_len = 0.5f * len_sq;
    long i = *(long *)&len_sq; // get the bit value at len_sq address to long value by casting value at the address and dereferencing it
    i = 0x5f3759df - (i >> 1); // as the bits of the number is its own logarith by deviding it it gives the  square root
    len_sq = *(float *)&i;     // get back the true value

    // after appling the f'x and fx to the eqaution this is what we get
    // y = y(3/2 - (x/2)y^2)
    len_sq = len_sq * (1.5f - (half_len * len_sq * len_sq));

    return vec3_scale(v, len_sq);
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
    // calculate the cos, sin values
    float cx = cosf(rx), sx = sinf(rx);
    float cy = cosf(ry), sy = sinf(ry);
    float cz = cosf(rz), sz = sinf(rz);

    // create the final rotation matrix
    mat4_t m = {{{cy * cz, sx * sy * cz + cx * sz, -cx * sy * cz + sx * sz, 0},
                 {-cy * sz, -sx * sy * sz + cx * cz, cx * sy * sz + sx * cz, 0},
                 {sy, -sx * cy, cx * cy, 0},
                 {0, 0, 0, 1}}};

    return m;
}

// Matrix multiplication
mat4_t mat4_multiply(mat4_t a, mat4_t b)
{
    mat4_t m;
    // cij = aik * b kj
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

// multiplies a 4×4 matrix(m) by a 4D vector(v) and returns the transformed vector
vec4_t mat4_transform_vec4(mat4_t m, vec4_t v)
{

    vec4_t result;
    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
    return result;
}

// transforms a 3D vector(v) using a 4×4 transformation matrix(m),
vec3_t mat4_transform_vec3(mat4_t m, vec3_t v)
{
    // create a vector in for 4x1 matix
    vec4_t input = {v.x, v.y, v.z, 1.0f};

    vec4_t result = mat4_transform_vec4(m, input);

    // makng sure that w is not zero and normalized it back to 3d
    if (fabsf(result.w) > 0.0001f)
    {
        result.x /= result.w;
        result.y /= result.w;
        result.z /= result.w;
    }

    return vec3_create(result.x, result.y, result.z);
}

// create a view matrix
mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up)
{
    // relative to camera
    vec3_t z = vec3_normalize_fast(vec3_sub(target, eye)); // get the unit vector from eye to taget(z)
    vec3_t x = vec3_normalize_fast(vec3_cross(up, z));     // get the unit vector of x
    vec3_t y = vec3_cross(z, x);                           // lector on y

    // homogeneous matrix
    mat4_t m = {{{x.x, x.y, x.z, -vec3_dot(x, eye)},
                 {y.x, y.y, y.z, -vec3_dot(y, eye)},
                 {z.x, z.y, z.z, -vec3_dot(z, eye)},
                 {0, 0, 0, 1}}};

    return m;
}

// Create asymmetric frustum projection matrix
// implementation of the matrix
mat4_t mat4_frustum_asymmetric(float left, float right, float bottom, float top, float near, float far)

{ // implementation of frustum projection matrix to Normalized device coordinates
    mat4_t m = {{{(2 * near) / (right - left), 0, (right + left) / (right - left), 0},
                 {0, (2 * near) / (top - bottom), (top + bottom) / (top - bottom), 0},
                 {0, 0, -(far + near) / (far - near), -(2 * far * near) / (far - near)},
                 {0, 0, -1, 0}}};

    return m;
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