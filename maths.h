#ifndef MATHS_H
#define MATHS_H

#include <cmath>
#include <iostream>

class vec3{
public:
	vec3();
	vec3(float e0, float e1, float e2);

	float x() const;
	float y() const;
	float z() const;
	float& operator[](int i);
	float operator[](int i) const;

	vec3 operator-() const;
	vec3& operator+=(const vec3& v);
	vec3& operator*=(const float t);
	vec3& operator/=(const float t);
	vec3 operator*(const float t)const;
	vec3 operator-(const float t)const;

	float norm_squared() const;
	float norm() const;

	friend std::ostream& operator<<(std::ostream& out, const vec3& v);
	float operator*(const vec3& v)const;
	vec3 operator^(const vec3& v)const;
	vec3& normalize();

public:
	float e[3];
};

class mat3 {
public:
	mat3();

	vec3& operator[](int i);
	vec3 operator[](int i) const;
	mat3 operator/(float t)const;
	vec3 operator*(vec3 v3)const;

	static mat3 identity();
	mat3 transpose() const;
	mat3 inverse() const;
	//mat3 inverse_transpose() const;

	friend std::ostream& operator<<(std::ostream& out, const mat3& m);


public:
	vec3 rows[3];
};

class vec4 {
public:
	vec4();
	vec4(float e0, float e1, float e2, float e3);

	float x() const;
	float y() const;
	float z() const;
	float w() const;
	float& operator[](int i);
	float operator[](int i) const;

	vec4& operator*=(const float t);
	vec4& operator/=(const float t);
	vec4 operator/(const float t) const;
	friend std::ostream& operator<<(std::ostream& out, const vec4& v);


public:
	float e[4];
};

class mat4 {
public:
	mat4();

	vec4& operator[](int i);
	vec4 operator[](int i) const;
	mat4 operator/(float t) const;
	mat4 operator*(const mat4 B) const;
	vec4 operator*(const vec4 V) const;

	static mat4 identity();
	mat4 transpose() const;
	mat4 inverse() const;
	//mat4 inverse_transpose() const;
	friend std::ostream& operator<<(std::ostream& out, const mat4& m);

public:
	vec4 rows[4];
};
#endif // !MATHS_H
