#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "matrix.h"


const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

Model* model = NULL;
const int width = 600;
const int height = 600;

Vec3f light_dir(0, 0, 1); //右手坐标系，表示在该点为起点的光照，非来自方向
Vec3f cameraPos(0, 0, 0);
Vec3f lookAtPos(0, 0, 0);

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
	//比如(80,200)和(70,100)这段特殊点
	//特殊1：x方向上是减小的而不是增大的
	//特殊2：Y幅度更大，应该按照Y轴每步走
	bool yStep = false;
	if (abs(x0 - x1) < abs(y0 - y1))
	{
		yStep = true;
	}
	if (yStep)
	{
		if (y0 > y1)//y1小，从y1到y0
		{
			//交换两个点，保持小的在左侧
			std::swap(x0, x1);
			std::swap(y0, y1);
		}
		for (int y = y0; y <= y1; y++)
		{
			float t = (y - y0) / (float)(y1 - y0);
			int x = x0 + t * (x1 - x0);
			image.set(x, y, color);
		}
	}
	else
	{
		if (x0 > x1)
		{
			std::swap(x0, x1);
			std::swap(y0, y1);
		}
		for (int x = x0; x < x1; x++)
		{
			float t = (x - x0) / (float)(x1 - x0);
			int y = y0 + t * (y1 - y0);
			image.set(x, y, color);
		}
	}
}

