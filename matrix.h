//#ifndef MATRIX_H
//#define MATRIX_H
//
//#include "geometry.h"
//
//
//
//
//
//class matrix2x2
//{
//public:
//	matrix2x2(float n1, float n2, float n3, float n4) :a1(n1), a2(n2), a3(n3), a4(n4) {}
//	matrix2x2 GetInverseMat();
//	matrix2x2 operator*(float t) { return matrix2x2(a1 * t, a2 * t, a3 * t, a4 * t); }
//
//public:
//	// a1,a3
//	// a2,a4
//	float a1, a2, a3, a4;//按列排序
//};
//
//template<class t>class matrix3x3
//{
//public:
//	matrix3x3(t a, t b, t c, t d, t e, t f, t g, t h, t i)
//		:a1(a), a2(b), a3(c), b1(d), b2(e), b3(f), c1(g), c2(h), c3(i) {}
//	matrix3x3(Vec3<t> column1, Vec3<t> column2, Vec3<t> column3)
//		:a1(column1.x), a2(column1.y), a3(column1.z), b1(column2.x), b2(column2.y), b3(column2.z), c1(column3.x), c2(column3.y), c3(column3.z) {}
//	matrix3x3<t> operator*(float f) const
//	{
//		return matrix3x3(a1 * f, a2 * f, a3 * f, b1 * f, b2 * f, b3 * f, c1 * f, c2 * f, c3 * f);
//	}
//	//matrix3x3<t> GetInverseMat() const;
//
//public:
//	//a1,b1,c1
//	//a2,b2,c2
//	//a3,b3,c3
//	t a1, a2, a3, b1, b2, b3, c1, c2, c3;//按列排序
//};
//
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

//
////定义真正通用的矩阵类
////template<typename T> 
//class Matrix
//{
//public:
//	//T** matrix;
//	float** matrix;
//	int row;//因为要兼顾4x1矩阵，所以行和列有可能不同
//	int col;
//
//public:
//	Matrix(const int r, const int c);
//	~Matrix();
//
//	void Identity();
//	//T* operator[](const int index);
//	float* operator[](const int index);
//	float* operator[](const int index)const;
//
//	Matrix operator*(const Matrix& matrixB);
//	Matrix operator=(const Matrix& matrixB);
//
//	//用于调试输出的
//	friend std::ostream& operator<<(std::ostream& out, const Matrix& m);
//};
////template<typename T> 
//Matrix::Matrix(const int r, const int c)
//{
//	row = r;
//	col = c;
//	matrix = new float* [row];
//	for (int i = 0; i < r; i++)
//	{
//		matrix[i] = new float[col];
//	}
//	//赋值为0
//	for (int i = 0; i < r; i++)
//		for (int j = 0; j < c; j++)
//			matrix[i][j] = 0;
//}
//
////template<typename T> 
//void Matrix::Identity()
//{
//	//已经执行过构造函数的对象，再执行Identity方法
//	if (row != col)
//		return;
//	for (int i = 0; i < row; i++)
//		for (int j = 0; j < col; j++)
//			matrix[i][j] = i == j ? 1 : 0;
//}
//
////template<typename T> 
//Matrix::~Matrix()
//{
//	if (matrix)
//	{
//		for (int i = 0; i < row; i++)
//		{
//			delete[] matrix[i];//删除matrix[i]对应的一维数组
//		}
//		delete[] matrix;//删除外层数组
//	}
//}
//
////template<typename T> 
//float* Matrix::operator[](const int index)
//{
//	//return matrix[index];
//	return this->matrix[index];
//}
////template<typename T>
//float* Matrix::operator[](const int index)const
//{
//	//return matrix[index];
//	return this->matrix[index];
//}
//
////template<typename T> 
//Matrix Matrix::operator*(const Matrix& matrixB)
//{
//	//Matrix errorMat(0, 0);
//	//if (col != matrixB.row)
//	//	return errorMat;
//
//	//float trans;
//	//Matrix result(col, matrixB.row);
//	//for (int i = 0; i < row; i++)
//	//{
//	//	for (int j = 0; j < matrixB.col; j++)
//	//	{
//	//		result[i][j] = 0;
//	//		for (int k = 0; k < col; k++)//这个col等于matrixB的row
//	//		{
//	//			result.matrix[i][j] += matrix[i][k] * matrixB.matrix[k][j];
//	//		}
//	//		//result.matrix[i][j] = trans;
//	//	}
//	//}
//	//return result;
//
//
//	Matrix ba_M(col, matrixB.row);
//	for (int i = 0; i < row; i++) {
//		for (int j = 0; j < matrixB.col; j++) {
//			for (int k = 0; k < col; k++) {
//				ba_M.matrix[i][j] += (matrix[i][k] * matrixB.matrix[k][j]);
//			}
//		}
//	}
//	return ba_M;
//
//}//由于trans的生命周期到这就结束了，所以result的matrix指针指向丢失量
//
////Matrix Matrix::operator*(const Matrix& m)const {
////	Matrix ba_M(rows_num, m.cols_num, 0.0);
////	for (int i = 0; i < rows_num; i++) {
////		for (int j = 0; j < m.cols_num; j++) {
////			for (int k = 0; k < cols_num; k++) {
////				ba_M.p[i][j] += (p[i][k] * m.p[k][j]);
////			}
////		}
////	}
////	return ba_M;
////}
//
//
////template<typename T> 
//Matrix Matrix::operator=(const Matrix& matrixB)
//{
//	Matrix error(0, 0);
//	if (this->row != matrixB.row || this->col != matrixB.col)
//		return error;
//
//	//Matrix<T> result(this->row, this->col);
//	for (int i = 0; i < this->row; i++)
//		for (int j = 0; j < this->col; j++)
//			matrix[i][j] = matrixB[i][j];
//}
//
//
////输出输入friend std::cout
////template<typename T> 
//std::ostream& operator<<(std::ostream& out, const Matrix& m)
//{
//	for (int i = 0; i < m.row; i++)
//	{
//		for (int j = 0; j < m.col; j++)
//		{
//			out << m[i][j] << ' ';
//		}
//		out << '\n';
//	}
//	return out;
//}
//
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
//
///// <summary>
///// 2x2矩阵的逆
///// </summary>
///// <returns></returns>
//matrix2x2 matrix2x2::GetInverseMat()
//{
//	float outMul = 1 / (a1 * a4 - a3 * a2);
//	matrix2x2 inMat = matrix2x2(a4, -a2, -a3, a1);
//	return inMat * outMul;
//}
//
////matrix3x3<t> matrix3x3::GetInverseMat() const
////{
////
////}
//
//#endif






