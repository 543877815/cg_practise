#include <iostream>
#include <vector>
#include <iomanip>

#include <Eigen/Dense>
#include <glm/vec3.hpp>
#include <glm/glm.hpp> // ����GLM��ͷ�ļ�
#include <glm/gtx/string_cast.hpp> // ��ѡ�����ڽ����ת��Ϊ�ַ���

#include "mat.h"
typedef std::vector<std::vector<double>> mat_d;

void PCA() {
	// �������ݼ�X��һ��m x n�ľ��󣬿���ͨ��Eigen::Matrix����ʾ
	Eigen::MatrixXd data(10, 2); // ��ʼ�����ݼ�X

	data << 3.7, 1.7,
		4.1, 3.8,
		4.7, 2.9,
		5.2, 2.8,
		6.0, 4.0,
		6.3, 3.6,
		9.7, 6.3,
		10.0, 4.9,
		11.0, 3.6,
		12.5, 6.4;

	// �����ݽ������Ļ�����
	Eigen::VectorXd mean = data.colwise().mean();
	data.rowwise() -= mean.transpose();


	// ����Э�������
	Eigen::MatrixXd cov = data.transpose() * data / (data.rows() - 1);

	Eigen::MatrixXd cov1(3, 3);
	cov1 << 4, -2, 2,
		-2, 3, -1,
		2, -1, 3;

	std::cout << cov << std::endl;

	// ��Э��������������ֵ�ֽ�
	Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigensolver(cov1);
	if (eigensolver.info() != Eigen::Success) {
		std::cerr << "Eigen decomposition failed" << std::endl;
		return;
	}

	// ѡ��ǰk������ֵ��Ӧ��������������PCA����
	int k = 2; // ����ѡ��ǰ2�����ɷ�
	Eigen::MatrixXd top_k_eigenvectors = eigensolver.eigenvectors().rightCols(k);
	std::cout << eigensolver.eigenvectors() << std::endl;
	std::cout << eigensolver.eigenvalues() << std::endl;

	// PCA����Ϊtop_k_eigenvectors
	Eigen::MatrixXd PCA_matrix = top_k_eigenvectors;


}

// ����һϵ�е�����OBB��code from GPT
void OBB(std::vector<glm::vec3> points) {
	size_t n = points.size();
	Eigen::MatrixXd data(n, 2);

	// �����ݽ������Ļ�����
	Eigen::VectorXd mean = data.colwise().mean();
	data.rowwise() -= mean.transpose();

	// �������ݵ�Э�������
	Eigen::VectorXd cov = data.transpose() * data / (data.rows() - 1);

	// ��Э�����������ֵ�ֽ�
	Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig(cov);

	// ѡȡǰk����������ֵ��Ӧ������������Ϊ���ɷ�
	int k = 2;
	Eigen::MatrixXd pcs = eig.eigenvectors().rightCols(k);

	Eigen::Vector2d maxExtends(INT_MIN, INT_MIN);
	Eigen::Vector2d minExtends(INT_MAX, INT_MAX);
	Eigen::Vector2d eigen_x = pcs.col(0);
	Eigen::Vector2d eigen_y = pcs.col(1);

	if (eigen_x.x() < 0) eigen_x = -eigen_x;
	if (eigen_y.y() < 0) eigen_y = -eigen_y;

	// �����ĵ�
	Eigen::Vector2d center(0.0, 0.0);
	for (int i = 0; i < n; i++) {
		Eigen::Vector2d tmp(points[i].x, points[i].y);
		center += tmp;
	}
	center = center / n;

	// ��ÿ���������������ϵ�ͶӰ
	for (int i = 0; i < n; i++) {
		Eigen::Vector2d point(points[i].x, points[i].y);
		Eigen::Vector2d displacement = point - center;
		maxExtends.x() = std::max(maxExtends.x(), displacement.dot(eigen_x));
		minExtends.x() = std::max(minExtends.x(), displacement.dot(eigen_x));
		maxExtends.y() = std::max(maxExtends.y(), displacement.dot(eigen_y));
		minExtends.y() = std::max(minExtends.y(), displacement.dot(eigen_y));
	}

	// �������ĵ㣬ͶӰ�ռ���
	Eigen::Vector2d HalfLen = (maxExtends - minExtends) / 2.0;
	Eigen::Vector2d offset = HalfLen + minExtends;

	// OBB�е�
	center += (eigen_x * offset.x());
	center += (eigen_y * offset.y());

	// OBB�ĸ���
	Eigen::Vector2d lb = center - HalfLen.x() * eigen_x - HalfLen.y() * eigen_y;
	Eigen::Vector2d lt = center - HalfLen.x() * eigen_x + HalfLen.y() * eigen_y;
	Eigen::Vector2d rb = center + HalfLen.x() * eigen_x - HalfLen.y() * eigen_y;
	Eigen::Vector2d rt = center + HalfLen.x() * eigen_x + HalfLen.y() * eigen_y;
}


