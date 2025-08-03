#pragma once
#include<vector>
#define SQRT2 1.4142136

int main();

bool isValid(int x, int y, int n, int m) {
	return x >= 0 && y >= 0 && x < n && y < m;
}

void computeGradient(std::vector<std::vector<double>>& data, int w, int h, std::vector<std::vector<double>>& gx, std::vector<std::vector<double>>& gy) {
	static std::vector<std::vector<double>> kernel_y = {
		{1, SQRT2, 1},
		{0, 0, 0},
		{-1, -SQRT2, -1}
	};
	static std::vector<std::vector<double>> kernel_x = {
		{1, 0, -1},
		{SQRT2, 0, -SQRT2},
		{1, 0, -1}
	};

	int kernelWidth = 3;
	int halfKernelWidth = kernelWidth / 2;
	for (int row = 0; row < h; row++) {
		for (int col = 0; col < w; col++) {
			double val_x = 0;
			double val_y = 0;
			if ((data[row][col] > 0.0) && (data[row][col] < 1.0)) {
				for (int i = -halfKernelWidth; i <= halfKernelWidth; i++) {
					for (int j = -halfKernelWidth; j <= halfKernelWidth; j++) {
						int new_row = i + row;
						int new_col = j + col;
						if (isValid(new_row, new_col, h, w)) {
							val_x += data[new_row][new_col] * kernel_x[halfKernelWidth + i][halfKernelWidth + j];
							val_y += data[new_row][new_col] * kernel_y[halfKernelWidth + i][halfKernelWidth + j];
						}
					}
				}
			}
			gx[row][col] = val_x;
			gy[row][col] = val_y;
			double glength = val_x * val_x + val_y * val_y;
			if (glength > 0.0) {
				glength = sqrt(glength);
				gx[row][col] = gx[row][col] / glength;
				gy[row][col] = gy[row][col] / glength;
			}
		}
	}
}


typedef struct Point {
	short distx, disty;
	double dist;
	struct Point(int _dx, int _dy, int _dist) : distx(_dx), disty(_dy), dist(_dist) {}

	double dist2() {
		return std::sqrt(static_cast<double>(distx * distx + disty * disty));
	}
} Point;


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
		a1 = 0.5 * gy / gx;  // ����ȷ����df����ʽ(4)
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


double distaa3(std::vector<std::vector<double>>& data,
	std::vector<std::vector<double>>& gximg,
	std::vector<std::vector<double>>& gyimg,
	std::vector<std::vector<Point>>& grid,
	int closestx, int closesty, int xi, int yi)
{

	double a = data[closestx][closesty];   // Grayscale value at the edge pixel
	double gx = gximg[closestx][closesty]; // X gradient component at the edge pixel
	double gy = gyimg[closestx][closesty]; // Y gradient component at the edge pixel

	if (a > 1.0) a = 1.0;
	if (a < 0.0) a = 0.0; // Clip grayscale values outside the range [0,1]
	if (a == 0.0) return 1000000.0; // Not an object pixel, return "very far" ("don't know yet")
	double df;

	double dx = (double)xi;
	double dy = (double)yi;

	double di = sqrt(dx * dx + dy * dy); // Length of integer vector, like a traditional EDT
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

//#define DISTAA(row, col) (distaa3(data, gx, gy, grid, row, col))
void Compare(std::vector<std::vector<double>>& data,
	std::vector<std::vector<Point>>& dist,
	std::vector<std::vector<double>>& gx,
	std::vector<std::vector<double>>& gy,
	int row, int col, int& h, int& w, int offsetx, int offsety, bool& changed) {
	double epsilon = 1e-3; // Safeguard against errors due to limited precision
	if (dist[row][col].dist <= 0) return;

	int new_row = row + offsetx, new_col = col + offsety;  // Index of candidate for testing  
	if (isValid(new_row, new_col, h, w)) {
		int cdistx = dist[new_row][new_col].distx;     // ��ȡ�����
		int cdisty = dist[new_row][new_col].disty;	   // ��ȡ�����
		int newdistx = cdistx - offsetx;			   // ����������offset��
		int newdisty = cdisty - offsety;			   // ����������offset��
		int closestx = new_row - cdistx;
		int closesty = new_col - cdisty;
		double newdist = distaa3(data, gx, gy, dist, closestx, closesty, newdistx, newdisty);
		double olddist = dist[row][col].dist;

		if (newdist < olddist - epsilon) {
			dist[row][col].dist = newdist;
			dist[row][col].distx = newdistx;
			dist[row][col].disty = newdisty;
			changed = true;
		}
	}
}

void edtaa3(std::vector<std::vector<double>>& data,
	std::vector<std::vector<double>>& gx,
	std::vector<std::vector<double>>& gy,
	std::vector<std::vector<Point>>& dist,
	int w, int h)
{
	/* Initialize the distance images */
	for (int row = 0; row < h; row++) {
		for (int col = 0; col < w; col++) {
			dist[row][col].distx = dist[row][col].disty = 0;
			if (data[row][col] <= 0.0) {
				dist[row][col].dist = 1000000.0; // Big value, means "not set yet"
			}
			else if (data[row][col] < 1.0) {
				dist[row][col].dist = edgedf(gx[row][col], gy[row][col], data[row][col]); // Gradient-assisted estimate
			}
			else {  // img[i] == 1.0
				dist[row][col].dist = 0.0; // Inside the object
			}
		}

	}

	// http://www.codersnotes.com/notes/signed-distance-fields/
	int time = 0;
	bool changed = false;
	do {
		time++;
		changed = false;
		for (int row = 0; row < h; row++) {
			for (int col = 0; col < w; col++) {
				Compare(data, dist, gx, gy, row, col, h, w, -1, -1, changed);
				Compare(data, dist, gx, gy, row, col, h, w, -1, -0, changed);
				Compare(data, dist, gx, gy, row, col, h, w, -1, 1, changed);
				Compare(data, dist, gx, gy, row, col, h, w, 0, -1, changed);
			}

			for (int col = w - 1; col >= 0; col--) {
				Compare(data, dist, gx, gy, row, col, h, w, 1, 0, changed);
			}
		}

		for (int row = h - 1; row >= 0; row--) {
			for (int col = w - 1; col >= 0; col--) {
				Compare(data, dist, gx, gy, row, col, h, w, 0, 1, changed);
				Compare(data, dist, gx, gy, row, col, h, w, 1, -1, changed);
				Compare(data, dist, gx, gy, row, col, h, w, 1, 0, changed);
				Compare(data, dist, gx, gy, row, col, h, w, 1, 1, changed);
			}

			for (int col = 0; col < w; col++) {
				Compare(data, dist, gx, gy, row, col, h, w, 0, -1, changed);
			}
		}
	} while (changed);
}