#ifndef __MATRIX_CLL_H__
#define __MATRIX_CCL_H__
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <cmath>
using std::endl;
using std::cout;
using std::istream;
const double EPS = 1e-10;

class Matrix {
private:
	int rows_num, cols_num;
	double** p;
	void initialize();//初始化矩阵

public:
	Matrix(int, int);
	Matrix(int, int, double);//预配分空间
	virtual ~Matrix();//析构函数应当是虚函数，除非此类不用做基类
	Matrix& operator=(const Matrix&);//矩阵的复制
	Matrix& operator=(double*);//将数组的值传给矩阵
	Matrix& operator+=(const Matrix&);//矩阵的+=操作
	Matrix& operator-=(const Matrix&);//-=
	//Matrix& operator*=(const Matrix&);//*=

	//实现*=
	Matrix& operator*=(const Matrix& m)
	{
		Matrix temp(rows_num, m.cols_num);//若C=AB,则矩阵C的行数等于矩阵A的行数，C的列数等于B的列数。
		for (int i = 0; i < temp.rows_num; i++) {
			for (int j = 0; j < temp.cols_num; j++) {
				for (int k = 0; k < cols_num; k++) {
					temp.p[i][j] += (p[i][k] * m.p[k][j]);
				}
			}
		}
		return (*this = temp);

		//*this = temp;
		//return *this;
	}

	//Matrix& operator*=(const Matrix& m)
	//{
	//	Matrix temp(rows_, m.cols_);
	//	for (int i = 0; i < temp.rows_; ++i) {
	//		for (int j = 0; j < temp.cols_; ++j) {
	//			for (int k = 0; k < cols_; ++k) {
	//				temp.p[i][j] += (p[i][k] * m.p[k][j]);
	//			}
	//		}
	//	}
	//	return (*this = temp);
	//}

	//Matrix operator*(const Matrix& m)const;
	//实现矩阵的乘法
	Matrix& operator*(const Matrix& m)const {
		Matrix ba_M(rows_num, m.cols_num, 0.0);
		for (int i = 0; i < rows_num; i++) {
			for (int j = 0; j < m.cols_num; j++) {
				for (int k = 0; k < cols_num; k++) {
					ba_M.p[i][j] += (p[i][k] * m.p[k][j]);
				}
			}
		}
		return ba_M;
	}

