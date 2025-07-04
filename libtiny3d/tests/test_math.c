#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "canvas.h"
#include "math3d.h"

// Cube vertex
vec3_t cube[8] = {
    {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1}, {-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}};

// edges
int edges[12][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0}, // bottom
    {4, 5},
    {5, 6},
    {6, 7},
    {7, 4}, // top
    {0, 4},
    {1, 5},
    {2, 6},
    {3, 7} // sides
};

// Look-at matrix

int main()
{
    const int width = 512, height = 512;
    const int frames = 100;

    // Define frustum (wider field of view)
    float near = 0.1f;
    float far = 10.0f;
    float fov = 1.0f; // in radians
    float aspect = (float)width / height;

    // Frustum bounds
    float top = near * tanf(fov * 0.5f);
    float bottom = -top;
    float right = top * aspect;
    float left = -right;

    mat4_t projection = mat4_frustum_asymmetric(left, right, bottom, top, near, far);

    for (int frame = 0; frame < frames; frame++)
    {
        canvas_t *canvas = canvas_create(width, height);
        canvas_clear(canvas, 0.0f); // make sure canvas is cleared

        float t = frame / (float)frames;
        float angle = t * 2 * 3.14159f; // Full rotation

        // Model transformations
        mat4_t scale = mat4_scale(1.0f, 1.0f, 1.0f);
        mat4_t rotate = mat4_rotate_xyz(angle, angle * 0.5f, 0.0f);
        mat4_t translate = mat4_translate(1.0f, 0.0f, 0.0f); // Move cube away

        // View matrix
        mat4_t view = mat4_look_at(
            (vec3_t){0.0f, 0.0f, 6.0f}, // Camera at z = 5
            (vec3_t){0.0f, 0.0f, 0.0f}, // Look at origin
            (vec3_t){0.0f, 1.0f, 0.0f}  // Up vector
        );

        // Combine transformations: projection * view * model
        mat4_t model = mat4_multiply(translate, mat4_multiply(rotate, scale));
        mat4_t model_view = mat4_multiply(view, model);
        mat4_t mvp = mat4_multiply(projection, model_view);

        // Transform and project vertices
        vec3_t projected[8];
        for (int i = 0; i < 8; i++)
        {
            vec3_t v = mat4_transform_vec3(mvp, cube[i]);

            // Map to screen coordinates
            projected[i].x = (v.x + 1.0f) * width / 2.0f;
            projected[i].y = (1.0f - v.y) * height / 2.0f;
        }

        // Draw edges
        for (int i = 0; i < 12; i++)
        {
            vec3_t a = projected[edges[i][0]];
            vec3_t b = projected[edges[i][1]];
            // Basic clipping
            if (a.x >= 0 && a.x <= width && a.y >= 0 && a.y <= height &&
                b.x >= 0 && b.x <= width && b.y >= 0 && b.y <= height)
            {
                draw_line_f(canvas, a.x, a.y, b.x, b.y, 1.0f);
            }
        }

        // Save frame
        char filename[128];
        snprintf(filename, sizeof(filename), "tests/frames/frame_%03d.pgm", frame);
        canvas_save_pgm(canvas, filename);
        canvas_destroy(canvas);
    }

    printf("Frames generated in tests/frames/ .\n");
    return 0;
}