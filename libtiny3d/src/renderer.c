#include "renderer.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

// project a 3d vertex with full transformation
vec3_t project_vertex(vec3_t vertex, mat4_t local_to_world, mat4_t world_to_camera, mat4_t projection, int canvas_width, int canvas_height)
{
    // Local to World -> World to Camera -> Camera to Projection
    mat4_t mvp = mat4_multiply(projection, mat4_multiply(world_to_camera, local_to_world));
    // apply the tarnsformation to the vertex
    vec4_t clip_pos = mat4_transform_vec4(mvp, (vec4_t){vertex.x, vertex.y, vertex.z, 1.0f});

    // // makng sure that w is not zero and convert back to standard coordinates
    if (fabsf(clip_pos.w) > 0.0000001)
    {
        // vertex is converted to homogeneous coordinates (adding w=1.0) and transformed by the MVP matrix, resulting in clip space coordinates.
        clip_pos.x /= clip_pos.w;
        clip_pos.y /= clip_pos.w;
        clip_pos.z /= clip_pos.w;
    }

    // Map to scsreen coordinates (Viewport transform)
    float screen_x = (clip_pos.x * 0.5f + 0.5f) * canvas_width;
    float screen_y = (1.0f - (clip_pos.y * 0.5f + 0.5f)) * canvas_height;

    // returns the vector
    return vec3_create(screen_x, screen_y, clip_pos.z);
}

// Checks if a pixel (x, y) is inside a circular drawing area defined by the canvas
int clip_to_circular_viewport(canvas_t *canvas, float x, float y)
{
    if (!canvas)
        return 0;

    // Calculate the center of the canvas
    float center_x = canvas->width * 0.5f;
    float center_y = canvas->height * 0.5f;

    // check whether the radius is the minimum of half of the width or half of the height to make sure that the circle is fit to the canvas
    float radius = fminf(canvas->width * 0.5f, canvas->height * 0.5f);

    // Calculate the distance from the pixel (x, y) to the center
    float dx = x - center_x;
    float dy = y - center_y;
    // calculate the distances
    float distance = sqrtf(dx * dx + dy * dy);

    // Return 1 if the pixel is inside the circular viewport, 0 otherwise
    return distance <= radius;
}

