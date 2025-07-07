#define _USE_MATH_DEFINES
#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <lighting.h>

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
    vec3_t eye = vec3_create(0, 2, 5);
    vec3_t target = vec3_create(0, 0, 0);
    vec3_t up = vec3_create(0, 1, 0);
    mat4_t world_to_camera = mat4_look_at(eye, target, up);

    // Projection setup - fruntum is along z axis
    float near = 0.1f, far = 100.0f;
    float aspect = (float)canvas->width / canvas->height;
    float fov = 60.0f * (M_PI / 180.0f); // in radians
    float top = near * tanf(fov / 2.0f);
    float right = top * aspect;
    mat4_t projection = mat4_frustum_asymmetric(-right, right, -top, top, near, far);

    light_t lights[] = {
        // Main light (bright, from top-front)
        {.direction = vec3_create(0.5f, 1.0f, 1.0f), .intensity = 0.9f},
        // Fill light (from side)
        {.direction = vec3_create(-1.0f, 0.5f, 0.5f), .intensity = 0.5f},
        // Back light
        {.direction = vec3_create(0.0f, 0.5f, -1.0f), .intensity = 0.3f},
        // Bottom light (subtle)
        {.direction = vec3_create(0.0f, -1.0f, 0.5f), .intensity = 0.2f}};
    int num_lights = sizeof(lights) / sizeof(lights[0]);

    // Animation loop (generate multiple frames)
    for (int frame = 0; frame < 200; frame++)
    {
        canvas_clear(canvas, 0.0f);

        // Rotate object
        float angle = frame * 0.1f;
        mat4_t rotation = mat4_rotate_xyz(angle, angle, angle);
        mat4_t scale = mat4_scale(1.5, 1.5, 1.5);

        mat4_t local_to_world = mat4_multiply(rotation, scale);

        // Render wireframe
        wireframe(canvas, soccer_ball, local_to_world, world_to_camera, projection, lights, 4, near, far);

        // Save frame
        char filename[128];
        snprintf(filename, sizeof(filename), "../tests/visual_tests/frames_pipeline/frame_%03d.pgm", frame);
        canvas_save_pgm(canvas, filename);
    }

    // Cleanup
    free(soccer_ball->vertices);
    free(soccer_ball->edges);
    free(soccer_ball);
    canvas_destroy(canvas);

    return 0;
}