//返回 点p在三角形trianglePtr中的重心坐标
Vec3f barycentric(Vec3f* trianglePtr, Vec3f P)
{
	//重心坐标还是不对
	//Vec3f A = trianglePtr[0];
	//Vec3f B = trianglePtr[1];
	//Vec3f C = trianglePtr[2];
	////求AB为一列，AC为一列构成的矩阵，他的逆矩阵
	////matrix2x2()
	//Vec3f v0 = C - A;
	//Vec3f v1 = B - A;
	//Vec3f v2 = P - A;
	//float fenmu = (v0 * v0) * (v1 * v1) - (v0 * v1) * (v1 * v0);//下面三行是搬运的，没有自己在本子上画
	//float u = ((v1 * v1) * (v2 * v0) - (v1 * v0) * (v2 * v1)) / fenmu;
	//float v = ((v0 * v0) * (v2 * v1) - (v0 * v1) * (v2 * v0)) / fenmu;
	//float s = 1 - u - v;
	//if (s < 0 || u < 0 || v < 0)
	//	return Vec3f(-1, -1, -1);
	//return Vec3f(s, u, v);



	Vec3f A = trianglePtr[0];
	Vec3f B = trianglePtr[1];
	Vec3f C = trianglePtr[2];

	Vec3f s[2];
	s[0] = Vec3f(C.x - A.x, B.x - A.x, A.x - P.x);
	s[1] = Vec3f(C.y - A.y, B.y - A.y, A.y - P.y);
	//for (int i = 2; i--; ) 
	//{
	//    s[i].x = C[i] - A[i];
	//    s[i].y = B[i] - A[i];
	//    s[i].z = A[i] - P[i];
	//}
	Vec3f u = s[0] ^ s[1];
	if (std::abs(u.z) > 1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
		return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);//因为u的结果是(u,v,1)
	return Vec3f(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizator




	//Vec3f A = trianglePtr[0];
	//Vec3f B = trianglePtr[1];
	//Vec3f C = trianglePtr[2];

	//Vec3f crossABC = (B - A) ^ (C - A); //ABC ABP APC PBC都是按点P在三角形内顺时针顺序组合的
	//Vec3f crossABP = (B - A) ^ (P - A);
	//Vec3f crossAPC = (P - A) ^ (C - A);
	//Vec3f crossPBC = (B - P) ^ (C - P);

	////叉积的方向，如果点P在三角形外，叉积结果就会有正负之分
	////新学到方法，用点积判断方向,同侧点积为正
	//float outsideAB = crossABC * crossABP;//如果点积为负数，表示ABP是逆时针的，P在AB边外侧
	//float outsideAC = crossABC * crossAPC;
	//float outsideBC = crossABC * crossPBC;
	//if (outsideAB < 0 || outsideAC < 0 || outsideBC < 0)
	//	return Vec3f(-1, -1, -1);//满足其中一个，则不在三角形内，不用下面计算了，也可以在下面uvw时候加负号,AB边外则对着w权重为负数

	////叉积的模表示两个向量的所围成的平行四边形面积，用来计算重心坐标
	//float areaABC = crossABC.norm() * 0.5; //如果点P在三角形外，叉积结果就会有正负之分
	//float areaABP = crossABP.norm() * 0.5;
	//float areaAPC = crossAPC.norm() * 0.5;
	//float areaPBC = crossPBC.norm() * 0.5;


	//float u = areaPBC / areaABC;
	//float v = areaAPC / areaABC;
	//float w = areaABP / areaABC;
	//return Vec3f(u, v, w);
}

//传入的是屏幕空间坐标,构建三角形(trianglePtr)的包围盒。逐一判断包围盒里面每个像素，是否在三角形内（重心坐标判断法）
void triangle(Vec3f* trianglePtr, Vec2f* triangleUVPtr, Vec3f* normalPtr, float* zBuffer, TGAImage& image, TGAColor color)
{
	Vec3f A = trianglePtr[0];
	Vec3f B = trianglePtr[1];
	Vec3f C = trianglePtr[2];

	Vec2i rectMin = Vec2i(width - 1, height - 1);
	Vec2i rectMax = Vec2i(0, 0);
	for (int i = 0; i < 3; i++)//三角形的3个点依次
	{
		rectMin.x = (int)(trianglePtr[i].x < rectMin.x ? trianglePtr[i].x : rectMin.x);
		rectMin.y = (int)(trianglePtr[i].y < rectMin.y ? trianglePtr[i].y : rectMin.y);
		rectMax.x = (int)(trianglePtr[i].x > rectMax.x ? trianglePtr[i].x : rectMax.x);
		rectMax.y = (int)(trianglePtr[i].y > rectMax.y ? trianglePtr[i].y : rectMax.y);
	}

	Vec3f curPixel;//设成int传参的,AABB内部填充的
	Vec3f barCoord;//这个像素在三个点的重心坐标
	for (int i = (int)rectMin.x; i <= rectMax.x; i++)
	{
		for (int j = (int)rectMin.y; j <= rectMax.y; j++)
		{
			curPixel = Vec3f(i, j, 1);
			barCoord = barycentric(trianglePtr, curPixel);//盒子内该像素在该三角形的重心坐标，判断像素是否在三角形内
			if (barCoord.x < 0 || barCoord.y < 0 || barCoord.z < 0)
				continue;//AABB盒子里这个像素的重心坐标小于0，说明这个像素在三角形外，不画

			//这个像素在三角形内，再判断这个像素的深度测试
			//先通过重心坐标找到这个像素的z深度值
			float z = A.z * barCoord.x + B.z * barCoord.y + C.z * barCoord.z;//三角形三个点的深度，按重心坐标取得该像素点的深度
			if (z > zBuffer[i * width + j])//右手坐标系
			{
				zBuffer[i * width + j] = z;
				//这个像素的uv值，通过三个点的重心坐标分别乘以三个点的uv值得到
				Vec2f uv = triangleUVPtr[0] * barCoord.x + triangleUVPtr[1] * barCoord.y + triangleUVPtr[2] * barCoord.z;
				Vec3f normal = normalPtr[0] * barCoord.x + normalPtr[1] * barCoord.y + normalPtr[2] * barCoord.z;
				normal = (normal + Vec3f(1.0, 1.0, 1.0)) / 2;
				Vec3f lightDir = light_dir.normalize();
				float lambert = normal * lightDir * 255;
				if (lambert < 0)
					lambert = 0;
				TGAColor diffuseColor = model->SamplerDiffseColor(uv);
				//image.set(i, j, diffuseColor);
				image.set(i, j, TGAColor(lambert, lambert, lambert, 1));//注释掉上面这行，使用lambert传来的color来算整个面的color
			}

			//image.set(i, j, TGAColor(barCoord.x * 255, barCoord.y * 255, barCoord.z * 255, 1));//输出每个像素的重心坐标
		}
	}
}

//.obj文件中定义的顶点坐标[-1,1]，转换到屏幕坐标[0,width]和[0,height]，z值不变，仍然是[-1,1]区间
Vec3f World2Screen(Vec3f worldPos)
{
	return Vec3f((worldPos.x + 1.0) * 0.5 * width, (worldPos.y + 1.0) * 0.5 * height, worldPos.z);
}

//把摄像机在世界坐标系下表示的轴uvw，按行排列，就是World->view。按列排列，就是view->World
//按列排列时候，第四列可以是view在世界坐标轴表示下的原点坐标，但反过来按行则不行，因为原点坐标不是正交性，求逆不等于求转置，3x3的旋转轴因为正交性所以可以
//按行排列，也可以这样理解：view的每一个轴u、v、w和世界坐标做点积，相当于这个世界坐标分别在u、v、w单位向量上的投影，当然就变到的viewSpace
//因为平移没有正交性，需要两个旋转和平移两个矩阵，对于一个物体来说，先反方向施加平移矩阵，再做旋转矩阵，即矩阵是V * T * 物体坐标
Matrix4x4 World2View(Vec3f cameraPos,Vec3f lookAtPos,Vec3f upDir)
{
	Vec3f z = (cameraPos - lookAtPos).normalize();
	Vec3f x = (upDir ^ z).normalize();
	Vec3f y = (z ^ x).normalize();
	Matrix4x4 viewMat = Matrix4x4::identity();//上面注释中的V，即旋转的3x3矩阵
	for (int j = 0; j < 3; j++)
	{
		viewMat[0][j] = x[j];//按行排列
		viewMat[1][j] = y[j];
		viewMat[2][j] = z[j];
	}

	Matrix4x4 translationMat = Matrix4x4::identity();
	for (int i = 0; i < 3; i++)
	{
		translationMat[i][3] = -cameraPos[i];//平移矩阵：第四列为负方向
	}

	return viewMat * translationMat;//先平移再旋转
}
#include <iostream>
int main(int argc, char** argv)
{
	const int aa = 4;
	Matrix<float> m(4, 4);	//Matrix<float>* m = new Matrix<float>(4, 4);
	m.Identity();
	std::cout << m.matrix[0][0] << std::endl;//matrix要变成private变量了
	m[0][0] = 3;
	std::cout << m[0][0] << std::endl;//第一个[]是Matrix类的重载[]，得到的是T*，是指针。第二个[]因为是T*指针，而数组名也可以是T*

	//Matrix4x4 m = Matrix4x4::identity();
	//Matrix4x4 m1 = Matrix4x4::identity();
	//m[1][1] = 3.6;
	//Matrix4x4 result = m * m1;
	//std::cout << result[1][1] << std::endl;
	return 1;

	if (2 == argc)
		model = new Model(argv[1]);
	else
		model = new Model("obj/african_head.obj");

	//通过画线可知，这是右手坐标系，从左下开始的（其实从左上开始，被下面flip_vertically改成了左下）
	TGAImage image(width, height, TGAImage::RGB); //纯黑的100 * 100图
	
	float* zBuffer = new float[width * height];
	for (int i = 0; i < width * height; i++)
		zBuffer[i] = -std::numeric_limits<float>::max();

	Matrix4x4 world2View = World2View(cameraPos, lookAtPos, Vec3f(0, 1, 0));

	for (int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);//face是含有三个元素的，三个点形成的面，即.obj文件中的一行
		//v0 v1 v2是三个[-1,1]的坐标
		Vec3f v0 = model->vert(face[0]);
		Vec2f uv0 = model->GetUV(face[1]);
		Vec3f normal0 = model->GetNormal(face[2]);

		Vec3f v1 = model->vert(face[3]);
		Vec2f uv1 = model->GetUV(face[4]);
		Vec3f normal1 = model->GetNormal(face[5]);

		Vec3f v2 = model->vert(face[6]);
		Vec2f uv2 = model->GetUV(face[7]);
		Vec3f normal2 = model->GetNormal(face[8]);

		v0 = (world2View * Matrix4x1(v0)).ToVec3();
		v1 = (world2View * Matrix4x1(v1)).ToVec3();
		v2 = (world2View * Matrix4x1(v2)).ToVec3();

		//转换到[0,width] [0,height]屏幕坐标
		Vec3f v0screenCoord = World2Screen(v0);
		Vec3f v1screenCoord = World2Screen(v1);
		Vec3f v2screenCoord = World2Screen(v2);
		Vec3f screenTriangle[3] = { v0screenCoord,v1screenCoord,v2screenCoord };

		//用面法线代替顶点法线插值到像素，画每个三角形的颜色
		Vec3f faceNormal = (v1 - v0) ^ (v2 - v0);//两个向量的叉积，既是垂直于他们的法向量
		//注意：faceNormal如果叉积左右换一下，算出来的normal方向也会换180度，通过输出重心坐标可以看v0 v1 v2的顺序是顺时针的
		faceNormal.normalize();
		float lambert = faceNormal * light_dir * 255;//整个三角形面是一个颜色

		//获得这个三角形三个顶点的uv值
		Vec2f triangleUV[3] = {uv0,uv1,uv2};

		Vec3f triangleNormals[3] = { normal0,normal1,normal2 };//到像素里面通过重心坐标插值算每个像素的normal，算lambert

		triangle(screenTriangle, triangleUV, triangleNormals,
			zBuffer, image, TGAColor(lambert, lambert, lambert, 1));//加了zBuffer检测，lambert>0的条件可以去掉了
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}