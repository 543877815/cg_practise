#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class t> struct Vec2 {
	union {
		struct { t u, v; };
		struct { t x, y; };
		t raw[2];
	};
	Vec2() : u(0), v(0) {}
	Vec2(t _u, t _v) : u(_u), v(_v) {}
	inline Vec2<t> operator +(const Vec2<t>& V) const { return Vec2<t>(u + V.u, v + V.v); }
	inline Vec2<t> operator -(const Vec2<t>& V) const { return Vec2<t>(u - V.u, v - V.v); }
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(u * f, v * f); }
	inline Vec2<t> operator /(float f)          const { return Vec2<t>(u / f, v / f); }
	inline t operator [](int i) const { return (i == 0) ? u : v; }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);
};

template <class t> struct Vec3 {
	union {
		struct { t x, y, z; };
		struct { t ivert, iuv, inorm; };
		t raw[3];
	};
	Vec3() : x(0), y(0), z(0) {}
	Vec3(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}
	inline Vec3<t> operator ^(const Vec3<t>& v) const { return Vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
	inline Vec3<t> operator +(const Vec3<t>& v) const { return Vec3<t>(x + v.x, y + v.y, z + v.z); }
	inline Vec3<t> operator -(const Vec3<t>& v) const { return Vec3<t>(x - v.x, y - v.y, z - v.z); }
	inline Vec3<t> operator *(float f)          const { return Vec3<t>(x * f, y * f, z * f); }
	inline Vec3<t> operator /(float f)          const { return Vec3<t>(x / f, y / f, z / f); }
	inline t       operator *(const Vec3<t>& v) const { return x * v.x + y * v.y + z * v.z; }
	float norm() const { return std::sqrt(x * x + y * y + z * z); }
	Vec3<t>& normalize(t l = 1) { *this = (*this) * (l / norm()); return *this; }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
};

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}


template<typename T>
struct Line {
	Vec3<T> m_origin;
	Vec3<T> m_direction;
	float m_distance;
	Line(T* vertices) : Line(Vec3<T>{vertices[0], vertices[1], vertices[2]},
		Vec3<T>{vertices[3], vertices[4], vertices[5]}) {}

	Line(Vec3<T> v1, Vec3<T> v2) {
		m_origin = v1;
		m_direction = v2 - v1;
		float magnitude = std::sqrt(m_direction * m_direction);
		if (magnitude == 0.0f)  throw std::logic_error("Cannot normalize a zero vector");
		m_direction = m_direction / magnitude;
	}

};

template<typename t>
struct Plain {
	Vec3<t> v1, v2, v3;
	Vec3<t> m_normal;
	Plain(Vec3<t> v1, Vec3<t> v2, Vec3<t> v3) {
		Line <t> line1{ v1, v2 };
		Line <t> line2{ v1, v3 };
		m_normal.x = line1.m_direction.y * line2.m_direction.z - line1.m_direction.z * line2.m_direction.y;
		m_normal.y = line1.m_direction.z * line2.m_direction.x - line1.m_direction.x * line2.m_direction.z;
		m_normal.z = line1.m_direction.x * line2.m_direction.y - line1.m_direction.y * line2.m_direction.x;

		//std::cout << line1.m_direction * m_normal << std::endl;
		//std::cout << line2.m_direction * m_normal << std::endl;
	}
};

#endif //__GEOMETRY_H__