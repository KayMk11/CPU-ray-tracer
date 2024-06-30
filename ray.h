#pragma once
#include "vec3.h"

class ray
{
    vec3 orig;
    vec3 dir;
    float tm;

public:
    ray() {}
    ray(const vec3 &origin, const vec3 &direction) : orig(origin), dir(direction), tm(0) {}
    ray(const vec3 &origin, const vec3 &direction, float time) : orig(origin), dir(direction), tm(time) {}

    const vec3 &origin() const { return orig; }
    const vec3 &direction() const { return dir; }
    const float time() const { return tm; }

    vec3 at(float t) const
    {
        return orig + t * dir;
    }
};