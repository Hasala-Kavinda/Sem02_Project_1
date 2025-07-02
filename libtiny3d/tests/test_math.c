#include "math3d.h"
#include <stdio.h>

// #define _USE_MATH_DEFINES
// #include <math.h>
#define M_PI 3.14159265358979323846

void print_vec3(const char *name, vec3_t v)
{
    printf("%s: (%.2f, %.2f, %.2f) [r=%.2f, θ=%.2f, φ=%.2f]\n",
           name, v.x, v.y, v.z, v.r, v.theta, v.phi);
}

void print_mat4(const char *name, mat4_t m)
{
    printf("%s:\n", name);
    for (int i = 0; i < 4; i++)
    {
        printf("[ ");
        for (int j = 0; j < 4; j++)
        {
            printf("%.2f ", m.m[i][j]);
        }
        printf("]\n");
    }
}

int main()
{
    printf("=== 3D Math Foundation Demo ===\n\n");

    // Vector operations demo
    printf("Vector Operations:\n");
    vec3_t a = vec3_create(1, 0, 0);
    vec3_t b = vec3_create(0, 1, 0);

    print_vec3("Vector A", a);
    print_vec3("Vector B", b);

    vec3_t c = vec3_cross(a, b);
    print_vec3("A × B (cross product)", c);

    vec3_t d = vec3_from_spherical(1.0f, M_PI / 4, M_PI / 4);
    print_vec3("Vector from spherical (r=1, θ=π/4, φ=π/4)", d);

    vec3_t e = vec3_slerp(a, b, 0.5f);
    print_vec3("Slerp between A and B at t=0.5", e);

    // Matrix operations demo
    printf("\nMatrix Operations:\n");
    mat4_t trans = mat4_translate(2, 3, 4);
    print_mat4("Translation matrix (2,3,4)", trans);

    mat4_t rot = mat4_rotate_xyz(M_PI / 4, M_PI / 6, M_PI / 3);
    print_mat4("Rotation matrix (π/4, π/6, π/3)", rot);

    mat4_t scale = mat4_scale(2, 3, 4);
    print_mat4("Scale matrix (2,3,4)", scale);

    mat4_t frustum = mat4_frustum_asymmetric(-1, 1, -1, 1, 1, 100);
    print_mat4("Frustum matrix (asymmetric)", frustum);

    // Transform a cube vertex
    printf("\nCube Transformation:\n");
    vec3_t cube_vertex = vec3_create(0.5f, 0.5f, 0.5f);
    print_vec3("Original vertex", cube_vertex);

    mat4_t transform = mat4_multiply(mat4_translate(0, 0, -5), mat4_rotate_xyz(0.3f, 0.5f, 0.2f));
    vec3_t transformed = mat4_transform_vec3(transform, cube_vertex);
    print_vec3("Transformed vertex", transformed);

    return 0;
}