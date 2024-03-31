#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/logger.hpp>

using namespace std;

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
	Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

	Eigen::Matrix4f translate;
	translate << 1, 0, 0, -eye_pos[0],
		0, 1, 0, -eye_pos[1],
		0, 0, 1, -eye_pos[2],
		0, 0, 0, 1;

	view = translate * view;

	return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
	Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

	// TODO: Implement this function
	// Create the model matrix for rotating the triangle around the Z axis.
	// Then return it.

	float a = rotation_angle / 180 * MY_PI;
	model << cos(a), -sin(a), 0, 0,
		sin(a), cos(a), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	return model;
}


Eigen::Matrix4f get_projection_matrix(float eye_fov, // 垂直角度
	float aspect_ratio, // 纵横比
	float zNear, // 近平面
	float zFar)  // 远平面
{
	// Students will implement this function
	Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

	// TODO: Implement this function
	// Create the projection matrix for the given parameters.
	// Then return it.

	float n = zNear, f = zFar;

	float fovY = eye_fov / 180 * MY_PI; // 角度转弧度
	float t = tan(fovY / 2) * (-n), b = -t; // 朝向-z方向|n|
	float r = aspect_ratio * t, l = -r;

	// 第一种实现
	// 透视投影->正交投影  挤压
	/*Eigen::Matrix4f Mpersp2orhtho;
	Mpersp2orhtho << n, 0, 0, 0,
		0, n, 0, 0,
		0, 0, n + f, -n * f,
		0, 0, 1, 0;*/

	/*Eigen::Matrix4f Mortho, Mtrans, Mscale;
	Mtrans << 1, 0, 0, -(r + l) / 2,
		0, 1, 0, -(t + b) / 2,
		0, 0, 1, -(n + f) / 2,
		0, 0, 0, 1;
	Mscale << 2 / (r - l), 0, 0, 0,
		0, 2 / (t - b), 0, 0,
		0, 0, 2 / (n - f), 0,
		0, 0, 0, 1;
	Mortho = Mscale * Mtrans;

	projection = Mortho * Mpersp2orhtho;*/

	//return projection;

	// 第二种实现
	projection << n / r, 0, 0, 0,
		0, n / t, 0, 0,
		0, 0, -(f + n) / (f - n), - (2 * f * n) / (f - n),
		0, 0, 1, 0;

	return projection;

}


Eigen::Matrix4f get_rotation(Vector3f axis, float angle) { // 罗德里格斯任意轴旋转
	// Use Rodrigues rotation formula
	float radian = angle / 180 * MY_PI;
	Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
	Eigen::Matrix3f I = Eigen::Matrix3f::Identity();
	Eigen::Matrix3f M;
	Eigen::Matrix3f Rk;
	Rk << 0, -axis[2], axis[1],
		axis[2], 0, -axis[0],
		-axis[1], axis[0], 0;

	M = I + (1 - cos(radian)) * Rk * Rk + sin(radian) * Rk;

	model << M(0, 0), M(0, 1), M(0, 2), 0,
		M(1, 0), M(1, 1), M(1, 2), 0,
		M(2, 0), M(2, 1), M(2, 2), 0,
		0, 0, 0, 1;
	return model;
}

int main(int argc, const char** argv)
{

	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT); // see: https://blog.csdn.net/benobug/article/details/117653268

	float angle = 0;
	bool command_line = false;
	std::string filename = "output.png";

	if (argc >= 3) {
		command_line = true;
		angle = std::stof(argv[2]); // -r by default
		if (argc == 4) {
			filename = std::string(argv[3]);
		}
		/*else
			return 0;*/
	}

	rst::rasterizer r(700, 700);  // 初始化光栅化器的实例r，且定义显示大小为700*700

	Eigen::Vector3f eye_pos = { 0, 0, 5 };  //摄像机的位置

	std::vector<Eigen::Vector3f> pos{ {2, 0, -2}, {0, 2, -2}, {-2, 0, -2} };  //三角形的位置，硬编码

	std::vector<Eigen::Vector3i> ind{ {0, 1, 2} };

	auto pos_id = r.load_positions(pos);  // 储存位置信息
	auto ind_id = r.load_indices(ind);  // 储存索引信息

	int key = 0;
	int frame_count = 0;

	if (command_line) {
		r.clear(rst::Buffers::Color | rst::Buffers::Depth);

		r.set_model(get_model_matrix(angle));
		r.set_view(get_view_matrix(eye_pos));
		r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

		r.draw(pos_id, ind_id, rst::Primitive::Triangle);
		cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
		image.convertTo(image, CV_8UC3, 1.0f);

		cv::imwrite(filename, image);

		return 0;
	}

	while (key != 27) {
		r.clear(rst::Buffers::Color | rst::Buffers::Depth);

		// 绕x轴转
		//r.set_model(get_model_matrix(angle));  
		
		// 罗德里格斯任意轴旋转
		Vector3f axis = { 0,0,1 };
		r.set_model(get_rotation(axis, angle));

		r.set_view(get_view_matrix(eye_pos));
		r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

		r.draw(pos_id, ind_id, rst::Primitive::Triangle);

		cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data()); // 定义的用于表示任意维度的稠密数组，OpenCV使用它来存储和传递图像
		image.convertTo(image, CV_8UC3, 1.0f);
		cv::imshow("image", image);
		key = cv::waitKey(10);

		std::cout << "frame count: " << frame_count++ << '\n';

		if (key == 'a') {
			angle += 10;
		}
		else if (key == 'd') {
			angle -= 10;
		}
	}

	return 0;
}
