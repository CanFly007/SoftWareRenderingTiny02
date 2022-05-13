#include "maths.h"

vec3::vec3() :e{ 0,0,0 } {}
vec3::vec3(float e0, float e1, float e2) : e{ e0,e1,e2 } {}
float vec3::x() const { return e[0]; }
float vec3::y() const { return e[1]; }
float vec3::z() const { return e[2]; }

float& vec3::operator[](int i) { return e[i]; }
float vec3::operator[](int i)const { return e[i]; }

vec3 vec3::operator-()const { return vec3(-e[0], -e[1], -e[2]); }
vec3& vec3::operator+=(const vec3& v)
{
	e[0] += v[0];
	e[1] += v[1];
	e[2] += v[2];
	return *this;
}
vec3& vec3::operator*=(const float t)
{
	e[0] *= t;
	e[1] *= t;
	e[2] *= t;
	return *this;
}
vec3& vec3::operator/=(const float t)
{
	e[0] /= t;
	e[1] /= t;
	e[2] /= t;
	return *this;
}
float vec3::norm_squared() const
{
	return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
}
float vec3::norm() const
{
	return sqrt(this->norm_squared());
}
std::ostream& operator<<(std::ostream& out, const vec3& v)
{
	return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}
float vec3::operator*(const vec3& v)const
{
	return e[0] * v.e[0] + e[1] * v.e[1] + e[2] * v.e[2];
}
vec3 vec3::operator^(const vec3& v)const
{
	return vec3(e[1] * v.e[2] - e[2] * v.e[1],
		e[2] * v.e[0] - e[0] * v.e[2],
		e[0] * v.e[1] - e[1] * v.e[0]);
}
vec3& vec3::normalize()//返回引用类型vec3&
{
	float t = norm();
	(*this) /= t;//this是指针 *this是取指针的指，即这个vec3对象
	return *this;
}

//------------------------------------------------------------------

mat3::mat3() {}
vec3& mat3::operator[](int i) { return rows[i]; }
vec3 mat3::operator[](int i)const { return rows[i]; }
mat3 mat3::operator/(float t)const
{
	mat3 m;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			m[i][j] = (*this)[i][j] / t;
	return m;
}

mat3 mat3::identity()
{
	mat3 m;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			m[i][j] = (i == j) ? 1 : 0;
	return m;
}

mat3 mat3::transpose()const
{
	mat3 transpose;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			transpose[i][j] = this->rows[j][i];
	return transpose;
}

static float mat3_determinant(const mat3& m)//返回m的行列式值
{
	//行列式的值 等于 任一行（列）的各个元素与它对应的代数余子式乘积之和
	float A00 = m[1][1] * m[2][2] - m[1][2] * m[2][1];//代数余子式A00
	float A01 = (-1) * (m[1][0] * m[2][2] - m[1][2] * m[2][0]);
	float A02 = m[1][0] * m[2][1] - m[1][1] * m[2][0];
	//代数余子式乘以对应各元素乘积之和：
	return (m[0][0] * A00 + m[0][1] * A01 + m[0][2] * A02);
}

static mat3 mat3_adjoint(const mat3 & m)//返回m的伴随矩阵
{
	//行列式 各个元素的代数余子式Aij所构成的如印象笔记的矩阵（按列排序），称为m的伴随矩阵
	//先按行排序，最后做一次转置操作即可
	mat3 adjoint;
	float A00 = m[1][1] * m[2][2] - m[1][2] * m[2][1];
	float A01 = (-1) * (m[1][0] * m[2][2] - m[1][2] * m[2][0]);
	float A02 = m[1][0] * m[2][1] - m[1][1] * m[2][0];
	float A10 = (-1) * (m[0][1] * m[2][2] - m[0][2] * m[2][1]);
	float A11 = m[0][0] * m[2][2] - m[0][2] * m[2][0];
	float A12 = (-1) * (m[0][0] * m[2][1] - m[0][1] * m[2][0]);
	float A20 = m[0][1] * m[1][2] - m[0][2] * m[1][1];
	float A21 = (-1) * (m[0][0] * m[1][2] - m[0][2] * m[1][0]);
	float A22 = m[0][0] * m[1][1] - m[0][1] * m[1][0];
	adjoint[0][0] = A00;
	adjoint[0][1] = A01;
	adjoint[0][2] = A02;
	adjoint[1][0] = A10;
	adjoint[1][1] = A11;
	adjoint[1][2] = A12;
	adjoint[2][0] = A20;
	adjoint[2][1] = A21;
	adjoint[2][2] = A22;
	
	adjoint = adjoint.transpose();//按列排序才是伴随矩阵，所以转置一下
	return adjoint;
}

