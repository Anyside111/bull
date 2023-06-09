#include "plane.h"
#include "hit.h"
#include "ray.h"
#include <cfloat>
#include <limits>

// Intersect with the half space defined by the plane.  The plane's normal
// points outside.  If the ray starts on the "inside" side of the plane, be sure
// to record a hit with t=0 as the first entry in hits.
Hit Plane::Intersection(const Ray &ray, int part) const {
//    double dom = dot(ray.direction, normal);
//    double distance = 10e5;
//    if (dom < 10e-8) {
//        distance = dot((x1 - ray.endpoint), normal) / dom;
//    }
//    return {this, distance, 0};
    double distance = 10e5;
    double dom = dot(ray.direction, normal);
    if (std::abs(dom) < 1e-8) { // Ray is parallel to the plane
        return {0, distance, 0};
    }
    distance = dot(normal, x1 - ray.endpoint) / dom;
    if (distance < 0) { // Intersection is behind ray origin
        return {0, 10e5, 0};
    }
    return {this, distance, 0};
}

vec3 Plane::Normal(const vec3 &point, int part) const {
    return normal;
}

// There is not a good answer for the bounding box of an infinite object.
// The safe thing to do is to return a box that contains everything.
Box Plane::Bounding_Box(int part) const {
    Box b;
    b.hi.fill(std::numeric_limits<double>::max());
    b.lo = -b.hi;
    return b;
}