// Draw wireframe with depth sorting and lighting
// Draw wireframe using logarithmic z-buffer
void wireframe(canvas_t *canvas, object3d_t *obj,
               mat4_t local_to_world, mat4_t world_to_camera, mat4_t projection,
               light_t *lights, int num_lights,
               float z_near, float z_far)
{
    if (!canvas || !obj)
        return;

    // a array for store 2D projected vertices
    vec3_t *projected_vertices = (vec3_t *)malloc(obj->vertex_count * sizeof(vec3_t));

    float *edge_depths = (float *)malloc(obj->edge_count * sizeof(float));
    int *sorted_edge_indices = (int *)malloc(obj->edge_count * sizeof(int));

    for (int i = 0; i < obj->vertex_count; i++)
    {
        projected_vertices[i] = project_vertex(obj->vertices[i], local_to_world, world_to_camera, projection, canvas->width, canvas->height);
    }

    // get the log values
    float log_z_near = logf(z_near + 1.0f); // 1 to avoid log 0
    float log_z_far = logf(z_far + 1.0f);

    for (int i = 0; i < obj->edge_count; i++)
    {
        int v0_idx = obj->edges[i][0];
        int v1_idx = obj->edges[i][1];

        // Local to World -> World to Camera  transformation
        vec3_t v0_world = mat4_transform_vec3(local_to_world, obj->vertices[v0_idx]);
        vec3_t v1_world = mat4_transform_vec3(local_to_world, obj->vertices[v1_idx]);

        vec3_t v0_camera = mat4_transform_vec3(world_to_camera, v0_world);
        vec3_t v1_camera = mat4_transform_vec3(world_to_camera, v1_world);

        float z0 = fabsf(v0_camera.z);
        float z1 = fabsf(v1_camera.z);
        float avg_z = (z0 + z1) * 0.5f;

        // from the formula
        edge_depths[i] = (logf(avg_z + 1.0f) - log_z_near) / (log_z_far - log_z_near);
        sorted_edge_indices[i] = i;
    }

    // Bubble sort edges back to front
    for (int i = 0; i < obj->edge_count - 1; i++)
    {
        for (int j = 0; j < obj->edge_count - i - 1; j++)
        {
            if (edge_depths[j] < edge_depths[j + 1])
            {
                float tmp_depth = edge_depths[j];
                edge_depths[j] = edge_depths[j + 1];
                edge_depths[j + 1] = tmp_depth;

                int tmp_idx = sorted_edge_indices[j];
                sorted_edge_indices[j] = sorted_edge_indices[j + 1];
                sorted_edge_indices[j + 1] = tmp_idx;
            }
        }
    }

    // Draw sorted edges
    for (int i = 0; i < obj->edge_count; i++)
    {
        int edge_idx = sorted_edge_indices[i];
        int v0_idx = obj->edges[edge_idx][0];
        int v1_idx = obj->edges[edge_idx][1];

        // transformation local to wolrd
        vec3_t v0_world = mat4_transform_vec3(local_to_world, obj->vertices[v0_idx]);
        vec3_t v1_world = mat4_transform_vec3(local_to_world, obj->vertices[v1_idx]);
        // vector of the edge
        vec3_t edge_dir = vec3_sub(v1_world, v0_world);

        // calculate the light
        float intensity = compute_lighting(edge_dir, lights, num_lights);

        //
        vec3_t p0 = projected_vertices[v0_idx];
        vec3_t p1 = projected_vertices[v1_idx];

        // cliping and draw
        if (clip_to_circular_viewport(canvas, p0.x, p0.y) &&
            clip_to_circular_viewport(canvas, p1.x, p1.y))
        {
            draw_line_f(canvas, p0.x, p0.y, p1.x, p1.y, 1.5f, intensity);
        }
    }

    // free the memory
    free(projected_vertices);
    free(edge_depths);
    free(sorted_edge_indices);
}

