#ifndef CANVAS_H
#define CANVAS_H

#include <stdint.h>

typedef struct
{
    int width;      // Canvas width in pixels
    int height;     // Canvas height in pixels
    float **pixels; // 2D array of brightness values [y][x]
} canvas_t;

canvas_t *canvas_create(int width, int height);
void canvas_destroy(canvas_t *canvas);
void canvas_clear(canvas_t *canvas);
void set_pixel_f(canvas_t *canvas, float x, float y, float intensity);

void draw_line_f(canvas_t *canvas, float x0, float y0, float x1, float y1, float thickness);
void canvas_save_pgm(canvas_t *canvas, const char *filename);

#endif