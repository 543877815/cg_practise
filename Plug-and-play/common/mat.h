#include<vector>
#include<iostream>


// TODO: move to vec
template<typename T>
class Vector {
private:
	std::vector<T> m_data;

public:
	Vector(const std::vector<T>& vec) : m_data(vec) {}

	Vector operator-(const Vector& other) {
		std::vector<T> result((*this).size());
		for (int i = 0; i < (*this).size(); i++) {
			result[i] = m_data[i] - other.m_data[i];
		}
		return Vector(result);
	}

	Vector operator+(const Vector& other) {
		std::vector<T> result((*this).size());
		for (int i = 0; i < (*this).size(); i++) {
			result[i] = m_data[i] + other.m_data[i];
		}
		return Vector(result);
	}

	size_t size() { return m_data.size(); }

	friend std::ostream& operator<<(std::ostream& os, Vector<T> vec)
	{
		for (int i = 0; i < vec.size(); i++) {
			std::cout << m_data[i] << " ";

		}
		os << std::endl;
		return os;
	}

	T& operator[](size_t index) {
		return m_data[index];
	}


};

// TODO: google test
template<typename T>
class Mat {
public:
	std::vector<std::vector<T>> m_data;
	size_t m_row = 0, m_col = 0;

	Mat(size_t row = 0, size_t col = 0) {
		this->m_data = std::vector<std::vector<T>>(row, std::vector<T>(col, 0));
		this->m_row = row;
		this->m_col = col;
	}

	Mat(size_t row, size_t col, std::vector<std::vector<T>>& mat) {
		this->m_data = mat;
		this->m_row = row;
		this->m_col = col;
	}

	Mat(std::vector<std::vector<T>>& mat) {
		this->m_data = mat;
		this->m_row = mat.size();
		this->m_col = mat[0].size();
	}

	size_t getRow() { return m_row; }
	size_t getCol() { return m_col; }
	size_t size() { return m_data.size(); }
	void push_back(std::vector<T>& row) {
		this->m_data.push_back(row);
		this->m_row = this->m_data.size();
		this->m_col = this->m_data[0].size();
	}

	Mat<T> transpose() {
		Mat<T> res = Mat(m_col, m_row);
		for (int i = 0; i < m_col; i++) {
			for (int j = 0; j < m_row; j++) {
				res[i][j] = m_data[j][i];
			}
		}
		return res;
	}

	Mat<T> mul(T t) {
		Mat<T> res = m_data;
		for (int i = 0; i < m_row; i++) {
			for (int j = 0; j < m_col; j++) {
				res[i][j] = res[i][j] * t;
			}
		}
		return res;
	}

	Mat<T> matmul(Mat<T>& mat) {
		if (this->m_col != mat.m_row) {
			std::cout << "Dim-matmul" << std::endl;
			exit(-1);
		};
		size_t m = this->m_row;
		size_t n = this->m_col;
		size_t p = mat.m_col;
		Mat<T> res = Mat<T>(m, p);

		for (size_t i = 0; i < m; i++) {
			for (size_t j = 0; j < n; j++) {
				for (size_t k = 0; k < p; k++) {
					res[i][k] += m_data[i][j] * mat[j][k];
				}
			}
		}
		return res;
	}

	Mat<T> covariance() {
		Mat<T> res = this->transpose().matmul(*this);
		if (this->m_row < 2) {
			std::cout << "Dim" << std::endl;
			exit(-1);
		}
		res = res.mul(1.0 / (this->m_row - 1));
		return res;
	}

	T determinant() {
		if (this->m_row != this->m_col) {
			std::cerr << "Error: Determinant can only be calculated for square matrices." << std::endl;
			return 0.0;
		}

		if (this->m_row == 1) {
			return (*this)[0][0];
		}

		double det = 0.0;
		int sign = 1;

		for (size_t i = 0; i < this->m_row; i++) {
			Mat<T> submat = submatrix(0, i);
			det += sign * (*this)[0][i] * submat.determinant();
			sign = -sign;
		}

		return det;
	}

	Mat<T> submatrix(size_t row, size_t col) {
		Mat<T> submat;
		for (size_t i = 0; i < this->m_row; i++) {
			if (i == row) continue;
			std::vector<T> subrow;
			for (size_t j = 0; j < this->m_col; j++) {
				if (j == col) continue;
				subrow.push_back((*this)[i][j]);
			}
			submat.push_back(subrow);
		}
		return submat;
	}

	Mat<T> normalize() {
		std::vector<double>colWiseMean(m_col);
		for (size_t col = 0; col < this->m_col; col++) {
			double mean = 0.0;
			for (size_t row = 0; row < this->m_row; row++) {
				mean += (*this)[row][col];
			}
			colWiseMean[col] = mean / this->m_row;
		}

		for (size_t row = 0; row < this->m_row; row++) {
			for (size_t col = 0; col < this->m_col; col++) {
				(*this)[row][col] -= colWiseMean[col];
			}
		}
		return *this;
	}

	static std::vector<T> VectorMultiplyMatrix(std::vector<T>& vec, Mat<T>& mat) {
		std::vector<T> result(mat.m_col, 0);
		for (int col = 0; col < mat.m_col; col++) {
			for (int row = 0; row < mat.m_row; row++) {
				result[col] += vec[row] * mat[row][col];
			}
		}

		return result;
	}

