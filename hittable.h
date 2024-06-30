#pragma once
#include "aabb.h"
class material;
struct hitrecord
{
    vec3 position;
    vec3 normal;
    shared_ptr<material> mat;
    float t;
    float u;
    float v;
    bool frontface;

    void setNormal(const ray &r, const vec3 &outwardNormal)
    {
        frontface = dot(r.direction(), outwardNormal) < 0;
        normal = frontface ? outwardNormal : -outwardNormal;
    }
};

class hittable
{
public:
    virtual ~hittable() = default;
    virtual bool hit(const ray &r, interval ray_t, hitrecord &record) const = 0;
    virtual aabb bounding_box() const = 0;
};

class translate : public hittable
{
    shared_ptr<hittable> object;
    vec3 offset;
    aabb bbox;

public:
    translate(shared_ptr<hittable> object, const vec3 &offset) : object(object), offset(offset)
    {
        bbox = object->bounding_box() + offset;
    }
    aabb bounding_box() const override
    {
        return bbox;
    }
    bool hit(const ray &r, interval ray_t, hitrecord &rec) const override
    {
        ray offset_r(r.origin() - offset, r.direction(), r.time());
        if (!object->hit(offset_r, ray_t, rec))
            return false;
        rec.position += offset;
        return true;
    }
};

class rotate_y : public hittable
{
    shared_ptr<hittable> object;
    float sin_theta, cos_theta;
    aabb bbox;

public:
    rotate_y(shared_ptr<hittable> object, float angle) : object(object)
    {
        auto radians = to_radians(angle);

        sin_theta = sin(radians);
        cos_theta = cos(radians);

        bbox = object->bounding_box();

        vec3 min(infinity, infinity, infinity);
        vec3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int k = 0; k < 2; k++)
                {
                    auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
                    auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
                    auto z = k * bbox.z.max + (1 - k) * bbox.z.min;

                    auto newx = cos_theta * x + sin_theta * z;
                    auto newz = -sin_theta * x + cos_theta * z;

                    vec3 tester(newx, y, newz);

                    for (int c = 0; c < 3; c++)
                    {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }
            }
        }
        bbox = aabb(min, max);
    }

    aabb bounding_box() const override { return bbox; }

    bool hit(const ray &r, interval ray_t, hitrecord &rec) const override
    {
        auto origin = r.origin();
        auto direction = r.direction();

        origin.x = cos_theta * r.origin().x - sin_theta * r.origin().z;
        origin.z = sin_theta * r.origin().x + cos_theta * r.origin().z;

        direction.x = cos_theta * r.direction().x - sin_theta * r.direction().z;
        direction.z = sin_theta * r.direction().x + cos_theta * r.direction().z;

        ray rotated_r(origin, direction, r.time());

        if (!object->hit(rotated_r, ray_t, rec))
            return false;

        auto p = rec.position;
        p.x = cos_theta * rec.position.x + sin_theta * rec.position.z;
        p.z = -sin_theta * rec.position.x + cos_theta * rec.position.z;

        auto normal = rec.normal;
        normal.x = cos_theta * rec.normal.x + sin_theta * rec.normal.z;
        normal.z = -sin_theta * rec.normal.x + cos_theta * rec.normal.z;

        rec.position = p;
        rec.normal = normal;

        return true;
    }
};