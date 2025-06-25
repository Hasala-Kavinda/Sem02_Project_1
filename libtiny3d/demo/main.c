#include "C:\Users\cmhas\OneDrive\Desktop\University\Semester 2\CO1020 Computer Systems Programming\Project 1\Sem02_Project_1\libtiny3d\include\canvas.h"
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main()
{
    printf("libtiny3d - Task 1 Demo: Canvas & Line Drawing\n");
    printf("=============================================\n");

    // Create a 400x400 canvas
    canvas_t *canvas = canvas_create(400, 400);
    if (!canvas)
    {
        printf("Error: Failed to create canvas\n");
        return 1;
    }

    // Clear canvas
    canvas_clear(canvas);

    // Canvas center
    float center_x = canvas->width / 2.0f;
    float center_y = canvas->height / 2.0f;

    // Draw lines from center at 15° intervals (like a clock face)
    printf("Drawing clock face pattern...\n");

    float radius = 150.0f; // Length of lines from center
    float thickness = 2.0f;

    for (int angle_deg = 0; angle_deg < 360; angle_deg += 15)
    {
        // Convert angle to radians
        float angle_rad = angle_deg * M_PI / 180.0f;

        // Calculate end point
        float end_x = center_x + radius * cosf(angle_rad);
        float end_y = center_y + radius * sinf(angle_rad);

        // Draw line from center to end point
        draw_line_f(canvas, center_x, center_y, end_x, end_y, thickness);

        printf("  Line at %3d°: (%.1f, %.1f) -> (%.1f, %.1f)\n",
               angle_deg, center_x, center_y, end_x, end_y);
    }

    // Add some diagonal test lines to showcase floating-point precision
    printf("\nAdding floating-point precision test lines...\n");

    // Draw some lines with floating-point coordinates
    draw_line_f(canvas, 50.5f, 50.7f, 350.3f, 60.2f, 1.5f);
    draw_line_f(canvas, 60.2f, 340.8f, 340.9f, 350.1f, 1.5f);

    // Draw a circle using many short line segments
    printf("Drawing circle using line segments...\n");
    float circle_radius = 80.0f;
    int num_segments = 36;

    for (int i = 0; i < num_segments; i++)
    {
        float angle1 = 2.0f * M_PI * i / num_segments;
        float angle2 = 2.0f * M_PI * (i + 1) / num_segments;

        float x1 = center_x + circle_radius * cosf(angle1);
        float y1 = center_y + circle_radius * sinf(angle1);
        float x2 = center_x + circle_radius * cosf(angle2);
        float y2 = center_y + circle_radius * sinf(angle2);

        draw_line_f(canvas, x1, y1, x2, y2, 1.0f);
    }

    // Test set_pixel_f with floating-point coordinates
    printf("Testing floating-point pixel setting...\n");

    // Draw some test pixels at floating-point coordinates
    for (float x = 10.0f; x < 20.0f; x += 0.5f)
    {
        for (float y = 10.0f; y < 20.0f; y += 0.5f)
        {
            set_pixel_f(canvas, x, y, 0.8f);
        }
    }

    // Save canvas as PGM image
    canvas_save_pgm(canvas, "task1_demo.pgm");

    printf("\nTask 1 Demo completed successfully!\n");
    printf("Output saved as 'task1_demo.pgm'\n");
    printf("You can view the PGM file with image viewers like GIMP or convert it to PNG\n");

    // Clean up
    canvas_destroy(canvas);

    return 0;
}