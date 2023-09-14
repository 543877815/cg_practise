//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include "global.hpp"
#include <Eigen/Eigen>
#include <opencv2/opencv.hpp>

class Texture{
private:
    cv::Mat image_data;

public:
    Texture(const std::string& name)
    {
        image_data = cv::imread(name);
        width = image_data.cols;
        height = image_data.rows;
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
    }

    int width, height;

    Eigen::Vector3f getColor(float u, float v)
    {
        if (u < 0) u = 0;
        if (u > 1) u = 1;
        if (v < 0) v = 0;
        if (v > 1) v = 1;
        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        auto color = image_data.at<cv::Vec3b>(v_img, u_img);
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

    Eigen::Vector3f getColorBilinear(float u, float v) {
        if (u < 0) u = 0.0f;
        if (u > 1) u = 0.999f;
        if (v < 0) v = 0.0f;
        if (v > 1) v = 0.999f;
        float u_img = u * width;
        float v_img = v * height;

        float u_0 = std::floor(u_img);
        float u_1 = std::ceil(u_img);
        float v_1 = std::ceil(v_img);
        float v_0 = std::floor(v_img);
        float s = u_img - u_0;
        float t = v_img - v_1;

        Eigen::Vector3f color_00, color_10, color_01, color_11, color_u0, color_u1, color;
        color_00 = getColor(u_0 / width, v_0 / height);
        color_10 = getColor(u_1 / width, v_0 / height);
        color_01 = getColor(u_0 / width, v_1 / height);
        color_11 = getColor(u_1 / width, v_1 / height);
        color_u0 = color_00 + s * (color_10 - color_00);
        color_u1 = color_01 + s * (color_11 - color_01);
        color = color_u0 + t * (color_u1 - color_u0);

        return color;
    }
};
#endif //RASTERIZER_TEXTURE_H
