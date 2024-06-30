#pragma once

#include "rtw.h"

class texture
{
public:
    virtual ~texture() = default;
    virtual color value(float u, float v, const vec3 &point) const = 0;
};

class solid_color : public texture
{
    color albedo;

public:
    solid_color(const color &albedo) : albedo(albedo) {}
    solid_color(float red, float green, float blue) : solid_color(color(red, green, blue)) {}
    color value(float u, float v, const vec3 &point) const override { return albedo; }
};

class checkered_texture : public texture
{
    float inv_scale;
    shared_ptr<texture> even;
    shared_ptr<texture> odd;

public:
    checkered_texture(float scale, shared_ptr<texture> even, shared_ptr<texture> odd) : inv_scale(1.0 / scale), even(even), odd(odd) {}
    checkered_texture(float scale, const color &c1, const color &c2) : inv_scale(1.0 / scale), even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)) {}
    virtual color value(float u, float v, const vec3 &point) const override
    {
        auto xInteger = int(std::floor(inv_scale * point.x));
        auto yInteger = int(std::floor(inv_scale * point.y));
        auto zInteger = int(std::floor(inv_scale * point.z));
        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;
        return isEven ? even->value(u, v, point) : odd->value(u, v, point);
    }
};