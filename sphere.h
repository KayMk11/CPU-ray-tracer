#pragma once

#include "hittable.h"

class sphere : public hittable
{
private:
    vec3 center1;
    float mRadius;
    shared_ptr<material> mat;
    bool is_moving;
    vec3 center_vec;
    aabb bbox;

public:
    // Stationary
    sphere(const vec3 &center, float radius, shared_ptr<material> mat) : center1(center), mRadius(radius), mat(mat), is_moving(false)
    {
        auto rvec = vec3(radius, radius, radius);
        bbox = aabb(center1 - rvec, center1 + rvec);
    }
    sphere(const vec3 &center1, const vec3 &center2, float radius, shared_ptr<material> mat) : center1(center1), mRadius(radius), mat(mat), is_moving(true)
    {
        auto rvec = vec3(radius, radius, radius);
        aabb box1(center1 - rvec, center1 + rvec);
        aabb box2(center2 - rvec, center2 + rvec);
        bbox = aabb(box1, box2);
        center_vec = center2 - center1;
    }
    aabb bounding_box() const override { return bbox; }
    virtual bool hit(const ray &r, interval ray_t, hitrecord &record) const override
    {
        vec3 center = is_moving ? sphere_center(r.time()) : center1;
        vec3 oc = center - r.origin();
        float a = r.direction().length_squared();
        float h = dot(r.direction(), oc);
        float c = oc.length_squared() - mRadius * mRadius;

        float discriminant = h * h - a * c;

        if (discriminant < 0)
            return false;

        auto sqrtd = sqrt(discriminant);

        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root))
        {
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        record.t = root;
        record.position = r.at(root);
        vec3 normal = (record.position - center) / mRadius;
        record.setNormal(r, normal);
        get_sphere_uv(normal, record.u, record.v);
        record.mat = mat;
        return true;
    }
    vec3 sphere_center(float time) const
    {
        return center1 + time * center_vec;
    }
    static void get_sphere_uv(const vec3 &p, float &u, float &v)
    {
        auto theta = acosf(-p.y);
        auto phi = atan2f(-p.z, p.x) + PI;
        u = phi / (2 * PI);
        v = theta / PI;
    }
};
