#include <iostream>
#include <cmath>
#include <vector>
#include "geometry.h"


// 判断线与线之间的相交
bool LineLineIntersection(Vec3<float>& intersection, const Vec3<float>& p1, const Vec3<float>& v1,
	const Vec3<float>& p2, const Vec3<float>& v2) {
	Vec3<float> vecS1 = v1 ^ v2; // 向量叉乘
	Vec3<float> startPointSeg = p2 - p1;
	float num = startPointSeg * vecS1;

	if (std::abs(num) < 1E-5f) {
		// 两直线平行
		return false;
	}

	Vec3<float> vecS2 = startPointSeg ^ v2;
	float num2 = vecS2 * vecS1 / vecS1.norm2();

	intersection = p1 + v1 * num2;
	return true;
}

int main() {
	Vec3<float> p1 = { 1.0f, 2.0f, 3.0f };
	Vec3<float> v1 = { 2.0f, 1.0f, 0.0f };
	Vec3<float> p2 = { 4.0f, 3.0f, 2.0f };
	Vec3<float> v2 = { -1.0f, 1.0f, 1.0f };

	Vec3<float> intersection;
	if (LineLineIntersection(intersection, p1, v1, p2, v2)) {
		std::cout << "Lines intersect at (" << intersection.x << ", "
			<< intersection.y << ", " << intersection.z << ")" << std::endl;
	}
	else {
		std::cout << "Lines do not intersect." << std::endl;
	}

	return 0;
}

