#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cmath>

template<class t>struct Vec2
{
	union {
		struct { t u, v; };
		struct { t x, y; };
		t raw[2];
	};
	Vec2() :u(0), v(0) {}
	Vec2(t _u, t _v) :u(_u), v(_v) {}
	inline Vec2<t> operator +(const Vec2<t>& V) const { return Vec2<t>(u + V.u, v + V.v); }
	inline Vec2<t> operator -(const Vec2<t> & V) const { return Vec2<t>(u - V.u, v - V.v); }
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(u * f, v * f); }
};

template<class t>struct Vec3
{
	union {
		struct { t x, y, z; };
		struct { t ivert, iuv, inorm; };
		t raw[3];
	};
	Vec3() :x(0), y(0), z(0) {}
	Vec3(t _x, t _y, t _z) :x(_x), y(_y), z(_z) {}
	//叉积
	inline Vec3<t> operator ^(const Vec3<t>& v) const { return Vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
	inline Vec3<t> operator +(const Vec3<t> & v) const { return Vec3<t>(x + v.x, y + v.y, z + v.z); }
	inline Vec3<t> operator -(const Vec3<t> & v) const { return Vec3<t>(x - v.x, y - v.y, z - v.z); }
	inline Vec3<t> operator *(float f)          const { return Vec3<t>(x * f, y * f, z * f); }
	friend Vec3<t> operator *(float f, const Vec3<t>& v)//定义float左乘向量
	{
		return v * f;
	}
	inline Vec3<t> operator /(float f)			const { return Vec3<t>(x / f, y / f, z / f); }
	inline Vec3<t>& operator -() { x = -x; y = -y; z = -z; return *this; }
	
	//数组重载下
	inline t operator[](int index)const { return index == 0 ? x : index == 1 ? y : z; }
	inline t& operator[](int index) { return index == 0 ? x : index == 1 ? y : z; }

	//点积
	inline t       operator *(const Vec3<t> & v) const { return x * v.x + y * v.y + z * v.z; }
	//长度
	float norm() const { return std::sqrt(x * x + y * y + z * z); }
	//归一化向量
	Vec3<t>& normalize() { *this = (*this) / norm(); return *this; }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
};

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v) 
{
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}
template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) 
{
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}
#endif // !GEOMETRY_H
