#include <iostream>
#include <vector>

typedef struct Point {
	int val;
	int beta_y = 1;
	int beta_x = 1;
	float dist2() {
		return std::sqrt(val);
	}
} Point;


std::ostream& operator<<(std::ostream& os, const Point& p) {
	os << p.val << " ";
	return os;
}


const int WIDTH = 5;
const int HEIGHT = 6;
const int MAX = 100;

void show(const std::vector<std::vector<Point>> grid) {
	for (int row = 0; row < HEIGHT; row++) {
		for (int col = 0; col < WIDTH; col++) {
			std::cout << grid[row][col] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void showSDF(std::vector<std::vector<Point>> grid1, std::vector<std::vector<Point>> grid2) {
	for (int row = 0; row < HEIGHT; row++) {
		for (int col = 0; col < WIDTH; col++) {
			std::cout << grid1[row][col].dist2() - grid2[row][col].dist2() << " ";
		}
		std::cout << std::endl;
	}
}

// 1. https://prideout.net/blog/distance_fields/
bool generateSDF(std::vector<std::vector<Point>>& grid, std::vector<std::vector<Point>>& tmp, float beta) {
	bool changed = false;

	// horizonal pass
	for (int row = 0; row < HEIGHT; row++) {
		for (int col = 0; col < WIDTH; col++) {
			tmp[row][col].val = grid[row][col].val;
			if (col + 1 < WIDTH && grid[row][col].val > grid[row][col + 1].val + beta) {
				tmp[row][col].val = grid[row][col + 1].val + beta;
				changed = true;
			}
			if (col - 1 >= 0 && grid[row][col].val > grid[row][col - 1].val + beta) {
				tmp[row][col].val = grid[row][col - 1].val + beta;
				changed = true;
			}
		}
	}

	// copy buffer
	for (int row = 0; row < HEIGHT; row++) {
		for (int col = 0; col < WIDTH; col++) {
			grid[row][col] = tmp[row][col];
		}
	}
	show(grid);

	// vertical pass
	for (int col = 0; col < WIDTH; col++) {
		for (int row = 0; row < HEIGHT; row++) {
			tmp[row][col].val = grid[row][col].val;
			if (row + 1 < HEIGHT && grid[row][col].val > grid[row + 1][col].val + beta) {
				tmp[row][col].val = grid[row + 1][col].val + beta;
				changed = true;
			}
			if (row - 1 >= 0 && grid[row][col].val > grid[row - 1][col].val + beta) {
				tmp[row][col].val = grid[row - 1][col].val + beta;
				changed = true;
			}
		}
	}

	// copy pass
	for (int row = 0; row < HEIGHT; row++) {
		for (int col = 0; col < WIDTH; col++) {
			grid[row][col] = tmp[row][col];
		}
	}
	return changed;
}

int main() {
	std::vector<std::vector<float>> data = {
			{0, 0, 0, 0, 0} ,
			{0, 0, 0, 0, 0},
			{0, 0, 255, 0, 0},
			{0, 0, 255, 0, 0},
			{0, 0, 0, 0, 0},
			{0, 0, 0, 255, 0}
	};

	std::vector<std::vector<Point>> grid1(HEIGHT, std::vector<Point>(WIDTH));
	std::vector<std::vector<Point>> grid2(HEIGHT, std::vector<Point>(WIDTH));

	for (int row = 0; row < HEIGHT; row++) {
		for (int col = 0; col < WIDTH; col++) {
			if (data[row][col] > 128) {
				grid1[row][col] = { 0 };
				grid2[row][col] = { MAX };
			}
			else {
				grid1[row][col] = { MAX };
				grid2[row][col] = { 0 };
			}
		}
	}

	std::vector<std::vector<Point>> tmp(HEIGHT, std::vector<Point>(WIDTH));

	int beta = 1;
	while (generateSDF(grid1, tmp, beta)) {
		beta += 2;
	}
	std::cout << std::endl;
	beta = 1;
	while (generateSDF(grid2, tmp, beta))
	{
		beta += 2;
	};
	showSDF(grid1, grid2);
	return 0;
}

