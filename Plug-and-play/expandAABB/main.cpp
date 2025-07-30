#include <iostream>
#include <vector>
#include <utility>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

void setColor(unsigned char* data, int width, int nrChannels, int row, int col, int index, unsigned char val) {
	data[width * nrChannels * row + nrChannels * col + index] = val;
}

int getColor(unsigned char* data, int width, int nrChannels, int row, int col, int index) {
	return data[width * nrChannels * row + nrChannels * col + index];
}


class AABB {
public:
	struct Point {
		int x, y;

		Point() : x(0.0), y(0.0) {}
		Point(int x, int y) : x(x), y(y) {}

		bool operator<(const Point& other) const {
			return x < other.x || (x == other.x && y < other.y);
		}
	};

public:
	Point m_minCorner{ INT_MAX, INT_MAX };  // AABB的最小角点
	Point m_maxCorner{ INT_MIN, INT_MIN };  // AABB的最大角点

public:
	// 构造函数
	AABB() {}
	AABB(const Point& min, const Point& max) : m_minCorner(min), m_maxCorner(max) {}

	// 获取最小角点
	Point GetMinCorner() const {
		return m_minCorner;
	}

	// 获取最大角点
	Point GetMaxCorner() const {
		return m_maxCorner;
	}

	// 重置AABB
	void Reset() {
		m_minCorner = Point{ INT_MAX, INT_MAX };
		m_maxCorner = Point{ INT_MIN, INT_MIN };
	}

	void Expand(const Point& point) {
		m_minCorner.x = std::min(m_minCorner.x, point.x);
		m_minCorner.y = std::min(m_minCorner.y, point.y);
		m_maxCorner.x = std::max(m_maxCorner.x, point.x);
		m_maxCorner.y = std::max(m_maxCorner.y, point.y);
	}

	bool Contains(const Point& point) const {
		return point.x >= m_minCorner.x && point.x <= m_maxCorner.x &&
			point.y >= m_minCorner.y && point.y <= m_maxCorner.y;
	}

	bool Overlaps(const AABB& other) const {
		return m_minCorner.x <= other.m_maxCorner.x && m_maxCorner.x >= other.m_minCorner.x &&
			m_minCorner.y <= other.m_maxCorner.y && m_maxCorner.y >= other.m_minCorner.y;
	}

	AABB Combine(const AABB& other) const {
		return AABB(
			Point(std::min(m_minCorner.x, other.m_minCorner.x),
				std::min(m_minCorner.y, other.m_minCorner.y)),
			Point(std::max(m_maxCorner.x, other.m_maxCorner.x),
				std::max(m_maxCorner.y, other.m_maxCorner.y))
		);
	}

	AABB Intersection(const AABB& other) const {
		if (!Overlaps(other)) {
			return AABB(); // 返回空AABB
		}
		return AABB(
			Point(std::max(m_minCorner.x, other.m_minCorner.x),
				std::max(m_minCorner.y, other.m_minCorner.y)),
			Point(std::min(m_maxCorner.x, other.m_maxCorner.x),
				std::min(m_maxCorner.y, other.m_maxCorner.y))
		);
	}

	Point GetCenter() const {
		return Point((m_minCorner.x + m_maxCorner.x) / 2.0f,
			(m_minCorner.y + m_maxCorner.y) / 2.0f);
	}

	Point GetSize() const {
		return Point(m_maxCorner.x - m_minCorner.x,
			m_maxCorner.y - m_minCorner.y);
	}

	float GetArea() const {
		return (m_maxCorner.x - m_minCorner.x) * (m_maxCorner.y - m_minCorner.y);
	}
};


void ExpandImageUsingAABB(unsigned char* img, int& width, int& height, int& nrChannels, int dataChannel, unsigned char* out, int outWidth, int outHeight, int outChannel)
{
	AABB aabb{ {width, height}, {0,0} };
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			double val = getColor(img, width, nrChannels, row, col, dataChannel);
			if (val != 1.0) {
				aabb.Expand({ row, col });
			}
		}
	}

	// init the image with width color
	for (uint32_t row = 0; row < outHeight; row++) {
		for (uint32_t col = 0; col < outWidth; col++) {
			setColor(out, outWidth, outChannel, row, col, 0, static_cast<unsigned char>(255.0));
			setColor(out, outWidth, outChannel, row, col, 1, static_cast<unsigned char>(255.0));
			setColor(out, outWidth, outChannel, row, col, 2, static_cast<unsigned char>(255.0));
			setColor(out, outWidth, outChannel, row, col, 3, static_cast<unsigned char>(255.0));
		}
	}

	AABB::Point center = aabb.GetCenter();
	AABB::Point size = aabb.GetSize();
	auto& aabbHeight = size.x;
	auto& aabbWidth = size.y;
	AABB::Point aabbMin = aabb.GetMinCorner();
	AABB::Point aabbMax = aabb.GetMaxCorner();

	int dstStartCol = (outWidth - aabbWidth) / 2;
	int dstStartRow = (outHeight - aabbHeight) / 2;

	for (uint32_t row = 0; row < aabbHeight; row++) {
		for (uint32_t col = 0; col < aabbWidth; col++) {
			int srcRow = static_cast<int>(aabbMin.x) + row;
			int srcCol = static_cast<int>(aabbMin.y) + col;

			if (srcCol >= 0 && srcCol < width && srcRow >= 0 && srcRow < height) {
				int dstRow = dstStartRow + row;
				int dstCol = dstStartCol + col;

				setColor(out, outWidth, outChannel, dstRow, dstCol, 0, static_cast<unsigned char>(getColor(img, width, nrChannels, row, col, 0)));
				setColor(out, outWidth, outChannel, dstRow, dstCol, 1, static_cast<unsigned char>(getColor(img, width, nrChannels, row, col, 0)));
				setColor(out, outWidth, outChannel, dstRow, dstCol, 2, static_cast<unsigned char>(getColor(img, width, nrChannels, row, col, 0)));
				setColor(out, outWidth, outChannel, dstRow, dstCol, 3, static_cast<unsigned char>(getColor(img, width, nrChannels, row, col, 0)));
			}
		}
	}
}

int main() {
	int width = 0, height = 0, nrChannels = 0;

	const char* path = "../resources/imgs/flipper128.png";
	unsigned char* img = stbi_load(path, &width, &height, &nrChannels, 0);

	if (!img)
	{
		std::cout << "Failed to read image: " << path << std::endl;
	}

	std::cout << "Input image:" << width << "x" << height << "x" << nrChannels << std::endl;;
	int outChannel = 4;
	int outWidth = 256;
	int outHeight = 256;
	unsigned char* out = new unsigned char[outWidth * outHeight * outChannel * sizeof(unsigned char)];
	ExpandImageUsingAABB(img, width, height, nrChannels, 0, out, outWidth, outHeight, outChannel);

	stbi_write_png("./output.png", outWidth, outHeight, outChannel, out, outWidth * outChannel);
}