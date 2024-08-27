#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
	// ��ȡͼƬ
	cv::Mat image;
	image = cv::imread("test.jpg", cv::IMREAD_COLOR);

	// ���ͼƬ�Ƿ���ȷ����
	if (image.empty()) {
		std::cerr << "�����޷�����ͼƬ" << std::endl;
		return -1;
	}

	// ת��Ϊ�Ҷ�ͼ
	cv::Mat gray_image;
	cv::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);

	// ��ʾͼƬ
	cv::namedWindow("Original Image", cv::WINDOW_AUTOSIZE);
	cv::imshow("Original Image", image);

	cv::namedWindow("Gray Image", cv::WINDOW_AUTOSIZE);
	cv::imshow("Gray Image", gray_image);

	// �ȴ��û��������ٹرմ���
	cv::waitKey(0);

	return 0;
}