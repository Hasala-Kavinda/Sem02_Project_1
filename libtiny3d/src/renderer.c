#include "renderer.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Project a 3D vertex through the full transformation pipeline
vec3_t project_vertex(vec3_t vertex, mat4_t local_to_world, mat4_t world_to_camera, mat4_t projection, int canvas_width, int canvas_height)
{
    // Local to World
    vec3_t world_pos = mat4_transform_vec3(local_to_world, vertex);

    // World to Camera
    vec3_t camera_pos = mat4_transform_vec3(world_to_camera, world_pos);

    // Camera to Projection (homogeneous coordinates)
    vec4_t proj_pos = mat4_transform_vec4(projection, (vec4_t){camera_pos.x, camera_pos.y, camera_pos.z, 1.0f});

    // Perspective divide
    if (fabsf(proj_pos.w) > 0.0001f)
    {
        proj_pos.x /= proj_pos.w;
        proj_pos.y /= proj_pos.w;
        proj_pos.z /= proj_pos.w;
    }

    // Map to screen coordinates
    float screen_x = (proj_pos.x * 0.5f + 0.5f) * canvas_width;
    float screen_y = (1.0f - (proj_pos.y * 0.5f + 0.5f)) * canvas_height;

    return vec3_create(screen_x, screen_y, proj_pos.z);
}

// Check if a pixel is inside the circular viewport
int clip_to_render_circular_viewport(canvas_t *canvas, float x, float y)
{
    if (!canvas)
        return 0;

    float center_x = canvas->width / 2.0f;
    float center_y = canvas->height / 2.0f;
    float radius = fminf(canvas->width, canvas->height) / 2.0f;

    float dx = x - center_x;
    float dy = y - center_y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

// Draw wireframe with depth sorting
void wireframe(canvas_t *canvas, object3d_t *obj, mat4_t local_to_world, mat4_t world_to_camera, mat4_t projection)
{
    if (!canvas || !obj)
        return;

    // Project all vertices
    vec3_t *projected = (vec3_t *)malloc(obj->vertex_count * sizeof(vec3_t));
    float *depths = (float *)malloc(obj->edge_count * sizeof(float));
    int *edge_indices = (int *)malloc(obj->edge_count * sizeof(int));

    for (int i = 0; i < obj->vertex_count; i++)
    {
        projected[i] = project_vertex(obj->vertices[i], local_to_world, world_to_camera, projection, canvas->width, canvas->height);
    }

    // Calculate average depth for each edge
    for (int i = 0; i < obj->edge_count; i++)
    {
        int v0 = obj->edges[i][0];
        int v1 = obj->edges[i][1];
        depths[i] = (projected[v0].z + projected[v1].z) / 2.0f;
        edge_indices[i] = i;
    }

    // Sort edges by depth (back to front)
    for (int i = 0; i < obj->edge_count - 1; i++)
    {
        for (int j = 0; j < obj->edge_count - i - 1; j++)
        {
            if (depths[j] < depths[j + 1])
            {
                float temp = depths[j];
                depths[j] = depths[j + 1];
                depths[j + 1] = temp;
                int temp_idx = edge_indices[j];
                edge_indices[j] = edge_indices[j + 1];
                edge_indices[j + 1] = temp_idx;
            }
        }
    }

    // Draw edges
    for (int i = 0; i < obj->edge_count; i++)
    {
        int idx = edge_indices[i];
        int v0 = obj->edges[idx][0];
        int v1 = obj->edges[idx][1];

        // Only draw if both endpoints are in the circular viewport
        if (clip_to_render_circular_viewport(canvas, projected[v0].x, projected[v0].y) ||
            clip_to_render_circular_viewport(canvas, projected[v1].x, projected[v1].y))
        {
            draw_line_f(canvas, projected[v0].x, projected[v0].y, projected[v1].x, projected[v1].y, 1.0f);
        }
    }

    free(projected);
    free(depths);
    free(edge_indices);
}

// Generate soccer ball (truncated icosahedron)
object3d_t *generate_soccer_ball()
{
    object3d_t *obj = (object3d_t *)malloc(sizeof(object3d_t));
    obj->vertex_count = 60;
    obj->edge_count = 90;

    obj->vertices = (vec3_t *)malloc(obj->vertex_count * sizeof(vec3_t));
    obj->edges = (int (*)[2])malloc(obj->edge_count * sizeof(int[2]));

    // Vertex coordinates from the provided JSON data
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

    // Load vertices
    for (int i = 0; i < obj->vertex_count; i++)
    {
        obj->vertices[i] = vec3_create(vertices_data[i * 3], vertices_data[i * 3 + 1], vertices_data[i * 3 + 2]);
    }

    // Edge list from the provided JSON data
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

    // Load edges
    for (int i = 0; i < obj->edge_count; i++)
    {
        obj->edges[i][0] = edges_data[i * 2];
        obj->edges[i][1] = edges_data[i * 2 + 1];
    }

    return obj;
}
