#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include "main.h"


void compare(std::vector<std::vector<double>>& data,
	std::vector<std::vector<Point>>& grid, int& height, int& width,
	int row, int col, int offsetx, int offsety, bool& changed) {

	double epsilon = 1e-3;
	if (grid[row][col].dist <= 0)return;
	int new_row = row + offsetx, new_col = col + offsety;
	if (isValid(new_row, new_col, height, width)) {
		int cdistx = grid[new_row][new_col].distx;
		int cdisty = grid[new_row][new_col].disty;
		int newdistx = cdistx + offsetx;
		int newdisty = cdisty + offsety;
		int closestx = new_row - cdistx;
		int closesty = new_col - cdisty;

		double newdist = distaa3(data, grid, closestx, closesty, newdistx, newdisty);
		double olddist = grid[row][col].dist;

		if (newdist < olddist - epsilon) {
			grid[row][col].dist = newdist;
			grid[row][col].distx = newdistx;
			grid[row][col].disty = newdisty;
			changed = true;
		}
	}

}

// 1. http://www.codersnotes.com/notes/signed-distance-fields/
// 2. https://github.com/Lisapple/8SSEDT
// 3. https://github.com/OpenGLInsights/OpenGLInsightsCode/tree/master/Chapter%2012%202D%20Shape%20Rendering%20by%20Distance%20Fields
void generateSDF(
	std::vector<std::vector<double>>& data,
	std::vector<std::vector<Point>>& grid,
	int& height, int& width) {
	bool changed = false;

	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			grid[row][col].distx = 0.0;
			grid[row][col].disty = 0.0;
			if (data[row][col] <= 0.0) {
				grid[row][col].dist = 100000.0;
			}
			else if (data[row][col] < 1.0) {
				grid[row][col].dist = edgedf(grid[row][col].gx, grid[row][col].gy, data[row][col]);
			}
			else {
				grid[row][col].dist = 0.0;
			}
		}
	}


	int count = 0;
	// Pass1
	do {
		changed = false;
		for (int row = 0; row < height; row++) {
			for (int col = 0; col < width; col++) {
				compare(data, grid, height, width, row, col, -1, -1, changed);
				compare(data, grid, height, width, row, col, -1, 0, changed);
				compare(data, grid, height, width, row, col, -1, 1, changed);
				compare(data, grid, height, width, row, col, 0, -1, changed);
			}
			for (int col = height - 1; col >= 0; col--) {
				compare(data, grid, height, width, row, col, 0, 1, changed);
			}
		}
		// Pass2
		for (int row = height - 1; row >= 0; row--) {
			for (int col = width - 1; col >= 0; col--) {
				compare(data, grid, height, width, row, col, 0, 1, changed);
				compare(data, grid, height, width, row, col, 1, -1, changed);
				compare(data, grid, height, width, row, col, 1, 0, changed);
				compare(data, grid, height, width, row, col, 1, 1, changed);
			}

			for (int col = 0; col < width; col++) {
				compare(data, grid, height, width, row, col, 0, -1, changed);
			}
		}
	} while (changed);

}


void make_distance_map(unsigned char* img, int& width, int& height, int& nrChannels, int dataChannel, unsigned char* out, int outChannel)
{
	std::vector<std::vector<double>> data(height, std::vector<double>(width));
	std::vector<std::vector<Point>> outside(height, std::vector<Point>(width));
	std::vector<std::vector<Point>> inside(height, std::vector<Point>(width));

	// Convert img into double (data)
	double img_min = 255, img_max = -255;
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			double v = getColor(img, width, nrChannels, row, col, dataChannel);
			if (v > img_max) img_max = v;
			if (v < img_min) img_min = v;

		}
	}

	// Rescale image levels between 0 and 1
	double img_offset = (img_min < 128.0) ? img_min : 0;
	double img_range = img_max - img_min;
	if (img_range == 0.0) img_range = 255.0; // Failsafe for constant image

	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			int val = getColor(img, width, nrChannels, row, col, dataChannel);
			data[row][col] = (val - img_offset) / img_range;
		}
	}

	computegradient(data, outside, height, width);
	generateSDF(data, outside, height, width);
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			if (outside[row][col].dist < 0.0) {
				outside[row][col].dist = 0.0;
			}
		}
	}


	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			data[row][col] = 1.0 - data[row][col];
		}
	}

	computegradient(data, inside, height, width);
	generateSDF(data, inside, height, width);
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			if (inside[row][col].dist < 0.0) {
				inside[row][col].dist = 0.0;
			}
		}
	}


	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			double dist = outside[row][col].dist - inside[row][col].dist;
			dist = 128.0 + dist;
			if (dist < 0.0) dist = 0.0;
			if (dist >= 256.0) dist = 255.999;

			setColor(out, width, outChannel, row, col, 0, static_cast<unsigned char>(dist));
			setColor(out, width, outChannel, row, col, 1, static_cast<unsigned char>((dist - floor(dist)) * 256.0));
			setColor(out, width, outChannel, row, col, 2, static_cast<unsigned char>(getColor(img, width, nrChannels, row, col, dataChannel)));
			setColor(out, width, outChannel, row, col, 3, static_cast<unsigned char>(255.0));

		}

	}

}

int main() {
	int width = 0, height = 0, nrChannels = 0;

	const char* path = "../resources/imgs/flipper128.png";
	unsigned char* img = stbi_load(path, &width, &height, &nrChannels, 0);

	if (!img)
	{
		fprintf(stderr, "Failed to read image '%s' !\n", path);
		return EXIT_FAILURE;
	}
	printf("Input image: %d x %d pixels, %d channels\n", width, height, nrChannels);


	int outChannel = 4;
	unsigned char* out = new unsigned char[width * height * outChannel * sizeof(unsigned char)];

	make_distance_map(img, width, height, nrChannels, 0, out, outChannel);

	stbi_write_png("./output.png", width, height, outChannel, out, width * outChannel);

}

