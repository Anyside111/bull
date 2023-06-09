#include "render_world.h"
#include "flat_shader.h"
#include "object.h"
#include "light.h"
#include "ray.h"

extern bool disable_hierarchy;

Render_World::Render_World()
        : background_shader(0), ambient_intensity(0), enable_shadows(true),
          recursion_depth_limit(3) {}

Render_World::~Render_World() {
    delete background_shader;
    for (size_t i = 0; i < objects.size(); i++) delete objects[i];
    for (size_t i = 0; i < lights.size(); i++) delete lights[i];
}

// Find and return the Hit structure for the closest intersection.  Be careful
// to ensure that hit.dist>=small_t.
Hit Render_World::Closest_Intersection(const Ray &ray) {
    double distance = 10e5;
    int idx = -1;
    int idx_part;
    for (int i = 0; i < this->objects.size(); i++) {
        auto object = objects[i];
        //modify the part by determing the object type
        if (object->number_parts <= 1) {
            Hit hit = object->Intersection(ray, 0);
            if (hit.dist < distance && hit.dist >= small_t) {
                distance = hit.dist;
                idx = i;
            }
        } else {
            for (int j = 0; j < object->number_parts; j++) {
                Hit hit = object->Intersection(ray, j);
                if (hit.dist >= small_t && hit.dist < distance) {
                    distance = hit.dist;
                    idx = i;
                    idx_part = j;
                }
            }
        }
    }
    if (idx == -1) {
        return {0, 0, 0};
    } else {
        auto object = objects[idx];
        if (object->number_parts <= 1) {
            return {object, distance, 0};
        } else {
            return {object, distance, idx_part};
        }
    }
}

// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2 &pixel_index) {

    Ray ray;
    vec2 cell = camera.Cell_Center(pixel_index);
    vec2 origin = (camera.min + camera.max) / 2;
    vec2 diff = cell - origin;
    vec3 diff_space = {camera.horizontal_vector * diff[0] + camera.vertical_vector * diff[1]};
    vec3 direction = diff_space + camera.look_vector;
    ray = {camera.position, direction};

    vec3 color = Cast_Ray(ray, 1);
    camera.Set_Pixel(pixel_index, Pixel_Color(color));
}

void Render_World::Render() {
    if (!disable_hierarchy)
        Initialize_Hierarchy();

    for (int j = 0; j < camera.number_pixels[1]; j++)
        for (int i = 0; i < camera.number_pixels[0]; i++)
            Render_Pixel(ivec2(i, j));
}

// cast ray and return the color of the closest intersected surface point,
// or the background color if there is no object intersection
vec3 Render_World::Cast_Ray(const Ray &ray, int recursion_depth) {
    vec3 color;
    Hit hit = Closest_Intersection(ray);
    auto object = hit.object;
    if (hit.object and hit.dist < 10e5) {
        vec3 intersection_point = ray.endpoint + hit.dist * ray.direction;
        vec3 normal;
        //no need to modify the part by determing the object type; because it is already done in Closest_Intersection
        normal = object->Normal(intersection_point, hit.part);
        color = object->material_shader->Shade_Surface(ray, intersection_point, normal, recursion_depth);
    } else {
        color = this->background_shader->Shade_Surface(ray, {0, 0, 0}, {0, 0, 0}, recursion_depth);
    }
    return color;
}

void Render_World::Initialize_Hierarchy() {
    // TODO; // Fill in hierarchy.entries; there should be one entry for
    // each part of each object.

    hierarchy.Reorder_Entries();
    hierarchy.Build_Tree();
}
