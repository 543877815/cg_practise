#include<iostream>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include <queue>
#include <utility>
#include "main.h"

void setColor(unsigned char* data, const int& c, const int& w, int row, int col, int idx, unsigned char val) {
	data[c * w * row + col * c + idx] = val;
}

unsigned char getColor(unsigned char* data, int c, int w, int row, int col, int idx) {
	return data[c * w * row + col * c + idx];
}

int main() {
	int w, h, c;
	unsigned char* img = stbi_load("../resources/imgs/flipper128.png", &w, &h, &c, 0);
	int rawChannel = 0;  // 3是alpha通道
	int outputChannel = 4;

	unsigned char* output = new unsigned char[w * h * outputChannel];

	double max_distance = 0;

	double MAX = std::max(h, w) * 2;

	std::vector<std::vector<Point>> inside(h, std::vector<Point>(w, { 0, 0, 0 }));
	std::vector<std::vector<Point>> outside(h, std::vector<Point>(w, { 0, 0, 0 }));
	std::vector<std::vector<double>> dist(h, std::vector<double>(w));

	// convert img into dobule data
	std::vector<std::vector<double>> data(h, std::vector<double>(w));
	double img_min = 255, img_max = -255;
	for (int row = 0; row < h; row++) {
		for (int col = 0; col < w; col++) {
			double v = static_cast<double>(getColor(img, c, w, row, col, rawChannel));
			data[row][col] = v;
			if (v > img_max) img_max = v;
			if (v < img_min) img_min = v;
		}
	}

	// Rescale image levels between 0 and 1
	double img_offset = (img_min < 128.0) ? img_min : 0;
	double img_range = img_max - img_min;
	if (img_range == 0.0) img_range = 255.0; // safe for constant image;
	for (int row = 0; row < h; row++) {
		for (int col = 0; col < w; col++) {
			double v = static_cast<double>(getColor(img, c, w, row, col, rawChannel));
			data[row][col] = (v - img_offset) / img_range;
		}
	}

	// Transform background (outside contour, in areas of 0's)
	std::vector<std::vector<double>> gx(h, std::vector<double>(w, 0));
	std::vector<std::vector<double>> gy(h, std::vector<double>(w, 0));
	computeGradient(data, w, h, gx, gy);
	edtaa3(data, gx, gy, outside, w, h);
	for (int row = 0; row < h; row++) {
		for (int col = 0; col < w; col++) {
			if (outside[row][col].dist < 0.0)
				outside[row][col].dist = 0.0;
		}
	}

	for (int row = 0; row < h; row++) {
		for (int col = 0; col < w; col++) {
			data[row][col] = 1.0 - data[row][col];
		}
	}
	computeGradient(data, w, h, gx, gy);
	edtaa3(data, gx, gy, inside, w, h);
	for (int row = 0; row < h; row++) {
		for (int col = 0; col < w; col++) {
			if (inside[row][col].dist < 0.0)
				inside[row][col].dist = 0.0;
		}
	}

	double min_dist = INT_MAX;
	double max_dist = INT_MIN;
	for (int row = 0; row < h; row++) {
		for (int col = 0; col < w; col++) {
			double dist1 = inside[row][col].dist2();
			double dist2 = outside[row][col].dist2();
			double dist = dist2 - dist1;
			min_dist = std::min(min_dist, dist);
			max_dist = std::max(max_dist, dist);
		}
	}

	std::vector<std::vector<double>> SDF(h, std::vector<double>(w));
	std::cout << "min_dist: " << min_dist << " ,max_dist: " << max_dist;
	for (int row = 0; row < h; row++) {
		for (int col = 0; col < w; col++) {
			double dist1 = inside[row][col].dist;
			double dist2 = outside[row][col].dist;
			double dist = dist1 - dist2;
			SDF[row][col] = dist;
			dist = dist + 128.0;
			if (dist < 0.0) dist = 0.0;
			if (dist >= 256.0) dist = 255.999;
			setColor(output, outputChannel, w, row, col, 0, static_cast<unsigned char>(dist));
			setColor(output, outputChannel, w, row, col, 1, static_cast<unsigned char>((dist - floor(dist)) * 256.0));
			setColor(output, outputChannel, w, row, col, 2, static_cast<unsigned char>(getColor(img, c, w, row, col, rawChannel)));
			setColor(output, outputChannel, w, row, col, 3, static_cast<unsigned char>(255.0));
		}
	}
	// 保存图片
	int success = stbi_write_png("SDF.png", w, h, outputChannel, output, w * 4);
	if (!success) std::cout << "write fail !" << std::endl;
	return 0;
}