// ����OBB�ĸ���͹��ߵ����ͷ��򣬼�����ߺ�OBB�Ľ��㡣
// https://zhuanlan.zhihu.com/p/138259656
bool CalculateIntersectionWithOBB(
	const std::vector<glm::vec3>& bounds,
	const glm::vec3& rayPoint,
	const glm::vec3& rayDir,
	glm::vec3& intersection,
	const glm::vec3& u,
	const glm::vec3& v)
{
	// Calculate the center of the OBB
	glm::vec3 center = (bounds[0] + bounds[1] + bounds[2] + bounds[3]) * 0.25f;

	// Calculate the half-lengths of the OBB along each local axis
	std::vector<float> len{
		glm::length(bounds[2] - bounds[1]) * 0.5f,
		glm::length(bounds[1] - bounds[0]) * 0.5f
	};

	// Define the local axes of the OBB
	std::vector<glm::vec3> axis{ u, v };

	// Initialize the min and max values for the ray's parameter t
	auto ray_t_min = -9999.0f;
	auto ray_t_max = 9999.0f;

	// Test intersection with each axis of the OBB
	for (uint32_t a = 0; a < axis.size(); ++a) {
		auto r = glm::dot(axis[a], rayDir);
		auto s = glm::dot(axis[a], (center - rayPoint));

		if (r == 0.0f) {
			// Ray is parallel to the current axis
			if ((-s - len[a]) > 0.0f || (-s + len[a]) < 0.0f) {
				return false; // No intersection
			}
		}
		else {
			// Ray is not parallel to the current axis
			float r_inv = 1.0f / r;

			auto t0 = (s - len[a]) * r_inv;
			auto t1 = (s + len[a]) * r_inv;

			// Swap t0 and t1 if necessary
			if (t0 > t1) {
				std::swap(t0, t1);
			}

			// Update the ray's min and max parameter values
			if (t0 > ray_t_min) {
				ray_t_min = t0;
			}
			if (t1 < ray_t_max) {
				ray_t_max = t1;
			}

			// Check if the interval is valid
			if (ray_t_max < ray_t_min || ray_t_max < 0.0f) {
				return false; // No intersection
			}
		}
	}

	// Compute the final intersection point
	float t;
	if (ray_t_min > 0.0f) {
		t = ray_t_min;
	}
	else {
		t = ray_t_max;
	}
	intersection = rayPoint + rayDir * t;

	return true; // Intersection found
}

int main() {
	mat_d data{
		{3.7, 1.7},
		{4.1, 3.8},
		{4.7, 2.9},
		{5.2, 2.8},
		{6.0, 4.0},
		{6.3, 3.6},
		{9.7, 6.3},
		{10.0, 4.9},
		{11.0, 3.6},
		{12.5, 6.4}
	};

	Mat<double> mat(data);
	Mat<double> norm_mat = mat.normalize();
	Mat<double> mat_T = mat.transpose();
	Mat<double> mat_mul = mat.mul(2.0);
	Mat<double> mat_matmul = mat_T.matmul(mat);
	Mat<double> cov = norm_mat.covariance();
	double det = cov.determinant();

	mat_d data1{
		{4, -1, 0},
		{-1, 4, -1},
		{0, -1, 4}
	};
	Mat<double> mat1(data1);

	std::vector<double> b = { 8, 7, 6 };
	int max_iter = 100;
	double tol = 1e-6;
	std::vector<double> solution = Mat<double>::JacobiIteration(data1, b, max_iter, tol);
	for (auto a : solution) {
		std::cout << a << " ";
	}
}