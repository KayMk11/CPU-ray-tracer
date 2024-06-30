#pragma once

#include "rtw.h"
#include "hittable.h"

class camera
{
public:
    double aspect_ratio = 1.0;
    int image_width = 100;
    int samples_per_pixel = 10;
    int max_depth = 10;
    color background;

    float vfov = 90;
    vec3 lookfrom = vec3(0, 0, 0);
    vec3 lookat = vec3(0, 0, -1);
    vec3 vup = vec3(0, 1, 0);

    float defocus_angle = 0;
    float focus_dist = 10;

    void render(const hittable &world)
    {
        initialize();
        // Render
        std::cout << "P3\n"
                  << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++)
        {
            std::clog << "Scanlines remaining: " << image_height - j << std::endl;
            for (int i = 0; i < image_width; i++)
            {
                color px(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; sample++)
                {
                    ray r = get_ray(i, j);
                    px += ray_color(r, max_depth, world);
                }
                write_color(std::cout, pixel_sample_scale * px);
            }
        }
        std::clog << "Done." << std::endl;
    }

private:
    int image_height;
    float pixel_sample_scale;
    vec3 camera_center;
    vec3 pixel00_loc;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    vec3 u, v, w;
    vec3 defocus_disk_u;
    vec3 defocus_disk_v;

    void initialize()
    {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_sample_scale = 1.0 / samples_per_pixel;

        camera_center = lookfrom;

        auto theta = to_radians(vfov);
        auto h = tan(theta / 2);
        float viewport_height = 2 * h * focus_dist;
        float viewport_width = viewport_height * (float(image_width) / image_height);

        w = unit(lookfrom - lookat);
        u = unit(cross(vup, w));
        v = cross(w, u);

        vec3 viewport_u = viewport_width * u;
        vec3 viewport_v = -viewport_height * v;

        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        vec3 viewport_upper_left = camera_center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + .5 * (pixel_delta_u + pixel_delta_v);

        auto defocus_radius = focus_dist * tan(to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j) const
    {
        auto offset = sample_square();
        auto pixel_sample = pixel00_loc +
                            ((i + offset.x) * pixel_delta_u) +
                            ((j + offset.y) * pixel_delta_v);
        auto ray_origin = (defocus_angle <= 0) ? camera_center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;
        auto ray_time = random_float();

        return ray(ray_origin, ray_direction, ray_time);
    }

    vec3 defocus_disk_sample() const
    {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return camera_center + (p.x * defocus_disk_u) + (p.y * defocus_disk_v);
    }

    vec3 sample_square() const
    {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return vec3(random_float() - 0.5, random_float() - 0.5, 0);
    }

    color ray_color(const ray &r, int depth, const hittable &world)
    {
        if (depth <= 0)
            return color(0, 0, 0);
        hitrecord record;
        if (!world.hit(r, interval(0.001, infinity), record))
        {
            return background;
        }
        ray scattered;
        color attenuation;
        color color_from_emission = record.mat->emitted(record.u, record.v, record.position);

        if (!record.mat->scatter(r, record, attenuation, scattered))
            return color_from_emission;

        color color_from_scatter = attenuation * ray_color(scattered, depth - 1, world);
        return color_from_emission + color_from_scatter;
    }
};