#pragma once
#include <iostream>
#include <algorithm> // ����std::min��std::max



class AABB {
public:
	// ����������
	struct Point {
		float x, y, z;

		Point() : x(0.0), y(0.0), z(0.0) {}
		Point(float x, float y, float z) : x(x), y(y), z(z) {}

		// ���������<���Ա���std::min��std::max��ʹ��
		bool operator<(const Point& other) const {
			return x < other.x || (x == other.x && (y < other.y || (y == other.y && z < other.z)));
		}
	};

private:
	Point minCorner; // AABB����С�ǵ�
	Point maxCorner; // AABB�����ǵ�

public:
	// ���캯��
	AABB() {};
	AABB(const Point& min, const Point& max) : minCorner(min), maxCorner(max) {}

	// ��ȡ��С�ǵ�
	Point getMinCorner() const {
		return minCorner;
	}

	// ��ȡ���ǵ�
	Point getMaxCorner() const {
		return maxCorner;
	}

	// ��չ AABB �԰��������ĵ�
	void expand(const Point& point) {
		minCorner.x = (std::min)(minCorner.x, point.x);
		minCorner.y = (std::min)(minCorner.y, point.y);
		minCorner.z = (std::min)(minCorner.z, point.z);
		maxCorner.x = (std::max)(maxCorner.x, point.x);
		maxCorner.y = (std::max)(maxCorner.y, point.y);
		maxCorner.z = (std::max)(maxCorner.z, point.z);
	}

	// �����Ƿ���AABB��
	bool contains(const Point& point) const {
		return point.x >= minCorner.x && point.x <= maxCorner.x &&
			point.y >= minCorner.y && point.y <= maxCorner.y &&
			point.z >= minCorner.z && point.z <= maxCorner.z;
	}

	// �������AABB�Ƿ��ص�
	bool overlaps(const AABB& other) const {
		return minCorner.x <= other.maxCorner.x && maxCorner.x >= other.minCorner.x &&
			minCorner.y <= other.maxCorner.y && maxCorner.y >= other.minCorner.y &&
			minCorner.z <= other.maxCorner.z && maxCorner.z >= other.minCorner.z;
	}

	// ��ӡAABB��Ϣ
	void print() const {
		std::cout << "AABB: Min Corner (" << minCorner.x << ", " << minCorner.y << ", " << minCorner.z << ") "
			<< "Max Corner (" << maxCorner.x << ", " << maxCorner.y << ", " << maxCorner.z << ")" << std::endl;
	}
};