	static std::vector<T> MatrixMultiplyVector(const Mat<T>& mat, const std::vector<T>& vec) {
		std::vector<T> result(mat.m_row, 0);
		for (int row = 0; row < mat.m_row; row++) {
			for (int col = 0; col < mat.m_col; col++) {
				result[row] += mat[row][col] * vec[col];
			}
		}
		return result;
	}

	static std::vector<T> VectorAdd(std::vector<T>& a, std::vector<T>& b) {
		assert(a.size() == b.size());
		std::vector<T> result(a.size());
		for (size_t i = 0; i < a.size(); i++) {
			result[i] = a[i] + b[i];
		}
		return result;
	}

	static T AbsDist(std::vector<T>& vec1, std::vector<T>& vec2) {
		assert(vec1.size() == vec2.size());
		T dist = 0.0;
		for (size_t i = 0; i < vec1.size(); i++) {
			dist += std::abs(vec1[i] - vec2[i]);
		}
		return dist;
	}

	static Mat<T> GetD(const Mat<T>& mat) {
		Mat<T> result(mat.m_row, mat.m_col);
		for (int i = 0; i < mat.m_row; i++) {
			result[i][i] = mat[i][i];
		}
		return result;
	}

	static Mat<T> GetDInv(const Mat<T>& mat) {
		Mat<T> result(mat.m_row, mat.m_col);
		for (int i = 0; i < mat.m_row; i++) {
			result[i][i] = 1 / mat[i][i];
		}
		return result;
	}

	static Mat<T> GetL(const Mat<T>& mat) {
		Mat<T> result(mat.m_row, mat.m_col);
		for (int i = 1; i < mat.m_row; i++) {
			for (int j = 0; j < i; j++) {
				result[i][j] = mat[i][j];
			}
		}
		return result;
	}

	static Mat<T> GetU(const Mat<T>& mat) {
		Mat<T> result(mat.m_row, mat.m_col);
		for (int i = 0; i < mat.m_row; i++) {
			for (int j = i + 1; j < mat.m_col; j++) {
				result[i][j] = mat[i][j];
			}
		}
		return result;
	}

	static std::vector<T> JacobiIteration(const Mat<T>& A, const std::vector<T>& b, int max_iter = 100, T tol = 1e-6) {
		Mat<T> D_inv = GetDInv(A);
		Mat<T> L = GetL(A);
		Mat<T> U = GetU(A);
		Mat<T> LplusU = L + U;
		Mat<T> B = -D_inv.matmul(LplusU);
		std::vector<T> d = Mat::MatrixMultiplyVector(D_inv, b);
		std::vector<T> x(d.size(), 0.0);
		std::vector<T> x_new(d.size(), 0.0);
		Mat<T> res;

		int iter = 0;

		while (iter < max_iter) {
			std::vector<T> Bx = Mat::MatrixMultiplyVector(B, x);
			x_new = Mat::VectorAdd(Bx, d);
			T error = Mat::AbsDist(x, x_new);

			if (error < tol) {
				break;
			}

			x = x_new;
			iter++;
		}
		return x;
	}


	std::vector<T>& operator[](size_t index) {
		return m_data[index];
	}

	std::vector<T> operator[](size_t index) const {
		return m_data[index];
	}
	Mat<T> operator+(Mat<T>& mat)
	{
		assert(this->m_col == mat.m_col && this->m_row == mat.m_row);
		Mat<T> result(this->m_row, this->m_col);
		for (size_t i = 0; i < this->m_row; i++) {
			for (size_t j = 0; j < this->m_col; j++) {
				result[i][j] = (*this)[i][j] + mat[i][j];
			}
		}
		return result;
	}

	Mat<T> operator-(const Mat<T>& mat) {
		assert(this->m_col == mat.m_col && this->m_row == mat.m_row);
		Mat<T> result(this->m_row, this->m_col);
		for (size_t i = 0; i < this->m_row; i++) {
			for (size_t j = 0; j < this->m_col; j++) {
				result[i][j] = (*this)[i][j] - mat[i][j];
			}
		}
		return result;
	}

	Mat<T> operator-() {
		Mat<T> result(this->m_row, this->m_col);
		for (size_t i = 0; i < this->m_row; i++) {
			for (size_t j = 0; j < this->m_col; j++) {
				result[i][j] = -(*this)[i][j];
			}
		}
		return result;
	}


	friend std::ostream& operator<<(std::ostream& os, Mat<T> mat)
	{
		for (int i = 0; i < mat.m_row; i++) {
			for (int j = 0; j < mat.m_col; j++) {
				os << std::fixed << std::setprecision(3) << std::showpos << std::setw(8) << mat.m_data[i][j] << "\t";
			}
			os << std::endl;
		}
		os << std::endl;
		return os;
	}

	friend std::ostream& operator<<(std::ostream& os, Mat<T>& mat)
	{
		for (int i = 0; i < mat.m_row; i++) {
			for (int j = 0; j < mat.m_col; j++) {
				os << std::fixed << std::setprecision(3) << std::showpos << std::setw(8) << mat.m_data[i][j] << "\t";
			}
			os << std::endl;
		}
		os << std::endl;
		return os;
	}

};
