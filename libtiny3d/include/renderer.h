#ifndef RENDERER_H
#define RENDERER_H

#include "canvas.h"
#include "math3d.h"
#include "lighting.h" // Include lighting header

// Represents a 3D object with vertices and edges
typedef struct
{
    vec3_t *vertices;
    int (*edges)[2];
    int vertex_count;
    int edge_count;
} object3d_t;

// Projects a 3D vertex to 2D screen coordinates
vec3_t project_vertex(vec3_t vertex, mat4_t local_to_world, mat4_t world_to_camera, mat4_t projection, int canvas_width, int canvas_height);

// Checks if the (x, y) screen coordinate lies within the circular viewport
int clip_to_circular_viewport(canvas_t *canvas, float x, float y);

// Renders a 3D object as a wireframe with depth sorting and lighting using logarithmic z-buffer
void wireframe(canvas_t *canvas, object3d_t *obj,
               mat4_t local_to_world, mat4_t world_to_camera, mat4_t projection,
               light_t *lights, int num_lights,
               float z_near, float z_far);

// Generates a 3D soccer ball object (truncated icosahedron)
object3d_t *generate_soccer_ball();
object3d_t *generate_letter_P();
#endif // RENDERER_H