#include "canvas.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

// Create a canvas with given width and height
canvas_t *canvas_create(int width, int height)
{
    if (width <= 0 || height <= 0)
        return NULL;

    canvas_t *canvas = (canvas_t *)malloc(sizeof(canvas_t));
    if (!canvas)
        return NULL;

    canvas->width = width;
    canvas->height = height;
    canvas->pixels = (float **)malloc(height * sizeof(float *));
    if (!canvas->pixels)
    {
        free(canvas);
        return NULL;
    }

    for (int y = 0; y < height; y++)
    {
        canvas->pixels[y] = (float *)calloc(width, sizeof(float));
        if (!canvas->pixels[y])
        {
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

// Destroy the canvas and free memory
void canvas_destroy(canvas_t *canvas)
{
    if (!canvas)
        return;
    for (int y = 0; y < canvas->height; y++)
    {
        free(canvas->pixels[y]);
    }
    free(canvas->pixels);
    free(canvas);
}

// Clear the canvas to a specific value
void canvas_clear(canvas_t *canvas, float value)
{
    if (!canvas)
        return;
    for (int y = 0; y < canvas->height; y++)
    {
        for (int x = 0; x < canvas->width; x++)
        {
            canvas->pixels[y][x] = value;
        }
    }
}

// Set pixel brightness at floating-point coordinates with bilinear filtering
void set_pixel_f(canvas_t *canvas, float x, float y, float intensity)
{
    if (!canvas || intensity <= 0.0f || intensity > 1.0f)
        return;

    int x0 = (int)floorf(x);
    int x1 = x0 + 1;
    int y0 = (int)floorf(y);
    int y1 = y0 + 1;

    if (x0 < 0 || x1 >= canvas->width || y0 < 0 || y1 >= canvas->height)
        return;

    float fx = x - x0;
    float fy = y - y0;
    float w00 = (1.0f - fx) * (1.0f - fy);
    float w10 = fx * (1.0f - fy);
    float w01 = (1.0f - fx) * fy;
    float w11 = fx * fy;

    canvas->pixels[y0][x0] += intensity * w00;
    if (canvas->pixels[y0][x0] > 1.0f)
        canvas->pixels[y0][x0] = 1.0f;

    canvas->pixels[y0][x1] += intensity * w10;
    if (canvas->pixels[y0][x1] > 1.0f)
        canvas->pixels[y0][x1] = 1.0f;

    canvas->pixels[y1][x0] += intensity * w01;
    if (canvas->pixels[y1][x0] > 1.0f)
        canvas->pixels[y1][x0] = 1.0f;

    canvas->pixels[y1][x1] += intensity * w11;
    if (canvas->pixels[y1][x1] > 1.0f)
        canvas->pixels[y1][x1] = 1.0f;
}

// Draw a line from (x0, y0) to (x1, y1) with thickness using circular brush and DDA
void draw_line_f(canvas_t *canvas, float x0, float y0, float x1, float y1, float thickness)
{
    if (!canvas)
        return;

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

    int steps = (int)ceilf(length * 2); // Increase steps for smoothness
    float x_increment = dx / steps;
    float y_increment = dy / steps;

    float radius = thickness / 2.0f;

    for (int i = 0; i <= steps; i++)
    {
        float x = x0 + i * x_increment;
        float y = y0 + i * y_increment;

        // Draw circular brush around (x,y)
        for (float dx = -radius; dx <= radius; dx += 0.5f)
        {
            for (float dy = -radius; dy <= radius; dy += 0.5f)
            {
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist <= radius)
                {
                    // Intensity fades linearly from center to edge
                    float intensity = 1.0f - (dist / radius);
                    set_pixel_f(canvas, x + dx, y + dy, intensity);
                }
            }
        }
    }
}

// Save canvas to PGM file
void canvas_save_pgm(canvas_t *canvas, const char *filename)
{
    if (!canvas || !filename)
        return;

    FILE *fp = fopen(filename, "w");
    if (!fp)
        return;

    fprintf(fp, "P2\n%d %d\n255\n", canvas->width, canvas->height);

    for (int y = 0; y < canvas->height; y++)
    {
        for (int x = 0; x < canvas->width; x++)
        {
            int pixel_value = (int)(canvas->pixels[y][x] * 255.0f);
            if (pixel_value > 255)
                pixel_value = 255;
            if (pixel_value < 0)
                pixel_value = 0;
            fprintf(fp, "%d ", pixel_value);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}