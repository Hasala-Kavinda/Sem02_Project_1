#ifndef RENDERER_H
#define RENDERER_H
#include "canvas.h"
#include "math3d.h"

typedef struct
{
    vec3_t *vertices;
    int vertex_count;
    int (*edges)[2];
    int edge_count;
} object3d_t;

vec3_t project_vertex(vec3_t vertex, mat4_t local_to_world, mat4_t world_to_camera, mat4_t projection, int canvas_width, int canvas_height);
int clip_to_render_circular_viewport(canvas_t *canvas, float x, float y);
void wireframe(canvas_t *canvas, object3d_t *obj, mat4_t local_to_world, mat4_t world_to_camera, mat4_t projection);
object3d_t *generate_soccer_ball();

#endif