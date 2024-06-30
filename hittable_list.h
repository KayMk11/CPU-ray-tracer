#pragma once

#include "rtw.h"
#include "hittable.h"

#include <vector>

class hittable_list : public hittable
{
    aabb bbox;

public:
    std::vector<shared_ptr<hittable>> objects;
    hittable_list() {}
    hittable_list(shared_ptr<hittable> object)
    {
        add(object);
        bbox = aabb(bbox, object->bounding_box());
    }

    void clear() { objects.clear(); }
    void add(shared_ptr<hittable> object) { objects.push_back(object); }

    bool hit(const ray &r, interval ray_t, hitrecord &record) const override
    {
        hitrecord temp;
        bool hit_anything = false;

        auto closest_so_far = ray_t.max;

        for (const auto &object : objects)
        {
            if (object->hit(r, interval(ray_t.min, closest_so_far), temp))
            {
                hit_anything = true;
                closest_so_far = temp.t;
                record = temp;
            }
        }
        return hit_anything;
    }
    aabb bounding_box() const override { return bbox; }
};