//逆矩阵 = 伴随矩阵 除以 矩阵行列式的值
mat3 mat3::inverse()const
{
	float determinant = mat3_determinant(*this);
	mat3 adjoint = mat3_adjoint(*this);
	mat3 inverse = adjoint / determinant;
	return inverse;
}

std::ostream& operator<<(std::ostream& out, const mat3& m)
{
	return out << m[0] << "\n" << m[1] << "\n" << m[2];
}

//---------------------------------------------------------------

vec4::vec4() :e{ 0,0,0,0 } {}
vec4::vec4(float e0, float e1, float e2, float e3) : e{ e0,e1,e2,e3 } {}
float vec4::x() const { return e[0]; }
float vec4::y() const { return e[1]; }
float vec4::z() const { return e[2]; }
float vec4::w() const { return e[3]; }

float& vec4::operator[](int i) { return e[i]; }
float vec4::operator[](int i)const { return e[i]; }
vec4& vec4::operator*=(const float t) { 
	e[0] *= t; e[1] *= t; e[2] *= t; e[3] *= t;
	return *this;
}
vec4& vec4::operator/=(const float t) { return *this *= (1 / t); }
vec4 vec4::operator/(const float t) const {
	return vec4(e[0] / t, e[1] / t, e[2] / t, e[3] / t);
}
std::ostream& operator<<(std::ostream& out, const vec4& v)
{
	return out << v[0] << "  " << v[1] << "  " << v[2] << "  " << v[3];
}

//--------------------------------------------------------------

mat4::mat4() {}
vec4& mat4::operator[](int i) { return rows[i]; }
vec4 mat4::operator[](int i) const { return rows[i]; }
mat4 mat4::operator/(float t) const{
	mat4 m;
	for (int i = 0; i < 4; i++)
		m[i] = rows[i] / t;
	return m;
}
mat4 mat4::operator*(const mat4 B) const {
	mat4 m;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
				m[i][j] += (*this)[i][k] * B[k][j];
	return m;
}
vec4 mat4::operator*(const vec4 V) const
{
	vec4 result;
	for (int i = 0; i < 4; i++)
		for (int k = 0; k < 4; k++)
			result[i] += (*this)[i][k] * V[k];
	return result;
}
mat4 mat4::identity()
{
	mat4 m;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			m[i][j] = (i == j) ? 1 : 0;
	return m;
}
mat4 mat4::transpose() const
{
	mat4 m;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			m[i][j] = (*this)[j][i];
	return m;
}

//返回A_r_c代数余子式的值
//4x4矩阵某一行某一列拿掉后的3x3行列式，即余子式的值
//余子式的值*正负就是代数余子式的值
//代数余子式，求行列式需要，求伴随矩阵也需要
static float mat4_minor(int r, int c, const mat4& mat4x4)
{
	mat3 minor;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int row = i < r ? i : i + 1;
			int col = j < c ? j : j + 1;//算在mat4x4的什么位置取值
			minor[i][j] = mat4x4[row][col];
		}
	}
	//minor是余子式，determinant是求该余子式的值
	float determinant = mat3_determinant(minor);
	//代数余子式的值
	int sign = (r + c) % 2 == 0 ? 1 : -1;
	float algebraicComplement = sign * determinant;
	return algebraicComplement;
}

//mat4行列式的值
//行列式的值，等于任意一行（或一列）每个元素乘以其对应的代数余子式的值加起来求和，就是整个行列式的值
static float mat4_determinant(const mat4& m)
{
	float determinant = 0;
	//按第0行来
	for (int j = 0; j < 4; j++)
	{
		determinant += m[0][j] * mat4_minor(0, j, m);
	}
	return determinant;
}
//返回矩阵m的伴随矩阵
static mat4 mat4_adjoint(const mat4& m)
{
	mat4 adjoint;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			float algebraicComplement = mat4_minor(i, j, m);//i行j列的代数余子式值
			adjoint[i][j] = algebraicComplement;//先正常，下面转置
		}
	}
	adjoint = adjoint.transpose();
	return adjoint;
}

mat4 mat4::inverse() const//可能有正负0之分
{
	mat4 inverse;
	mat4 adjoint = mat4_adjoint(*this);
	float determinant = mat4_determinant(*this);
	inverse = adjoint / determinant;
	return inverse;
}

std::ostream& operator<<(std::ostream& out, const mat4& m)
{
	return out << m[0] << "\n" << m[1] << "\n" << m[2] << "\n" << m[3];
}
