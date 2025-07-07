#ifndef CANVAS_H
#define CANVAS_H

#include <stdio.h>

// canvas structure that store the canvas data
typedef struct
{
    int width;
    int height;
    float **pixels; // a 2D array to store the pixel values
} canvas_t;

// Create a canvas with given width and height
canvas_t *canvas_create(int width, int height);

// Destroy the canvas and free memory
void canvas_destroy(canvas_t *canvas);

// Clear the canvas to a specific value
void canvas_clear(canvas_t *canvas, float value);

// Set pixel brightness at floating-point coordinates with bilinear filtering
void set_pixel_f(canvas_t *canvas, float x, float y, float intensity);

// Draw a line from (x0, y0) to (x1, y1) with thickness and intensity
void draw_line_f(canvas_t *canvas, float x0, float y0, float x1, float y1, float thickness, float intensity);

// Save canvas to PGM file
void canvas_save_pgm(canvas_t *canvas, const char *filename);

#endif // CANVAS_H
