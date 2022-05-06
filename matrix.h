#ifndef MATRIX_H
#define MATRIX_H

#include "geometry.h"





class matrix2x2
{
public:
	matrix2x2(float n1, float n2, float n3, float n4) :a1(n1), a2(n2), a3(n3), a4(n4) {}
	matrix2x2 GetInverseMat();
	matrix2x2 operator*(float t) { return matrix2x2(a1 * t, a2 * t, a3 * t, a4 * t); }

public:
	// a1,a3
	// a2,a4
	float a1, a2, a3, a4;//按列排序
};

template<class t>class matrix3x3
{
public:
	matrix3x3(t a, t b, t c, t d, t e, t f, t g, t h, t i)
		:a1(a), a2(b), a3(c), b1(d), b2(e), b3(f), c1(g), c2(h), c3(i) {}
	matrix3x3(Vec3<t> column1, Vec3<t> column2, Vec3<t> column3)
		:a1(column1.x), a2(column1.y), a3(column1.z), b1(column2.x), b2(column2.y), b3(column2.z), c1(column3.x), c2(column3.y), c3(column3.z) {}
	matrix3x3<t> operator*(float f) const
	{
		return matrix3x3(a1 * f, a2 * f, a3 * f, b1 * f, b2 * f, b3 * f, c1 * f, c2 * f, c3 * f);
	}
	//matrix3x3<t> GetInverseMat() const;

public:
	//a1,b1,c1
	//a2,b2,c2
	//a3,b3,c3
	t a1, a2, a3, b1, b2, b3, c1, c2, c3;//按列排序
};

class Matrix4x1
{
public:
	float mat[4];

public:
	Matrix4x1(float a, float b, float c, float d)
	{
		mat[0] = a; mat[1] = b; mat[2] = c; mat[3] = d;
	}

	Matrix4x1(Vec3f v, float pointOrVector = 1)
	{
		mat[0] = v[0]; mat[1] = v[1]; mat[2] = v[2]; mat[3] = pointOrVector;
	}

	Vec3f ToVec3()
	{
		return Vec3f(mat[0] / mat[3], mat[1] / mat[3], mat[2] / mat[3]);
	}
};


//定义真正通用的矩阵类
template<typename T> class Matrix
{
public:
	T** matrix;
	int row;//因为要兼顾4x1矩阵，所以行和列有可能不同
	int col;

public:
	Matrix(const int r, const int c);
	~Matrix();

	void Identity();
	T* operator[](const int index);
	T* operator[](const int index)const;

	Matrix<T> operator*(const Matrix<T>& matrixB);
	Matrix<T> operator=(const Matrix<T>& matrixB);

	//用于调试输出的
	template<typename T> friend std::ostream& operator<<(std::ostream& out, const Matrix<T>& m);
};
template<typename T> Matrix<T>::Matrix(const int r, const int c)
{
	row = r;
	col = c;
	matrix = new T* [row];
	for (int i = 0; i < r; i++)
	{
		matrix[i] = new T[col];
	}
}

template<typename T> void Matrix<T>::Identity()
{
	//已经执行过构造函数的对象，再执行Identity方法
	if (row != col)
		return;
	for (int i = 0; i < row; i++)
		for (int j = 0; j < col; j++)
			matrix[i][j] = i == j ? 1 : 0;
}

template<typename T> Matrix<T>::~Matrix()
{
	if (matrix)
	{
		for (int i = 0; i < row; i++)
		{
			delete[] matrix[i];//删除matrix[i]对应的一维数组
		}
		delete[] matrix;//删除外层数组
	}
}

template<typename T> T* Matrix<T>::operator[](const int index)
{
	//return matrix[index];
	return this->matrix[index];
}
template<typename T> T* Matrix<T>::operator[](const int index)const
{
	//return matrix[index];
	return this->matrix[index];
}

template<typename T> Matrix<T> Matrix<T>::operator*(const Matrix<T>& matrixB)
{
	Matrix<T> errorMat(0, 0);
	if (col != matrixB.row)
		return errorMat;

	T trans;
	Matrix<T> result(col, matrixB.row);
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < matrixB.col; j++)
		{
			trans = 0;
			for (int k = 0; k < col; k++)//这个col等于matrixB的row
			{
				trans += matrix[i][k] * matrixB.matrix[k][j];
			}
			result[i][j] = trans;
		}
	}
	return result;
}//由于trans的生命周期到这就结束了，所以result的matrix指针指向丢失量

template<typename T> Matrix<T> Matrix<T>::operator=(const Matrix<T>& matrixB)
{
	Matrix<T> error(0, 0);
	if (this->row != matrixB.row || this->col != matrixB.col)
		return error;

	//Matrix<T> result(this->row, this->col);
	for (int i = 0; i < this->row; i++)
		for (int j = 0; j < this->col; j++)
			matrix[i][j] = matrixB[i][j];
}


//输出输入friend std::cout
template<typename T> std::ostream& operator<<(std::ostream& out, const Matrix<T>& m)
{
	for (int i = 0; i < m.row; i++)
	{
		for (int j = 0; j < m.col; j++)
		{
			out << m[i][j] << ' ';
		}
		out << '\n';
	}
	return out;
}

class Matrix4x4
{
public:
	//float** mat;
	float mat[4][4];//第一个[4]可以分开看，每一个对应下面的一个一维数组[4],共有4条线

public:
	Matrix4x4(float m11, float m12, float m13, float m14,
		float m21, float m22, float m23, float m24,
		float m31, float m32, float m33, float m34,
		float m41, float m42, float m43, float m44)
	{
		//因为数组声明之后，不能在用{m11,m12}这种方式赋值，要一个一个赋值
		mat[0][0] = m11; mat[0][1] = m12; mat[0][2] = m13; mat[0][3] = m14;
		mat[1][0] = m21; mat[1][1] = m22; mat[1][2] = m23; mat[1][3] = m24;
		mat[2][0] = m31; mat[2][1] = m32; mat[2][2] = m33; mat[2][3] = m34;
		mat[3][0] = m41; mat[3][1] = m42; mat[3][2] = m43; mat[3][3] = m44;
	}

	static Matrix4x4 identity();

	float* operator[](const int index)
	{
		return mat[index];
	}

	Matrix4x4 operator*(const Matrix4x4& B)const
	{
		Matrix4x4 resultMat = Matrix4x4::identity();
		float trans;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				trans = 0;
				for (int k = 0; k < 4; k++)
				{
					trans += this->mat[i][k] * B.mat[k][j];
				}
				resultMat[i][j] = trans;
			}
		}
		return resultMat;
	}

	Matrix4x1 operator*(const Matrix4x1& B)const
	{
		Matrix4x1 resultMat = Matrix4x1(0, 0, 0, 1);
		float trans;
		for (int i = 0; i < 4; i++)
		{
			trans = 0;
			for (int k = 0; k < 4; k++)
			{
				trans += this->mat[0][k] * B.mat[k];
			}
			resultMat.mat[i] = trans;
		}
		return resultMat;
	}
};

Matrix4x4 Matrix4x4::identity()
{
	return Matrix4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

/// <summary>
/// 2x2矩阵的逆
/// </summary>
/// <returns></returns>
matrix2x2 matrix2x2::GetInverseMat()
{
	float outMul = 1 / (a1 * a4 - a3 * a2);
	matrix2x2 inMat = matrix2x2(a4, -a2, -a3, a1);
	return inMat * outMul;
}

//matrix3x3<t> matrix3x3::GetInverseMat() const
//{
//
//}

#endif