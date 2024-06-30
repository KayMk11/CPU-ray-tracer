#pragma once

#include "rtw.h"
#include "hittable.h"
#include "texture.h"

class material
{
public:
    virtual ~material() = default;
    virtual bool scatter(const ray &r_in, const hitrecord &rec, color &attenuation, ray &scattered) const
    {
        return false;
    }
    virtual color emitted(float u, float v, const vec3 &p) const
    {
        return color(0, 0, 0);
    }
};

class lambertian : public material
{
    // color albedo;
    shared_ptr<texture> tex;

public:
    lambertian(const color &albedo) : tex(make_shared<solid_color>(albedo)) {}
    lambertian(shared_ptr<texture> tex) : tex(tex) {}

    virtual bool scatter(const ray &r_in, const hitrecord &rec, color &attenuation, ray &scattered) const override
    {
        auto scatter_direction = rec.normal + random_unit_vector();
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;
        scattered = ray(rec.position, scatter_direction, r_in.time());
        attenuation = tex->value(rec.u, rec.v, rec.position);
        return true;
    }
};

class metal : public material
{
    color albedo;
    float fuzziness;

public:
    metal(const color &albedo, float fuzz) : albedo(albedo), fuzziness(fuzz < 1 ? fuzz : 1) {}
    virtual bool scatter(const ray &r_in, const hitrecord &rec, color &attenuation, ray &scattered) const override
    {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit(reflected) + (fuzziness * random_unit_vector());
        scattered = ray(rec.position, reflected, r_in.time());
        attenuation = albedo;
        return dot(scattered.direction(), rec.normal) > 0;
    }
};

class dielectric : public material
{
    float refractive_index;

    static float reflectance(float cosine, float refractive_index)
    {
        auto r0 = (1 - refractive_index) / (1 + refractive_index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }

public:
    dielectric(float refractive_index) : refractive_index(refractive_index) {}
    virtual bool scatter(const ray &r_in, const hitrecord &rec, color &attenuation, ray &scattered) const override
    {
        attenuation = vec3(1., 1., 1.);
        float ri = rec.frontface ? (1. / refractive_index) : refractive_index;
        vec3 unit_direction = unit(r_in.direction());

        auto cos_theta = fminf(dot(-unit_direction, rec.normal), 1.0);
        auto sin_theta = sqrt(1 - cos_theta * cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;

        vec3 direction;
        if (cannot_refract || reflectance(cos_theta, ri) > random_float())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri);

        scattered = ray(rec.position, direction, r_in.time());
        return true;
    }
};

class diffuse_light : public material
{
    shared_ptr<texture> tex;

public:
    diffuse_light(shared_ptr<texture> tex) : tex(tex) {}
    diffuse_light(const color &emit) : tex(make_shared<solid_color>(emit)) {}

    color emitted(float u, float v, const vec3 &p) const override
    {
        return tex->value(u, v, p);
    }
};

class isotropic : public material
{
public:
    isotropic(const color &albedo) : tex(make_shared<solid_color>(albedo)) {}
    isotropic(shared_ptr<texture> tex) : tex(tex) {}

    bool scatter(const ray &r_in, const hitrecord &rec, color &attenuation, ray &scattered)
        const override
    {
        scattered = ray(rec.position, random_unit_vector(), r_in.time());
        attenuation = tex->value(rec.u, rec.v, rec.position);
        return true;
    }

private:
    shared_ptr<texture> tex;
};