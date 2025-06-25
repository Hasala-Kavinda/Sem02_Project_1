#include "../include/canvas.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

canvas_t *canvas_create(int width, int height)
{
    // make sure the width and height is valid
    if (width <= 0 || height <= 0)
    {
        return NULL;
    }

    canvas_t *canvas = malloc(sizeof(canvas_t));

    // make sure the memory allocation for the canvas is successfull
    if (!canvas)
    {
        return NULL;
    }

    canvas->width = width;
    canvas->height = height;

    // Allocate 2D array for pixels
    canvas->pixels = malloc(height * sizeof(float *));
    if (!canvas->pixels)
    {
        free(canvas);
        return NULL;
    }

    for (int y = 0; y < height; y++)
    {
        canvas->pixels[y] = calloc(width, sizeof(float));
        if (!canvas->pixels[y])
        {
            // Clean up on failure
            for (int i = 0; i < y; i++)
            {
                free(canvas->pixels[i]);
            }
            free(canvas->pixels);
            free(canvas);
            return NULL;
        }
    }

    return canvas;
}

void canvas_destroy(canvas_t *canvas)
{
    if (!canvas)
        return NULL;

    if (canvas->pixels)
    {
        for (int y = 0; y < canvas->height; y++)
        {
            free(canvas->pixels[y]);
        }
        free(canvas->pixels);
    }

    free(canvas);
}

void canvas_clear(canvas_t *canvas)
{
    if (!canvas || !canvas->pixels)
        return NULL;

    for (int y = 0; y < canvas->height; y++)
    {
        memset(canvas->pixels[y], 0, canvas->width * sizeof(float));
    }
}

void set_pixel_f(canvas_t *canvas, float x, float y, float intensity)
{
    if (!canvas || !canvas->pixels)
        return;

    // Clamp intensity to valid range
    if (intensity < 0.0f)
        intensity = 0.0f;
    if (intensity > 1.0f)
        intensity = 1.0f;

    // Get integer coordinates and fractional parts
    int x0 = (int)floor(x);
    int y0 = (int)floor(y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float fx = x - x0; // Fractional part of x
    float fy = y - y0; // Fractional part of y

    // Bilinear interpolation weights
    float w00 = (1.0f - fx) * (1.0f - fy); // Top-left
    float w10 = fx * (1.0f - fy);          // Top-right
    float w01 = (1.0f - fx) * fy;          // Bottom-left
    float w11 = fx * fy;                   // Bottom-right

    // Apply intensity to each pixel with bounds checking
    if (x0 >= 0 && x0 < canvas->width && y0 >= 0 && y0 < canvas->height)
    {
        canvas->pixels[y0][x0] += intensity * w00;
        if (canvas->pixels[y0][x0] > 1.0f)
            canvas->pixels[y0][x0] = 1.0f;
    }

    if (x1 >= 0 && x1 < canvas->width && y0 >= 0 && y0 < canvas->height)
    {
        canvas->pixels[y0][x1] += intensity * w10;
        if (canvas->pixels[y0][x1] > 1.0f)
            canvas->pixels[y0][x1] = 1.0f;
    }

    if (x0 >= 0 && x0 < canvas->width && y1 >= 0 && y1 < canvas->height)
    {
        canvas->pixels[y1][x0] += intensity * w01;
        if (canvas->pixels[y1][x0] > 1.0f)
            canvas->pixels[y1][x0] = 1.0f;
    }

    if (x1 >= 0 && x1 < canvas->width && y1 >= 0 && y1 < canvas->height)
    {
        canvas->pixels[y1][x1] += intensity * w11;
        if (canvas->pixels[y1][x1] > 1.0f)
            canvas->pixels[y1][x1] = 1.0f;
    }
}

void draw_line_f(canvas_t *canvas, float x0, float y0, float x1, float y1, float thickness)
{
    if (!canvas || thickness <= 0.0f)
        return;

    // Calculate line direction and length
    float dx = x1 - x0;
    float dy = y1 - y0;
    float length = sqrtf(dx * dx + dy * dy);

    if (length == 0.0f)
    {
        // Single point - draw a circle
        for (float t = -thickness; t <= thickness; t += 0.1f)
        {
            for (float s = -thickness; s <= thickness; s += 0.1f)
            {
                if (t * t + s * s <= thickness * thickness)
                {
                    set_pixel_f(canvas, x0 + s, y0 + t, 1.0f);
                }
            }
        }
        return;
    }

    // Normalize direction vector
    float ux = dx / length;
    float uy = dy / length;

    // Perpendicular vector for thickness
    float px = -uy;
    float py = ux;

    // DDA algorithm with thickness
    int steps = (int)(length + 1);
    if (steps < 1)
        steps = 1;

    float step_x = dx / steps;
    float step_y = dy / steps;

    for (int i = 0; i <= steps; i++)
    {
        float curr_x = x0 + i * step_x;
        float curr_y = y0 + i * step_y;

        // Draw thickness by drawing perpendicular line segment
        for (float t = -thickness / 2.0f; t <= thickness / 2.0f; t += 0.1f)
        {
            float offset_x = curr_x + t * px;
            float offset_y = curr_y + t * py;

            // Anti-aliasing based on distance from line center
            float distance_from_center = fabsf(t) / (thickness / 2.0f);
            float intensity = 1.0f - distance_from_center;
            if (intensity < 0.0f)
                intensity = 0.0f;

            set_pixel_f(canvas, offset_x, offset_y, intensity);
        }
    }
}

void canvas_save_pgm(canvas_t *canvas, const char *filename)
{
    if (!canvas || !canvas->pixels || !filename)
        return NULL;

    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }

    // Write PGM header
    fprintf(file, "P2\n");
    fprintf(file, "%d %d\n", canvas->width, canvas->height);
    fprintf(file, "255\n");

    // Write pixel data
    for (int y = 0; y < canvas->height; y++)
    {
        for (int x = 0; x < canvas->width; x++)
        {
            int pixel_value = (int)(canvas->pixels[y][x] * 255.0f);
            if (pixel_value > 255)
                pixel_value = 255;
            fprintf(file, "%d ", pixel_value);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    printf("Canvas saved to %s\n", filename);
}