// Generate soccer ball (truncated icosahedron)
object3d_t *generate_soccer_ball()
{
    // allocate the memory for the object
    object3d_t *obj = (object3d_t *)malloc(sizeof(object3d_t));
    // set the object values
    obj->vertex_count = 60;
    obj->edge_count = 90;

    // set the object structure values
    obj->vertices = (vec3_t *)malloc(obj->vertex_count * sizeof(vec3_t));
    obj->edges = (int (*)[2])malloc(obj->edge_count * sizeof(int[2]));

    // Vertex coordinates from data file
    float vertices_data[] = {
        0, 0, 1.021, 0.4035482, 0, 0.9378643, -0.2274644, 0.3333333, 0.9378643,
        -0.1471226, -0.375774, 0.9378643, 0.579632, 0.3333333, 0.7715933, 0.5058321, -0.375774, 0.8033483,
        -0.6020514, 0.2908927, 0.7715933, -0.05138057, 0.6666667, 0.7715933, 0.1654988, -0.6080151, 0.8033483,
        -0.5217096, -0.4182147, 0.7715933, 0.8579998, 0.2908927, 0.4708062, 0.3521676, 0.6666667, 0.6884578,
        0.7841999, -0.4182147, 0.5025612, -0.657475, 0.5979962, 0.5025612, -0.749174, -0.08488134, 0.6884578,
        -0.3171418, 0.8302373, 0.5025612, 0.1035333, -0.8826969, 0.5025612, -0.5836751, -0.6928964, 0.4708062,
        0.8025761, 0.5979962, 0.2017741, 0.9602837, -0.08488134, 0.3362902, 0.4899547, 0.8302373, 0.3362902,
        0.7222343, -0.6928964, 0.2017741, -0.8600213, 0.5293258, 0.1503935, -0.9517203, -0.1535518, 0.3362902,
        -0.1793548, 0.993808, 0.1503935, 0.381901, -0.9251375, 0.2017741, -0.2710537, -0.9251375, 0.3362902,
        -0.8494363, -0.5293258, 0.2017741, 0.8494363, 0.5293258, -0.2017741, 1.007144, -0.1535518, -0.06725804,
        0.2241935, 0.993808, 0.06725804, 0.8600213, -0.5293258, -0.1503935, -0.7222343, 0.6928964, -0.2017741,
        -1.007144, 0.1535518, 0.06725804, -0.381901, 0.9251375, -0.2017741, 0.1793548, -0.993808, -0.1503935,
        -0.2241935, -0.993808, -0.06725804, -0.8025761, -0.5979962, -0.2017741, 0.5836751, 0.6928964, -0.4708062,
        0.9517203, 0.1535518, -0.3362902, 0.2710537, 0.9251375, -0.3362902, 0.657475, -0.5979962, -0.5025612,
        -0.7841999, 0.4182147, -0.5025612, -0.9602837, 0.08488134, -0.3362902, -0.1035333, 0.8826969, -0.5025612,
        0.3171418, -0.8302373, -0.5025612, -0.4899547, -0.8302373, -0.3362902, -0.8579998, -0.2908927, -0.4708062,
        0.5217096, 0.4182147, -0.7715933, 0.749174, 0.08488134, -0.6884578, 0.6020514, -0.2908927, -0.7715933,
        -0.5058321, 0.375774, -0.8033483, -0.1654988, 0.6080151, -0.8033483, 0.05138057, -0.6666667, -0.7715933,
        -0.3521676, -0.6666667, -0.6884578, -0.579632, -0.3333333, -0.7715933, 0.1471226, 0.375774, -0.9378643,
        0.2274644, -0.3333333, -0.9378643, -0.4035482, 0, -0.9378643, 0, 0, -1.021};

    // create vectors for each vertex in the object file
    for (int i = 0; i < obj->vertex_count; i++)
    {
        obj->vertices[i] = vec3_create(vertices_data[i * 3], vertices_data[i * 3 + 1], vertices_data[i * 3 + 2]);
    }

    // Edge list from data file
    int edges_data[] = {
        13, 15, 19, 29, 20, 30, 41, 50, 35, 45, 32, 22, 56, 52, 12, 5, 48, 56, 58, 51, 40, 30,
        57, 53, 16, 26, 33, 43, 33, 22, 21, 31, 8, 5, 0, 3, 2, 6, 2, 7, 13, 6, 10, 18, 46, 54,
        13, 22, 49, 50, 58, 59, 45, 53, 12, 21, 57, 59, 1, 5, 28, 39, 36, 46, 41, 31, 35, 36,
        26, 36, 32, 42, 17, 26, 0, 2, 57, 50, 17, 9, 54, 55, 10, 19, 16, 8, 1, 4, 25, 21,
        40, 44, 18, 20, 10, 4, 42, 51, 24, 30, 34, 44, 24, 34, 11, 7, 0, 1, 25, 35, 14, 23,
        41, 45, 8, 3, 55, 47, 56, 59, 49, 39, 24, 15, 48, 38, 37, 46, 40, 38, 19, 12, 48, 49,
        15, 7, 58, 55, 27, 23, 29, 39, 29, 31, 11, 4, 9, 3, 51, 52, 18, 28, 11, 20, 28, 38,
        9, 14, 16, 25, 33, 23, 17, 27, 27, 37, 52, 44, 37, 47, 43, 47, 42, 43, 32, 34, 14, 6,
        53, 54};

    // set the edges of object from the array
    for (int i = 0; i < obj->edge_count; i++)
    {
        obj->edges[i][0] = edges_data[i * 2];
        obj->edges[i][1] = edges_data[i * 2 + 1];
    }

    return obj;
}
