#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

Model* model = NULL;
const int width = 600;
const int height = 600;

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
	Vec3f A = trianglePtr[0];
	Vec3f B = trianglePtr[1];
	Vec3f C = trianglePtr[2];

	Vec3f crossABC = (B - A) ^ (C - A); //ABC ABP APC PBC都是按点P在三角形内顺时针顺序组合的
	Vec3f crossABP = (B - A) ^ (P - A);
	Vec3f crossAPC = (P - A) ^ (C - A);
	Vec3f crossPBC = (B - P) ^ (C - P);

	//叉积的方向，如果点P在三角形外，叉积结果就会有正负之分
	//新学到方法，用点积判断方向,同侧点积为正
	float outsideAB = crossABC * crossABP;//如果点积为负数，表示ABP是逆时针的，P在AB边外侧
	float outsideAC = crossABC * crossAPC;
	float outsideBC = crossABC * crossPBC;
	if (outsideAB < 0 || outsideAC < 0 || outsideBC < 0)
		return Vec3f(-1, -1, -1);//满足其中一个，则不在三角形内，不用下面计算了，也可以在下面uvw时候加负号,AB边外则对着w权重为负数

	//叉积的模表示两个向量的所围成的平行四边形面积，用来计算重心坐标
	float areaABC = crossABC.norm() * 0.5; //如果点P在三角形外，叉积结果就会有正负之分
	float areaABP = crossABP.norm() * 0.5;
	float areaAPC = crossAPC.norm() * 0.5;
	float areaPBC = crossPBC.norm() * 0.5;


	float u = areaPBC / areaABC;
	float v = areaAPC / areaABC;
	float w = areaABP / areaABC;
	return Vec3f(u, v, w);
}

//传入的是屏幕空间坐标,构建三角形(trianglePtr)的包围盒。逐一判断包围盒里面每个像素，是否在三角形内（重心坐标判断法）
void triangle(Vec3f* trianglePtr,TGAImage& image,TGAColor color)
{
	Vec3f A = trianglePtr[0];
	Vec3f B = trianglePtr[1];
	Vec3f C = trianglePtr[2];

	Vec2i rectMin = Vec2i(width - 1, height - 1);
	Vec2i rectMax = Vec2i(0, 0);
	for (int i = 0; i < 3; i++)
	{
		rectMin.x = (int)(trianglePtr[i].x < rectMin.x ? trianglePtr[i].x : rectMin.x);
		rectMin.y = (int)(trianglePtr[i].y < rectMin.y ? trianglePtr[i].y : rectMin.y);
		rectMax.x = (int)(trianglePtr[i].x > rectMax.x ? trianglePtr[i].x : rectMax.x);
		rectMax.y = (int)(trianglePtr[i].y > rectMax.y ? trianglePtr[i].y : rectMax.y);
	}

	Vec3f curPixel;
	Vec3f barCoord;
	for (int i = (int)rectMin.x; i <= rectMax.x; i++)
	{
		for (int j = (int)rectMin.y; j <= rectMax.y; j++)
		{
			curPixel = Vec3f(i, j, 1);
			barCoord = barycentric(trianglePtr, curPixel);//盒子内该像素在该三角形的重心坐标，判断像素是否在三角形内
			if (barCoord.x < 0 || barCoord.y < 0 || barCoord.z < 0)
				continue;//这个像素的重心坐标小于0，说明这个像素在三角形外，不画

			image.set(i, j, TGAColor(barCoord.x * 255, barCoord.y * 255, barCoord.z * 255, 1));
		}
	}
}

int main(int argc, char** argv)
{
	if (2 == argc)
		model = new Model(argv[1]);
	else
		model = new Model("obj/african_head.obj");

	TGAImage image(width, height, TGAImage::RGB); //纯黑的100 * 100图

	for (int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);//face是含有三个元素的，三个点形成的面，即.obj文件中的一行
		//v0 v1 v2是三个[-1,1]的坐标
		Vec3f v0 = model->vert(face[0]);
		Vec3f v1 = model->vert(face[1]);
		Vec3f v2 = model->vert(face[2]);
		//转换到[0,width] [0,height]屏幕坐标
		Vec3f v0screenCoord = Vec3f((v0.x + 1.0) * width / 2.0, (v0.y + 1.0) * height / 2.0, v0.z);
		Vec3f v1screenCoord = Vec3f((v1.x + 1.0) * width / 2.0, (v1.y + 1.0) * height / 2.0, v1.z);
		Vec3f v2screenCoord = Vec3f((v2.x + 1.0) * width / 2.0, (v2.y + 1.0) * height / 2.0, v2.z);
		Vec3f screenTriangle[3] = {v0screenCoord,v1screenCoord,v2screenCoord};
		triangle(screenTriangle, image, TGAColor(rand() % 255, rand() % 255, rand() % 255,1));
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}