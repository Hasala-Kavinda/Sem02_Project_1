#include "canvas.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Create a canvas with given width and height
canvas_t *canvas_create(int width, int height)
{
    // if width and height is not in the
    if (width <= 0 || height <= 0)
        return NULL;

    // a pointer to a canvas structure
    canvas_t *canvas = (canvas_t *)malloc(sizeof(canvas_t));
    if (!canvas)
        return NULL;

    // assgin width and height to the canvas object  values
    canvas->width = width;
    canvas->height = height;

    // create arrays of float values
    canvas->pixels = (float **)malloc(height * sizeof(float *));
    // free the canvas if the array memory allocation was not successful
    if (!canvas->pixels)
    {
        free(canvas);
        return NULL;
    }

    for (int y = 0; y < height; y++)
    {
        // assign each array with dynamically allocated values intially 0
        canvas->pixels[y] = (float *)calloc(width, sizeof(float));

        // if it fails free the height array and the canvas
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

//  free the canvas
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

// Set pixels to a specific value
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

// Set pixel intensity at floating-point coordinates with bilinear filtering
void set_pixel_f(canvas_t *canvas, float x, float y, float intensity)
{
    if (!canvas || intensity <= 0.0f)
        return;

    // get the nearest pixel values
    int x0 = (int)floorf(x);
    int x1 = x0 + 1;
    int y0 = (int)floorf(y);
    int y1 = y0 + 1;

    if (x0 < 0 || x1 >= canvas->width || y0 < 0 || y1 >= canvas->height)
        return;

    //
    float fx = x - x0;
    float fy = y - y0;

    // calculate the weight values
    float w00 = (1.0f - fx) * (1.0f - fy);
    float w10 = fx * (1.0f - fy);
    float w01 = (1.0f - fx) * fy;
    float w11 = fx * fy;

    //
    canvas->pixels[y0][x0] += intensity * w00;
    // clamping the wieght to 1 if it exceds 1
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

// Draw a line from (x0, y0) to (x1, y1) with thickness and intensity
void draw_line_f(canvas_t *canvas, float x0, float y0, float x1, float y1, float thickness, float intensity)
{
    if (!canvas || intensity <= 0.0f)
        return;

    // calcluate the dx
    float dx = x1 - x0;
    // printf("%f", dx);

    // calculate dy
    float dy = y1 - y0;
    // get the lenth
    float len = sqrtf(dx * dx + dy * dy);
    // get the radius of the ticknexss
    float radius = thickness / 2.0f;

    // if the length of line is 0 draw a point if it should be visible
    if (len == 0.0f)
    {
        for (float t = -radius; t <= radius; t += 0.5f)
        {
            for (float s = -radius; s <= radius; s += 0.5f)
            {
                if (s * s + t * t <= radius * radius)
                {
                    // set the intensity of the point form the middle of it
                    set_pixel_f(canvas, x0 + s, y0 + t, intensity);
                }
            }
        }
        return;
    }

    // calculate the  number of steps (multiply by 2 to increase the step count so that it will be more smooth)
    int steps = (int)ceilf(len * 2.0f);
    float x_increment = dx / steps;
    float y_increment = dy / steps;

    for (int i = 0; i <= steps; i++)
    {
        float x = x0 + i * x_increment;
        float y = y0 + i * y_increment;

        // make the line smooth
        // Draw a circular brush at each step along the line
        for (float brush_dx = -radius; brush_dx <= radius; brush_dx += 0.5f)
        {
            for (float brush_dy = -radius; brush_dy <= radius; brush_dy += 0.5f)
            {
                // area of the point
                float dist_sq = brush_dx * brush_dx + brush_dy * brush_dy;
                if (dist_sq <= radius * radius)
                {

                    float dist = sqrtf(dist_sq);
                    // fade the brush from center to its edge
                    // At the center (dist=0) : Full intensity (1.0))
                    // At the edge  (dist=r) : Zero intensity (0.0))

                    float brush_intensity = 1.0f - (dist / radius);
                    // set the intensity
                    set_pixel_f(canvas, x + brush_dx, y + brush_dy, intensity * brush_intensity);
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
    // open the file
    FILE *fp = fopen(filename, "wb");
    // check if it is successful it is null
    if (!fp)
        return;

    // print the pgm header
    fprintf(fp, "P2\n%d %d\n255\n", canvas->width, canvas->height);

    // print the rows
    for (int y = 0; y < canvas->height; y++)
    {
        // print the columns
        for (int x = 0; x < canvas->width; x++)
        {
            // calculate the reavant value for in 0 -255 range
            int pixel_value = (int)(canvas->pixels[y][x] * 255.0f);
            // clamping the value to 255
            if (pixel_value > 255)
                pixel_value = 255;
            fprintf(fp, "%d ", pixel_value);
        }
        fprintf(fp, "\n");
    }
    // close the file
    fclose(fp);
}