	//template<typename T> 
double* operator[](const int index)
{
	//return matrix[index];
	return this->p[index];
}
//template<typename T>
double* operator[](const int index)const
{
	return this->p[index];
}

	static Matrix Solve(const Matrix&, const Matrix&);//求解线性方程组Ax=b
	void Show() const;//矩阵显示
	void swapRows(int, int);
	double det();//求矩阵的行列式
	double Point(int i, int j) const;
	static Matrix inv(Matrix);//求矩阵的逆矩阵
	static Matrix eye(int);//制造一个单位矩阵
	int row() const;
	int col() const;
	static Matrix T(const Matrix& m);//矩阵转置的实现,且不改变矩阵
	Matrix gaussianEliminate();//高斯消元法
	friend std::istream& operator>>(std::istream&, Matrix&);//实现矩阵的输入
};


void Matrix::initialize() {//初始化矩阵大小
	p = new double* [rows_num];//分配rows_num个指针
	for (int i = 0; i < rows_num; ++i) {
		p[i] = new double[cols_num];//为p[i]进行动态内存分配，大小为cols
	}
}
//声明一个全0矩阵
Matrix::Matrix(int rows, int cols)
{
	rows_num = rows;
	cols_num = cols;
	initialize();
	for (int i = 0; i < rows_num; i++) {
		for (int j = 0; j < cols_num; j++) {
			p[i][j] = 0;
		}
	}
}
//声明一个值全部为value的矩阵
Matrix::Matrix(int rows, int cols, double value)
{
	rows_num = rows;
	cols_num = cols;
	initialize();
	for (int i = 0; i < rows_num; i++) {
		for (int j = 0; j < cols_num; j++) {
			p[i][j] = value;
		}
	}
}

//析构函数
Matrix::~Matrix() {
	for (int i = 0; i < rows_num; ++i) {
		delete[] p[i];
	}
	delete[] p;
}
//实现矩阵的复制
Matrix& Matrix::operator=(const Matrix& m)
{
	if (this == &m) {
		return *this;
	}

	if (rows_num != m.rows_num || cols_num != m.cols_num) {
		for (int i = 0; i < rows_num; ++i) {
			delete[] p[i];
		}
		delete[] p;

		rows_num = m.rows_num;
		cols_num = m.cols_num;
		initialize();
	}

	for (int i = 0; i < rows_num; i++) {
		for (int j = 0; j < cols_num; j++) {
			p[i][j] = m.p[i][j];
		}
	}
	return *this;
}
//将数组的值传递给矩阵(要求矩阵的大小已经被声明过了)
Matrix& Matrix::operator=(double* a) {
	for (int i = 0; i < rows_num; i++) {
		for (int j = 0; j < cols_num; j++) {
			p[i][j] = *(a + i * cols_num + j);
		}
	}
	return *this;
}
//+=操作
Matrix& Matrix::operator+=(const Matrix& m)
{
	for (int i = 0; i < rows_num; i++) {
		for (int j = 0; j < cols_num; j++) {
			p[i][j] += m.p[i][j];
		}
	}
	return *this;
}
//实现-=
Matrix& Matrix::operator-=(const Matrix& m)
{
	for (int i = 0; i < rows_num; i++) {
		for (int j = 0; j < cols_num; j++) {
			p[i][j] -= m.p[i][j];
		}
	}
	return *this;
}

//解方程Ax=b
Matrix Matrix::Solve(const Matrix& A, const Matrix& b)
{
	//高斯消去法实现Ax=b的方程求解
	for (int i = 0; i < A.rows_num; i++) {
		if (A.p[i][i] == 0) {

			cout << "请重新输入" << endl;
		}
		for (int j = i + 1; j < A.rows_num; j++) {
			for (int k = i + 1; k < A.cols_num; k++) {
				A.p[j][k] -= A.p[i][k] * (A.p[j][i] / A.p[i][i]);
				if (abs(A.p[j][k]) < EPS)
					A.p[j][k] = 0;
			}
			b.p[j][0] -= b.p[i][0] * (A.p[j][i] / A.p[i][i]);
			if (abs(A.p[j][0]) < EPS)
				A.p[j][0] = 0;
			A.p[j][i] = 0;
		}
	}

	// 反向代换
	Matrix x(b.rows_num, 1);
	x.p[x.rows_num - 1][0] = b.p[x.rows_num - 1][0] / A.p[x.rows_num - 1][x.rows_num - 1];
	if (abs(x.p[x.rows_num - 1][0]) < EPS)
		x.p[x.rows_num - 1][0] = 0;
	for (int i = x.rows_num - 2; i >= 0; i--) {
		double sum = 0;
		for (int j = i + 1; j < x.rows_num; j++) {
			sum += A.p[i][j] * x.p[j][0];
		}
		x.p[i][0] = (b.p[i][0] - sum) / A.p[i][i];
		if (abs(x.p[i][0]) < EPS)
			x.p[i][0] = 0;
	}

	return x;
}

