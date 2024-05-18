#include <iostream>
#include <vector>

using namespace std;

vector<double> jacobiIteration(vector<vector<double>> A, vector<double> b, int max_iter, double tol) {
	int n = A.size();
	vector<double> x(n, 0.0);
	vector<double> x_new(n, 0.0);
	int iter = 0;

	while (iter < max_iter) {
		for (int i = 0; i < n; i++) {
			double sum = 0.0;
			for (int j = 0; j < n; j++) {
				if (j != i) {
					sum += A[i][j] * x[j];
				}
			}
			x_new[i] = (b[i] - sum) / A[i][i];
		}

		double error = 0.0;
		for (int i = 0; i < n; i++) {
			error += abs(x_new[i] - x[i]);
		}

		if (error < tol) {
			break;
		}

		x = x_new;
		iter++;
	}

	return x;
}

int main() {
	vector<vector<double>> A = { {4, -1, 0},
								 {-1, 4, -1},
								 {0, -1, 4} };
	vector<double> b = { 8, 7, 6 };
	int max_iter = 100;
	double tol = 1e-6;

	vector<double> solution = jacobiIteration(A, b, max_iter, tol);

	cout << "Solution: ";
	for (int i = 0; i < solution.size(); i++) {
		cout << solution[i] << " ";
	}
	cout << endl;

	return 0;
}