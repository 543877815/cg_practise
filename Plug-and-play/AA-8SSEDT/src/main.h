#pragma once


typedef struct Point {
	double dist, distx, disty, gx, gy;
	Point() {
		dist = distx = disty = gx = gy = 0.0;
	}
} Point;


bool isValid(int row, int col, int h, int w) {
	return row >= 0 && col >= 0 && row < h && col < w;
}

void setColor(unsigned char* data, int width, int nrChannels, int row, int col, int index, unsigned char val) {
	data[width * nrChannels * row + nrChannels * col + index] = val;
}

int getColor(unsigned char* data, int width, int nrChannels, int row, int col, int index) {
	return data[width * nrChannels * row + nrChannels * col + index];
}

#define SQRT2 1.4142136
void computegradient(std::vector<std::vector<double>>& data, std::vector<std::vector<Point>>& grid, int height, int width)
{
	const int kernelWidth = 3;
	int halfKernelWidth = kernelWidth / 2;
	double kernel_y[kernelWidth][kernelWidth] = {
		{1, SQRT2, 1},
		{ 0, 0, 0 },
		{-1, -SQRT2, -1},
	};
	double kernel_x[kernelWidth][kernelWidth] = {
		{1, 0, -1},
		{SQRT2, 0, -SQRT2},
		{1, 0, -1}
	};

	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			double gx = 0.0, gy = 0.0;
			if (data[row][col] <= 0.0 || data[row][col] >= 1.0) continue;
			for (int i = -halfKernelWidth; i <= halfKernelWidth; i++) {
				for (int j = -halfKernelWidth; j <= halfKernelWidth; j++) {
					int new_row = row + i;
					int new_col = col + j;
					if (isValid(new_row, new_col, height, width)) {
						gx += data[new_row][new_col] * kernel_x[i + halfKernelWidth][j + halfKernelWidth];
						gy += data[new_row][new_col] * kernel_y[i + halfKernelWidth][j + halfKernelWidth];
					}
				}
			}
			double glength = gx * gx + gy * gy;
			if (glength > 0.0) {
				glength = sqrt(glength);
				grid[row][col].gx = gx / glength;
				grid[row][col].gy = gy / glength;
			}

		}
	}
}


double edgedf(double gx, double gy, double a)
{
	double df, glength, temp, a1;

	if ((gx == 0) || (gy == 0)) { // Either A) gu or gv are zero, or B) both
		df = 0.5 - a;  // Linear approximation is A) correct or B) a fair guess
	}
	else {
		glength = sqrt(gx * gx + gy * gy);
		if (glength > 0) {
			gx = gx / glength;
			gy = gy / glength;
		}
		/* Everything is symmetric wrt sign and transposition,
		 * so move to first octant (gx>=0, gy>=0, gx>=gy) to
		 * avoid handling all possible edge directions.
		 */
		gx = fabs(gx);
		gy = fabs(gy);
		if (gx < gy) {
			temp = gx;
			gx = gy;
			gy = temp;
		}
		a1 = 0.5 * gy / gx;
		if (a < a1) { // 0 <= a < a1
			df = 0.5 * (gx + gy) - sqrt(2.0 * gx * gy * a);
		}
		else if (a < (1.0 - a1)) { // a1 <= a <= 1-a1
			df = (0.5 - a) * gx;
		}
		else { // 1-a1 < a <= 1
			df = -0.5 * (gx + gy) + sqrt(2.0 * gx * gy * (1.0 - a));
		}
	}
	return df;
}


double distaa3(std::vector<std::vector<double>>& data, std::vector<std::vector<Point>>& grid, int closestx, int closesty, int newdistx, int newdisty) {
	double di, df, dx, dy, gx, gy, a;


	a = data[closestx][closesty];    // Grayscale value at the edge pixel
	gx = grid[closestx][closesty].gx; // X gradient component at the edge pixel
	gy = grid[closestx][closesty].gy; // Y gradient component at the edge pixel

	if (a > 1.0) a = 1.0;
	if (a < 0.0) a = 0.0; // Clip grayscale values outside the range [0,1]
	if (a == 0.0) return 1000000.0; // Not an object pixel, return "very far" ("don't know yet")

	dx = (double)newdistx;
	dy = (double)newdisty;
	di = sqrt(dx * dx + dy * dy); // Length of integer vector, like a traditional EDT
	if (di == 0) { // Use local gradient only at edges
		// Estimate based on local gradient only
		df = edgedf(gx, gy, a);
	}
	else {
		// Estimate gradient based on direction to edge (accurate for large di)
		df = edgedf(dx, dy, a);
	}
	return di + df; // Same metric as edtaa2, except at edges (where di=0)
}