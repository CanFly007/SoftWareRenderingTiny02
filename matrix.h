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
matrix2x2 matrix2x2::GetInverseMat()
{
	float outMul = 1 / (a1 * a4 - a3 * a2);
	matrix2x2 inMat = matrix2x2(a4, -a2, -a3, a1);
	return inMat * outMul;
}

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

//matrix3x3<t> matrix3x3::GetInverseMat() const
//{
//
//}

#endif