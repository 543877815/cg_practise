#include <iostream>
#include <vector>

const int WIDTH = 5;
const int HEIGHT = 5;
const int MAX = 100;

typedef struct Point {
	float dx, dy;

	float dist2() {
		return std::sqrt(dx * dx + dy * dy);
	}
} Point;

std::ostream& operator<<(std::ostream& os, const Point& p) {
	os << "(" << p.dx << ", " << p.dy << ")";
	return os;
}

void show(const std::vector<std::vector<Point>> grid) {
	for (int row = 0; row < HEIGHT; row++) {
		for (int col = 0; col < WIDTH; col++) {
			std::cout << grid[row][col] << " ";
		}
		std::cout << std::endl;
	}
}

void showSDF(std::vector<std::vector<Point>> grid1, std::vector<std::vector<Point>> grid2) {
	for (int row = 0; row < HEIGHT; row++) {
		for (int col = 0; col < WIDTH; col++) {
			std::cout << grid1[row][col].dist2() - grid2[row][col].dist2() << " ";
		}
		std::cout << std::endl;
	}
}

bool isValid(int row, int col) {
	return row >= 0 && col >= 0 && row < HEIGHT && col < WIDTH;
}

void compare(std::vector<std::vector<Point>>& grid, Point& p, const int& row, const int& col, int offsetx, int offsety) {
	int row_n = row + offsetx;
	int col_n = col + offsety;
	if (isValid(row_n, col_n)) {
		Point other{ grid[row_n][col_n].dx + offsetx, grid[row_n][col_n].dy + offsety };
		if (other.dist2() < p.dist2()) {
			p = other;
		}
	}
}

// 1. http://www.codersnotes.com/notes/signed-distance-fields/
// 2. https://github.com/Lisapple/8SSEDT
void generateSDF(std::vector<std::vector<Point>>& grid) {
	// Pass1
	for (int row = 0; row < HEIGHT; row++) {
		for (int col = 0; col < WIDTH; col++) {
			Point p = grid[row][col];
			compare(grid, p, row, col, -1, -1);
			compare(grid, p, row, col, -1, 0);
			compare(grid, p, row, col, -1, 1);
			compare(grid, p, row, col, 0, -1);
			grid[row][col] = p;
		}

		for (int col = WIDTH - 1; col >= 0; col--) {
			Point p = grid[row][col];
			compare(grid, p, row, col, 0, 1);
			grid[row][col] = p;

		}
	}

	// Pass2
	for (int row = HEIGHT - 1; row >= 0; row--) {
		for (int col = WIDTH - 1; col >= 0; col--) {
			Point p = grid[row][col];
			compare(grid, p, row, col, 0, 1);
			compare(grid, p, row, col, 1, -1);
			compare(grid, p, row, col, 1, 0);
			compare(grid, p, row, col, 1, 1);
			grid[row][col] = p;
		}

		for (int col = 0; col < WIDTH; col++) {
			Point p = grid[row][col];
			compare(grid, p, row, col, 0, -1);
			grid[row][col] = p;
		}
	}
}


int main() {
	std::vector<std::vector<float>> data = {
		{0,0,0,0,0},
		{0,0,0,0,0},
		{0,0,0,255,255},
		{255,255,255,255,255},
		{255,255,255,255,255}
	};

	std::vector<std::vector<Point>> grid1(HEIGHT, std::vector<Point>(WIDTH));
	std::vector<std::vector<Point>> grid2(HEIGHT, std::vector<Point>(WIDTH));

	for (int row = 0; row < HEIGHT; row++) {
		for (int col = 0; col < WIDTH; col++) {
			if (data[row][col] > 128) {
				grid1[row][col] = { 0,0 };
				grid2[row][col] = { MAX, MAX };
			}
			else {
				grid1[row][col] = { MAX, MAX };
				grid2[row][col] = { 0, 0 };
			}
		}
	}

	generateSDF(grid1);
	generateSDF(grid2);

	show(grid1);
	std::cout << std::endl;
	show(grid2);
	std::cout << std::endl;
	showSDF(grid1, grid2);
	return 0;
}

