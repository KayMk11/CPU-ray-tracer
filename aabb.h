#pragma once

#include "rtw.h"

class aabb
{
    void pad_to_minimum()
    {
        float delta = 0.0001;
        if (x.size() < delta)
            x = x.expand(delta);
        if (y.size() < delta)
            y = y.expand(delta);
        if (z.size() < delta)
            z = z.expand(delta);
    }

public:
    interval x, y, z;
    aabb() {}
    aabb(const interval &x, const interval &y, const interval &z) : x(x), y(y), z(z)
    {
        pad_to_minimum();
    }

    aabb(const vec3 &a, const vec3 &b)
    {
        x = interval(fminf(a.x, b.x), fmaxf(a.x, b.x));
        y = interval(fminf(a.y, b.y), fmaxf(a.y, b.y));
        z = interval(fminf(a.z, b.z), fmaxf(a.z, b.z));
        pad_to_minimum();
    }
    aabb(const aabb &box0, const aabb &box1)
    {
        x = interval(box0.x, box1.x);
        y = interval(box0.y, box1.y);
        z = interval(box0.z, box1.z);
    }

    const interval &axis_interval(int n) const
    {
        if (n == 1)
            return y;
        if (n == 2)
            return z;
        return x;
    }

    bool hit(const ray &r, interval ray_t) const
    {
        const vec3 ray_orig = r.origin();
        const vec3 &ray_dir = r.direction();

        for (int axis = 0; axis < 3; axis++)
        {
            const interval &ax = axis_interval(axis);
            const float adinv = 1.0 / ray_dir[axis];

            auto t0 = (ax.min - ray_orig[axis]) * adinv;
            auto t1 = (ax.max - ray_orig[axis]) * adinv;

            if (t0 < t1)
            {
                if (t0 > ray_t.min)
                    ray_t.min = t0;
                if (t1 < ray_t.max)
                    ray_t.max = t1;
            }
            else
            {
                if (t1 > ray_t.min)
                    ray_t.min = t1;
                if (t0 < ray_t.max)
                    ray_t.max = t0;
            }

            if (ray_t.max <= ray_t.min)
                return false;
        }
        return true;
    }
};

aabb operator+(const aabb &bbox, const vec3 &offset)
{
    return aabb(bbox.x + offset.x, bbox.y + offset.y, bbox.z + offset.z);
}

aabb operator+(const vec3 &offset, const aabb &bbox)
{
    return bbox + offset;
}