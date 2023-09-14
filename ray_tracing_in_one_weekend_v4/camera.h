#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "pdf.h"
#include <iostream>

class camera {
public:
    /* Public Camera Parameters Here */

    double aspect_ratio = 1.0;  // Ratio of image width over height
    int    image_width = 100;  // Rendered image width in pixel count
    int    samples_per_pixel = 10;   // Count of random samples for each pixel
    int    max_depth = 10;   // Maximum number of ray bounces into scene
    color  background;               // Scene background color

    double vfov = 90;  // Vertical view angle (field of view)
    point3 lookfrom = point3(0, 0, -1);  // Point camera is looking from
    point3 lookat = point3(0, 0, 0);   // Point camera is looking at
    vec3   vup = vec3(0, 1, 0);     // Camera-relative "up" direction

    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus

    void render(const hittable& world) {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; ++j) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i) {
                color pixel_color(0, 0, 0);
                for (int s_j = 0; s_j < sqrt_spp; ++s_j) {
                    for (int s_i = 0; s_i < sqrt_spp; ++s_i) {
                        ray r = get_ray(i, j, s_i, s_j);
                        pixel_color += ray_color(r, max_depth, world);
                    }
                }
                write_color(std::cout, pixel_color, samples_per_pixel);
            }
        }

        std::clog << "\rDone.                 \n";
    }

    void render(const hittable& world, const hittable& lights) {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; ++j) {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i) {
                color pixel_color(0, 0, 0);
                for (int s_j = 0; s_j < sqrt_spp; ++s_j) {
                    for (int s_i = 0; s_i < sqrt_spp; ++s_i) {
                        ray r = get_ray(i, j, s_i, s_j);
                        pixel_color += ray_color(r, max_depth, world, lights);
                    }
                }
                write_color(std::cout, pixel_color, samples_per_pixel);
            }
        }

        std::clog << "\rDone.                 \n";
    }


    /* Private Camera Variables Here */
private:
    int    image_height;   // Rendered image height
    point3 center;         // Camera center
    int    sqrt_spp;        // Square root of number of samples per pixel
    double recip_sqrt_spp;  // 1 / sqrt_spp
    point3 pixel00_loc;    // Location of pixel 0, 0
    vec3   pixel_delta_u;  // Offset to pixel to the right
    vec3   pixel_delta_v;  // Offset to pixel below
    vec3   u, v, w;        // Camera frame basis vectors
    vec3   defocus_disk_u;  // Defocus disk horizontal radius
    vec3   defocus_disk_v;  // Defocus disk vertical radius

    void initialize() {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = lookfrom;

        // Determine viewport dimensions.
        //auto focal_length = (lookfrom - lookat).length();
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

        sqrt_spp = int(sqrt(samples_per_pixel));
        recip_sqrt_spp = 1.0 / sqrt_spp;

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
        vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j, int s_i, int s_j) const {
        // Get a randomly-sampled camera ray for the pixel at location i,j, originating from
        // the camera defocus disk, and randomly sampled around the pixel location.

        auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        auto pixel_sample = pixel_center + pixel_sample_square(s_i, s_j);
        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;
        auto ray_time = random_double();

        return ray(ray_origin, ray_direction, ray_time);
    }

    vec3 pixel_sample_square(int s_i, int s_j) const {
        // Returns a random point in the square surrounding a pixel at the origin, given
        // the two subpixel indices.
        auto px = -0.5 + recip_sqrt_spp * (s_i + random_double());
        auto py = -0.5 + recip_sqrt_spp * (s_j + random_double());
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }

    point3 defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    //color ray_color(const ray& r, int depth, const hittable& world, const hittable& lights) const {
    //    hit_record rec;

    //    // If we've exceeded the ray bounce limit, no more light is gathered.
    //    if (depth <= 0)
    //        return color(0, 0, 0);

    //    // If the ray hits nothing, return the background color.
    //    if (!world.hit(r, interval(0.001, infinity), rec))
    //        return background;

    //    ray scattered;
    //    color attenuation;
    //    double pdf_val;

    //    color color_from_emission = rec.mat->emitted(r, rec, rec.u, rec.v, rec.p);

    //    if (!rec.mat->scatter(r, rec, attenuation, scattered, pdf_val))
    //        return color_from_emission;

    //    /*hittable_pdf light_pdf(lights, rec.p);
    //    scattered = ray(rec.p, light_pdf.generate(), r.time());
    //    pdf_val = light_pdf.value(scattered.direction());*/

    //    auto p0 = make_shared<hittable_pdf>(lights, rec.p);
    //    auto p1 = make_shared<cosine_pdf>(rec.normal);
    //    mixture_pdf mixed_pdf(p0, p1);

    //    scattered = ray(rec.p, mixed_pdf.generate(), r.time());
    //    pdf_val = mixed_pdf.value(scattered.direction());

    //    double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);

    //    color sample_color = ray_color(scattered, depth - 1, world, lights);
    //    color color_from_scatter = (attenuation * scattering_pdf * sample_color) / pdf_val;

    //    return color_from_emission + color_from_scatter;
    //}

    color ray_color(const ray& r, int depth, const hittable& world, const hittable& lights)
        const {
        hit_record rec;

        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return color(0, 0, 0);

        // If the ray hits nothing, return the background color.
        if (!world.hit(r, interval(0.001, infinity), rec))
            return background;

        scatter_record srec;
        color color_from_emission = rec.mat->emitted(r, rec, rec.u, rec.v, rec.p);

        if (!rec.mat->scatter(r, rec, srec))
            return color_from_emission;

        if (srec.skip_pdf) {
            return srec.attenuation * ray_color(srec.skip_pdf_ray, depth - 1, world, lights);
        }

        auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);
        mixture_pdf p(light_ptr, srec.pdf_ptr);

        ray scattered = ray(rec.p, p.generate(), r.time());
        auto pdf_val = p.value(scattered.direction());

        double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);

        color sample_color = ray_color(scattered, depth - 1, world, lights);
        color color_from_scatter = (srec.attenuation * scattering_pdf * sample_color) / pdf_val;

        return color_from_emission + color_from_scatter;
    }

    color ray_color(const ray& r, int depth, const hittable& world) const {
        hit_record rec;

        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return color(0, 0, 0);

        // If the ray hits nothing, return the background color.
        if (!world.hit(r, interval(0.001, infinity), rec))
            return background;

        ray scattered;
        color attenuation;
        double pdf;
        color color_from_emission = rec.mat->emitted(r, rec, rec.u, rec.v, rec.p);

        if (!rec.mat->scatter(r, rec, attenuation, scattered, pdf))
            return color_from_emission;

        auto on_light = point3(random_double(213, 343), 554, random_double(227, 332));
        auto to_light = on_light - rec.p;
        auto distance_squared = to_light.length_squared();
        to_light = unit_vector(to_light);

        if (dot(to_light, rec.normal) < 0)
            return color_from_emission;

        double light_area = (343 - 213) * (332 - 227);
        auto light_cosine = fabs(to_light.y());
        if (light_cosine < 0.000001)
            return color_from_emission;

        pdf = distance_squared / (light_cosine * light_area);
        scattered = ray(rec.p, to_light, r.time());

        double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);


        color color_from_scatter = (attenuation * scattering_pdf * ray_color(scattered, depth - 1, world)) / pdf;

        return color_from_emission + color_from_scatter;
    }
};

#endif