#ifndef MATRIX_H
#define MATRIX_H

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
					resultMat[i][j] = trans;
				}
			}
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