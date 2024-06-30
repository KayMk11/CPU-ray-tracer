#pragma once
#include "rtw.h"
#include "hittable.h"
#include "hittable_list.h"

class quad : public hittable
{
    vec3 Q, u, v, w, normal;
    float D;
    shared_ptr<material> mat;
    aabb bbox;

public:
    quad(const vec3 &Q, const vec3 &u, const vec3 &v, shared_ptr<material> mat)
        : Q(Q), u(u), v(v), mat(mat)
    {
        auto n = cross(u, v);
        normal = unit(n);
        D = dot(normal, Q);
        w = n / dot(n, n);
        set_bounding_box();
    }

    virtual void set_bounding_box()
    {
        auto bbox_diag1 = aabb(Q, Q + u + v);
        auto bbox_diag2 = aabb(Q + u, Q + v);
        bbox = aabb(bbox_diag1, bbox_diag2);
    }
    aabb bounding_box() const override
    {
        return bbox;
    }
    bool hit(const ray &r, interval ray_t, hitrecord &rec) const override
    {
        auto denom = dot(normal, r.direction());

        // No hit if the ray is parallel to the plane.
        if (fabs(denom) < 1e-8)
            return false;

        // Return false if the hit point parameter t is outside the ray interval.
        auto t = (D - dot(normal, r.origin())) / denom;
        if (!ray_t.contains(t))
            return false;

        auto intersection = r.at(t);
        vec3 planer_hitpt_vector = intersection - Q;
        auto alpha = dot(w, cross(planer_hitpt_vector, v));
        auto beta = dot(w, cross(u, planer_hitpt_vector));

        if (!is_interior(alpha, beta, rec))
            return false;

        rec.t = t;
        rec.position = intersection;
        rec.mat = mat;
        rec.setNormal(r, normal);

        return true;
    }
    virtual bool is_interior(float a, float b, hitrecord &rec) const
    {
        interval unit_interval = interval(0, 1);
        if (!unit_interval.contains(a) || !unit_interval.contains(b))
            return false;

        rec.u = a;
        rec.v = b;
        return true;
    }
};

inline shared_ptr<hittable_list> box(const vec3 &a, const vec3 &b, shared_ptr<material> mat)
{
    auto sides = make_shared<hittable_list>();

    auto min = vec3(fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z));
    auto max = vec3(fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z));

    auto dx = vec3(max.x - min.x, 0, 0);
    auto dy = vec3(0, max.y - min.y, 0);
    auto dz = vec3(0, 0, max.z - min.z);

    sides->add(make_shared<quad>(vec3(min.x, min.y, max.z), dx, dy, mat));  // front
    sides->add(make_shared<quad>(vec3(max.x, min.y, max.z), -dz, dy, mat)); // right
    sides->add(make_shared<quad>(vec3(max.x, min.y, min.z), -dx, dy, mat)); // back
    sides->add(make_shared<quad>(vec3(min.x, min.y, min.z), dz, dy, mat));  // left
    sides->add(make_shared<quad>(vec3(min.x, max.y, max.z), dx, -dz, mat)); // top
    sides->add(make_shared<quad>(vec3(min.x, min.y, min.z), dx, dz, mat));  // bottom
    return sides;
}