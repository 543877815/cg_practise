#pragma once
#include <iostream>
#include <algorithm> // 用于std::min和std::max



class AABB {
public:
	// 定义点的类型
	struct Point {
		float x, y, z;

		Point() : x(0.0), y(0.0), z(0.0) {}
		Point(float x, float y, float z) : x(x), y(y), z(z) {}

		// 重载运算符<，以便于std::min和std::max的使用
		bool operator<(const Point& other) const {
			return x < other.x || (x == other.x && (y < other.y || (y == other.y && z < other.z)));
		}
	};

private:
	Point minCorner; // AABB的最小角点
	Point maxCorner; // AABB的最大角点

public:
	// 构造函数
	AABB() {};
	AABB(const Point& min, const Point& max) : minCorner(min), maxCorner(max) {}

	// 获取最小角点
	Point getMinCorner() const {
		return minCorner;
	}

	// 获取最大角点
	Point getMaxCorner() const {
		return maxCorner;
	}

	// 扩展 AABB 以包含给定的点
	void expand(const Point& point) {
		minCorner.x = (std::min)(minCorner.x, point.x);
		minCorner.y = (std::min)(minCorner.y, point.y);
		minCorner.z = (std::min)(minCorner.z, point.z);
		maxCorner.x = (std::max)(maxCorner.x, point.x);
		maxCorner.y = (std::max)(maxCorner.y, point.y);
		maxCorner.z = (std::max)(maxCorner.z, point.z);
	}

	// 检查点是否在AABB内
	bool contains(const Point& point) const {
		return point.x >= minCorner.x && point.x <= maxCorner.x &&
			point.y >= minCorner.y && point.y <= maxCorner.y &&
			point.z >= minCorner.z && point.z <= maxCorner.z;
	}

	// 检查两个AABB是否重叠
	bool overlaps(const AABB& other) const {
		return minCorner.x <= other.maxCorner.x && maxCorner.x >= other.minCorner.x &&
			minCorner.y <= other.maxCorner.y && maxCorner.y >= other.minCorner.y &&
			minCorner.z <= other.maxCorner.z && maxCorner.z >= other.minCorner.z;
	}

	// 打印AABB信息
	void print() const {
		std::cout << "AABB: Min Corner (" << minCorner.x << ", " << minCorner.y << ", " << minCorner.z << ") "
			<< "Max Corner (" << maxCorner.x << ", " << maxCorner.y << ", " << maxCorner.z << ")" << std::endl;
	}
};

