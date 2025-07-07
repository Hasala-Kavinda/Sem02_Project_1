#include "lighting.h"
#include <math.h>

/**
 * @brief Computes the total light intensity on an edge from multiple light sources.
 */
float compute_lighting(vec3_t edge_dir, light_t *lights, int num_lights)
{
    if (num_lights <= 0)
    {
        return 0.2f; // Default ambient light if no lights are defined
    }
    // varible to store the lights
    float total_intensity = 0.0f;
    // get the unit vector
    vec3_t normalized_edge_dir = vec3_normalize_fast(edge_dir);

    // Accumulate intensity from each light source
    for (int i = 0; i < num_lights; i++)
    {
        // unit vector og the light direction
        vec3_t normalized_light_dir = vec3_normalize_fast(lights[i].direction);

        // Calculate the dot product (Lambert's cosine law) a.b = a.b.cos theta
        float dot_product = vec3_dot(normalized_edge_dir, normalized_light_dir);

        // Add the contribution of the current light, make sure its not negative by getting the ma with 0
        total_intensity += fmaxf(0.0f, dot_product) * lights[i].intensity;
    }

    // Clamp the final intensity to the [0, 1] range
    return fminf(1.0f, total_intensity);
}