//矩阵显示
void Matrix::Show() const {
	//cout << rows_num <<" "<<cols_num<< endl;//显示矩阵的行数和列数
	for (int i = 0; i < rows_num; i++) {
		for (int j = 0; j < cols_num; j++) {
			cout << p[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}
//实现行变换
void Matrix::swapRows(int a, int b)
{
	a--;
	b--;
	double* temp = p[a];
	p[a] = p[b];
	p[b] = temp;
}
//计算矩阵行列式的值
double Matrix::det() {
	//为计算行列式做一个备份
	double** back_up;
	back_up = new double* [rows_num];
	for (int i = 0; i < rows_num; i++) {
		back_up[i] = new double[cols_num];
	}
	for (int i = 0; i < rows_num; i++) {
		for (int j = 0; j < cols_num; j++) {
			back_up[i][j] = p[i][j];
		}
	}
	if (rows_num != cols_num) {
		std::abort();//只有方阵才能计算行列式，否则调用中断强制停止程序
	}
	double ans = 1;
	for (int i = 0; i < rows_num; i++) {
		//通过行变化的形式，使得矩阵对角线上的主元素不为0
		if (abs(p[i][i]) <= EPS) {
			bool flag = false;
			for (int j = 0; (j < cols_num) && (!flag); j++) {
				//若矩阵的一个对角线上的元素接近于0且能够通过行变换使得矩阵对角线上的元素不为0
				if ((abs(p[i][j]) > EPS) && (abs(p[j][i]) > EPS)) {
					flag = true;
					//注：进行互换后,p[i][j]变为p[j][j]，p[j][i]变为p[i][i]
					//对矩阵进行行变换
					double temp;
					for (int k = 0; k < cols_num; k++) {
						temp = p[i][k];
						p[i][k] = p[j][k];
						p[j][k] = temp;
					}
				}
			}
			if (flag)
				return 0;
		}
	}
	for (int i = 0; i < rows_num; i++) {
		for (int j = i + 1; j < rows_num; j++) {
			for (int k = i + 1; k < cols_num; k++) {
				p[j][k] -= p[i][k] * (p[j][i] * p[i][i]);
			}
		}
	}
	for (int i = 0; i < rows_num; i++) {
		ans *= p[i][i];
	}
	for (int i = 0; i < rows_num; i++) {
		for (int j = 0; j < cols_num; j++) {
			p[i][j] = back_up[i][j];
		}
	}
	return ans;
}
//返回矩阵第i行第j列的数
double Matrix::Point(int i, int j) const {
	return this->p[i][j];
}
//求矩阵的逆矩阵
Matrix Matrix::inv(Matrix A) {
	if (A.rows_num != A.cols_num) {
		std::cout << "只有方阵能求逆矩阵" << std::endl;
		std::abort();//只有方阵能求逆矩阵
	}
	double temp;
	Matrix A_B = Matrix(A.rows_num, A.cols_num);
	A_B = A;//为矩阵A做一个备份
	Matrix B = eye(A.rows_num);
	//将小于EPS的数全部置0
	for (int i = 0; i < A.rows_num; i++) {
		for (int j = 0; j < A.cols_num; j++) {
			if (abs(A.p[i][j]) <= EPS) {
				A.p[i][j] = 0;
			}
		}
	}
	//选择需要互换的两行选主元
	for (int i = 0; i < A.rows_num; i++) {
		if (abs(A.p[i][i]) <= EPS) {
			bool flag = false;
			for (int j = 0; (j < A.rows_num) && (!flag); j++) {
				if ((abs(A.p[i][j]) > EPS) && (abs(A.p[j][i]) > EPS)) {
					flag = true;
					for (int k = 0; k < A.cols_num; k++) {
						temp = A.p[i][k];
						A.p[i][k] = A.p[j][k];
						A.p[j][k] = temp;
						temp = B.p[i][k];
						B.p[i][k] = B.p[j][k];
						B.p[j][k] = temp;
					}
				}
			}
			if (!flag) {
				std::cout << "逆矩阵不存在\n";
				std::abort();
			}
		}
	}
	//通过初等行变换将A变为上三角矩阵
	double temp_rate;
	for (int i = 0; i < A.rows_num; i++) {
		for (int j = i + 1; j < A.rows_num; j++) {
			temp_rate = A.p[j][i] / A.p[i][i];
			for (int k = 0; k < A.cols_num; k++) {
				A.p[j][k] -= A.p[i][k] * temp_rate;
				B.p[j][k] -= B.p[i][k] * temp_rate;
			}
			A.p[j][i] = 0;
		}
	}
	//使对角元素均为1
	for (int i = 0; i < A.rows_num; i++) {
		temp = A.p[i][i];
		for (int j = 0; j < A.cols_num; j++) {
			A.p[i][j] /= temp;
			B.p[i][j] /= temp;
		}
	}
	//std::cout<<"算法可靠性检测，若可靠，输出上三角矩阵"<<std::endl;
	//将已经变为上三角矩阵的A，变为单位矩阵
	for (int i = A.rows_num - 1; i >= 1; i--) {
		for (int j = i - 1; j >= 0; j--) {
			temp = A.p[j][i];
			for (int k = 0; k < A.cols_num; k++) {
				A.p[j][k] -= A.p[i][k] * temp;
				B.p[j][k] -= B.p[i][k] * temp;
			}
		}
	}
	std::cout << "算法可靠性检测，若可靠，输出单位矩阵" << std::endl;
	for (int i = 0; i < A.rows_num; i++) {
		for (int j = 0; j < A.cols_num; j++) {
			printf("%7.4lf\t\t", A.p[i][j]);
		}
		cout << endl;
	}
	A = A_B;//还原A
	return B;//返回该矩阵的逆矩阵
}
//制造一个单位矩阵
Matrix Matrix::eye(int n) {
	Matrix A(n, n);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i == j) {
				A.p[i][j] = 1;
			}
			else {
				A.p[i][j] = 0;
			}
		}
	}
	return A;
}
//读取矩阵行列数
int Matrix::row() const {
	return rows_num;
}
int Matrix::col() const {
	return cols_num;
}
//实现矩阵的转置
Matrix Matrix::T(const Matrix& m)
{
	int col_size = m.col();
	int row_size = m.row();
	Matrix mt(col_size, row_size);
	for (int i = 0; i < row_size; i++) {
		for (int j = 0; j < col_size; j++) {
			mt.p[j][i] = m.p[i][j];
		}
	}
	return mt;
}
//高斯消元法
Matrix Matrix::gaussianEliminate()
{
	Matrix Ab(*this);
	int rows = Ab.rows_num;
	int cols = Ab.cols_num;
	int Acols = cols - 1;

	int i = 0; //跟踪行
	int j = 0; //跟踪列
	while (i < rows)
	{
		bool flag = false;
		while (j < Acols && !flag)
		{
			if (Ab.p[i][j] != 0) {
				flag = true;
			}
			else {
				int max_row = i;
				double max_val = 0;
				for (int k = i + 1; k < rows; ++k)
				{
					double cur_abs = Ab.p[k][j] >= 0 ? Ab.p[k][j] : -1 * Ab.p[k][j];
					if (cur_abs > max_val)
					{
						max_row = k;
						max_val = cur_abs;
					}
				}
				if (max_row != i) {
					Ab.swapRows(max_row, i);
					flag = true;
				}
				else {
					j++;
				}
			}
		}
		if (flag)
		{
			for (int t = i + 1; t < rows; t++) {
				for (int s = j + 1; s < cols; s++) {
					Ab.p[t][s] = Ab.p[t][s] - Ab.p[i][s] * (Ab.p[t][j] / Ab.p[i][j]);
					if (abs(Ab.p[t][s]) < EPS)
						Ab.p[t][s] = 0;
				}
				Ab.p[t][j] = 0;
			}
		}
		i++;
		j++;
	}
	return Ab;
}
//实现矩阵的输入
istream& operator>>(istream& is, Matrix& m)
{
	for (int i = 0; i < m.rows_num; i++) {
		for (int j = 0; j < m.cols_num; j++) {
			is >> m.p[i][j];
		}
	}
	return is;
}



#endif