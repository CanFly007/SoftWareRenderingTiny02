#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

//测试model.cpp中的iss>>返回什么
//#include <iostream>
//#include <string>
//#include <fstream>
//#include <sstream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
Model* model = NULL;
const int width = 800;
const int height = 800;

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

int main(int argc, char** argv)
{
	//测试model.cpp中的iss>>返回什么
	//std::string os("f 24/1/24 25/2/25 26/3/26");
	//char trash;
	//int itrash, idx;
	//std::istringstream iss(os.c_str());
	//iss >> trash;
	//std::cout << trash << std::endl;
	//iss >> idx >> trash>>itrash>>trash;
	//std::cout << idx << std::endl;
	//std::cout << trash << std::endl;
	//std::cout << itrash << std::endl;
	//return 1;

	if (2 == argc)
		model = new Model(argv[1]);
	else
		model = new Model("obj/african_head.obj");

	TGAImage image(width, height, TGAImage::RGB); //纯黑的100 * 100图

	for (int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);//face是含有三个元素的，三个点形成的面，即.obj文件中的一行
		for (int j = 0; j < 3; j++)//循环3次，分别画顶点0和顶点1连线、顶点1和顶点2连线、顶点2和顶点0连线
		{
			Vec3f v0 = model->vert(face[j]);//face[0]：f行中第一个元素。vert(face[0])：指向的顶点，即v行中3个数值Vec3f
			Vec3f v1 = model->vert(face[(j + 1) % 3]);//取余数，即第二个顶点Vec3f
			int x0 = (v0.x + 1.0) * width / 2.0;
			int y0 = (v0.y + 1.0) * height / 2.0;
			int x1 = (v1.x + 1.0) * width / 2.0;
			int y1 = (v1.y + 1.0) * height / 2.0;//[-1,1] -> [0,height]
			line(x0, y0, x1, y1, image, white);
		}
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}