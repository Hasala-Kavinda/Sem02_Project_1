#ifndef LIGHTING_H
#define LIGHTING_H

#include "math3d.h"

/**
 * Represents a directional light source in the scene.
 */
typedef struct
{
    vec3_t direction; // The direction vector of the light.
    float intensity;  // The brightness of the light ( between 0 and 1).
} light_t;

//   Computes the total light intensity on an edge based on Lambert's cosine law.
float compute_lighting(vec3_t edge_dir, light_t *lights, int num_lights);

#endif // LIGHTING_H
