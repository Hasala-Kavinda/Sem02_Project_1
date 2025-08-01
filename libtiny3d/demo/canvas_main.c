#define _USE_MATH_DEFINES
#include "../include/canvas.h"
#include <math.h>
#include <stdio.h>

#define WIDTH 400
#define HEIGHT 400
#define RADIUS 100.0f
int main()
{
    canvas_t *canvas = canvas_create(WIDTH, HEIGHT);
    if (!canvas)
    {
        printf("Failed to create canvas\n");
        return 1;
    }

    canvas_clear(canvas, 0.0f);

    float center_x = WIDTH / 2.0f;
    float center_y = HEIGHT / 2.0f;

    // Draw radial lines every 15 degrees, thickness 3 for visible brush effect
    for (int angle = 0; angle < 360; angle += 15)
    {
        float radians = angle * M_PI / 180.0f;
        float x1 = center_x + RADIUS * cosf(radians);
        float y1 = center_y + RADIUS * sinf(radians);
        draw_line_f(canvas, center_x, center_y, x1, y1, 1.0f, 1.0f);
    }

    char filename[128];
    snprintf(filename, sizeof(filename), "../tests/visual_tests/task_1_demo.pgm");
    canvas_save_pgm(canvas, filename);

    canvas_destroy(canvas);

    printf("Canvas saved to task_1_demo.pgm\n");
    return 0;
}
