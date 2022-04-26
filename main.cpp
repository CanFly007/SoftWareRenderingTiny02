#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

//����model.cpp�е�iss>>����ʲô
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
	//����(80,200)��(70,100)��������
	//����1��x�������Ǽ�С�Ķ����������
	//����2��Y���ȸ���Ӧ�ð���Y��ÿ����
	bool yStep = false;
	if (abs(x0 - x1) < abs(y0 - y1))
	{
		yStep = true;
	}
	if (yStep)
	{
		if (y0 > y1)//y1С����y1��y0
		{
			//���������㣬����С�������
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
	//����model.cpp�е�iss>>����ʲô
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

	TGAImage image(width, height, TGAImage::RGB); //���ڵ�100 * 100ͼ

	for (int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);//face�Ǻ�������Ԫ�صģ��������γɵ��棬��.obj�ļ��е�һ��
		for (int j = 0; j < 3; j++)//ѭ��3�Σ��ֱ𻭶���0�Ͷ���1���ߡ�����1�Ͷ���2���ߡ�����2�Ͷ���0����
		{
			Vec3f v0 = model->vert(face[j]);//face[0]��f���е�һ��Ԫ�ء�vert(face[0])��ָ��Ķ��㣬��v����3����ֵVec3f
			Vec3f v1 = model->vert(face[(j + 1) % 3]);//ȡ���������ڶ�������Vec3f
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