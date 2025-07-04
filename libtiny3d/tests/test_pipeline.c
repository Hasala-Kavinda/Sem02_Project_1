#define _USE_MATH_DEFINES
#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{
    // Create canvas
    canvas_t *canvas = canvas_create(512, 512);
    if (!canvas)
    {
        printf("Failed to create canvas\n");
        return 1;
    }

    // Generate soccer ball
    object3d_t *soccer_ball = generate_soccer_ball();

    // Camera setup
    vec3_t eye = vec3_create(0, 0, 5);
    vec3_t target = vec3_create(0, 0, 0);
    vec3_t up = vec3_create(0, 1, 0);
    mat4_t world_to_camera = mat4_look_at(eye, target, up);

    // Projection setup (symmetric frustum for simplicity)
    float near = 0.1f, far = 100.0f;
    float aspect = (float)canvas->width / canvas->height;
    float fov = 60.0f * (M_PI / 180.0f);
    float top = near * tanf(fov / 2.0f);
    float right = top * aspect;
    mat4_t projection = mat4_frustum_asymmetric(-right, right, -top, top, near, far);

    // Animation loop (generate multiple frames)
    for (int frame = 0; frame < 100; frame++)
    {
        canvas_clear(canvas, 0.0f);

        // Rotate object
        float angle = frame * 0.1f;
        mat4_t rotation = mat4_rotate_xyz(0, angle, 0);
        mat4_t local_to_world = rotation;

        // Render wireframe
        wireframe(canvas, soccer_ball, local_to_world, world_to_camera, projection);

        // Save frame
        char filename[32];
        snprintf(filename, sizeof(filename), "frame_%03d.pgm", frame);
        canvas_save_pgm(canvas, filename);
    }

    // Cleanup
    free(soccer_ball->vertices);
    free(soccer_ball->edges);
    free(soccer_ball);
    canvas_destroy(canvas);

    return 0;
}