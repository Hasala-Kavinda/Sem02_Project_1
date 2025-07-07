#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "canvas.h"
#include "renderer.h"
#include "math3d.h"
#include "animation.h"
#include "lighting.h"

#define WIDTH 800
#define HEIGHT 600
#define NUM_FRAMES 300 // Total frames for a full loop
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main()
{
    // create the canvas
    canvas_t *canvas = canvas_create(WIDTH, HEIGHT);
    // create the soccer ball
    object3d_t *soccer_ball = generate_soccer_ball();

    // Define multiple light sources
    light_t lights[] = {
        // Main light (bright, from top-front)
        {.direction = vec3_create(0.5f, 1.0f, 1.0f), .intensity = 0.9f},
        // Fill light (from side)
        {.direction = vec3_create(-1.0f, 0.5f, 0.5f), .intensity = 0.4f},
        // // Back light
        // {.direction = vec3_create(0.0f, 0.5f, -1.0f), .intensity = 0.1f},
        // // Bottom light (subtle)
        {.direction = vec3_create(0.0f, -1.0f, 0.5f), .intensity = 0.1f}};

    // get the number of lights
    int num_lights = sizeof(lights) / sizeof(lights[0]);

    // Setup camera (view matrix)
    vec3_t eye = vec3_create(0, 0, 12);
    vec3_t target = vec3_create(0, 0, 0);
    vec3_t up = vec3_create(0, 1, 0); // y
    mat4_t world_to_camera = mat4_look_at(eye, target, up);

    // Setup projection matrix with correct aspect ratio
    float aspect_ratio = (float)WIDTH / (float)HEIGHT;
    float fov = 45.0f * M_PI / 180.0f; // rads
    float near = 1.0f;
    float far = 100.0f;
    float top = tanf(fov / 2.0f) * near;
    float bottom = -top; // symmetric
    float right = top * aspect_ratio;
    float left = -right; // symmetric
    mat4_t projection = mat4_frustum_asymmetric(left, right, bottom, top, near, far);

    // Object 1 points
    vec3_t path1_p0 = vec3_create(-3, 0, 2.0f); // Start left
    vec3_t path1_p1 = vec3_create(-3, 3, 2.0f); // Control point up-left
    vec3_t path1_p2 = vec3_create(3, -3, 2.0f); // Control point down-right
    vec3_t path1_p3 = vec3_create(3, 0, 2.0f);  // End right

    // Object 2 points
    vec3_t path2_p0 = vec3_create(0, 3, -2.0f);   // Start top
    vec3_t path2_p1 = vec3_create(3, 3, -2.0f);   // Control point top-right
    vec3_t path2_p2 = vec3_create(-3, -3, -2.0f); // Control point bottom-left
    vec3_t path2_p3 = vec3_create(0, -3, -2.0f);  // End bottom

    // Animation loop
    printf("Rendering %d frames...\n", NUM_FRAMES);
    for (int i = 0; i < NUM_FRAMES; i++)
    {
        // Calculate time 't' for the current frame, looping from 0 to 1
        float t = (float)i / (NUM_FRAMES);

        // Use a smoother easing function that loops perfectly
        float eased_t = 0.5f - 0.5f * cosf(t * 2 * M_PI); // Goes 0->1->0

        canvas_clear(canvas, 0.0f); // Clear canvas to black

        // --- Object 1 (Front) ---
        // Animate position along the simplified Bezier curve
        vec3_t pos1 = vec3_bezier(path1_p0, path1_p1, path1_p2, path1_p3, eased_t);
        mat4_t translation1 = mat4_translate(pos1.x, pos1.y, pos1.z);
        // Simple rotation around Y axis
        mat4_t rotation1 = mat4_rotate_xyz(0, t * 4 * M_PI, 0);
        mat4_t local_to_world1 = mat4_multiply(translation1, rotation1);

        // Render the first object
        wireframe(canvas, soccer_ball, local_to_world1, world_to_camera, projection, lights, num_lights, near, far);

        // --- Object 2 (Back) - Synchronized ---
        // Animate position along its own path using the same 't'
        vec3_t pos2 = vec3_bezier(path2_p0, path2_p1, path2_p2, path2_p3, eased_t);
        mat4_t translation2 = mat4_translate(pos2.x, pos2.y, pos2.z);
        // Different rotation pattern
        mat4_t rotation2 = mat4_rotate_xyz(t * 2 * M_PI, 0, t * 2 * M_PI);
        mat4_t local_to_world2 = mat4_multiply(translation2, rotation2);

        // Render the second object
        wireframe(canvas, soccer_ball, local_to_world2, world_to_camera, projection, lights, num_lights, near, far);

        // Save the rendered frame to a PGM file
        char filename[100];
        sprintf(filename, "../tests/visual_tests/frames_animation/frame_%04d.pgm", i);
        canvas_save_pgm(canvas, filename);
        printf("\rFrame %d/%d", i + 1, NUM_FRAMES);
        fflush(stdout);
    }
    printf("\nAnimation rendered successfully!\n");

    // --- Cleanup ---
    canvas_destroy(canvas);
    free(soccer_ball->vertices);
    free(soccer_ball->edges);
    free(soccer_ball);

    return 0;
}