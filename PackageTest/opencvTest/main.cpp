#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
	// 读取图片
	cv::Mat image;
	image = cv::imread("test.jpg", cv::IMREAD_COLOR);

	// 检查图片是否正确加载
	if (image.empty()) {
		std::cerr << "错误：无法加载图片" << std::endl;
		return -1;
	}

	// 转换为灰度图
	cv::Mat gray_image;
	cv::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);

	// 显示图片
	cv::namedWindow("Original Image", cv::WINDOW_AUTOSIZE);
	cv::imshow("Original Image", image);

	cv::namedWindow("Gray Image", cv::WINDOW_AUTOSIZE);
	cv::imshow("Gray Image", gray_image);

	// 等待用户按键，再关闭窗口
	cv::waitKey(0);

	return 0;
}

