#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

class camera {
public:
    camera(
        point3 lookfrom,  // 成像平面
        point3 lookat,   // 聚焦的物体平面
        vec3   vup,   // 垂直于世界平面的单位向量
        double vfov, // 以度为单位的垂直视野
        double aspect_ratio,  // 宽高比
        double aperture,  // 小孔直径
        double focus_dist,  // 焦点到成像平面的距离
        double _time0 = 0,  // 快门打开时间
        double _time1 = 0   // 快门关闭时间
    ) {
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2.0 * h;
        auto viewport_width = aspect_ratio * viewport_height;

        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));  // 叉乘
        v = cross(w, u);

        origin = lookfrom;  // 屏幕中心
        horizontal = focus_dist * viewport_width * u;  
        vertical = focus_dist * viewport_height * v;
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;  // 屏幕左下角元带你

        lens_radius = aperture / 2;  // 小孔半径'
        time0 = _time0;
        time1 = _time1;
    }


    ray get_ray(double s, double t) const {
        vec3 rd = lens_radius * random_in_unit_disk();  
        vec3 offset = u * rd.x() + v * rd.y();
        // Instead, I usually start rays from the lens, and send them toward the focus plane (focus_dist away from the lens), 
        // where everything on that plane is in perfect focus.
        return ray(
            origin + offset,  // 在小孔的圆上随机采样而不是点
            lower_left_corner + s * horizontal + t * vertical - origin - offset,   // 发射方向
            random_double(time0, time1)  // 定义一个随机时间
        );
    }

private:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
    double lens_radius;
    double time0, time1; // 快门打开关闭时间


};
#endif