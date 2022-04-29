#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

Model* model = NULL;
const int width = 200;
const int height = 200;

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

//画三角形，三个顶点ABC、依次画AB、BC、CA三条line
//像素坐标，所以是Vec2i int值
void triangle(Vec2i a, Vec2i b, Vec2i c, TGAImage& image, TGAColor color)
{
	//按照y坐标排序，保持a.y < b.y < c.y 冒泡排序法
	if (a.y > b.y) std::swap(a, b);
	if (a.y > c.y) std::swap(a, c);
	if (b.y > c.y) std::swap(b, c);

	//a点在最下面，中间是b点，c点在最上面
	//三角形从b处水平切分成上下两部分，因为ab和bc斜率不同，画x的起点要分两次算
	//从y轴底开始画（可以在草稿本上画个三角形看下）、水平一行一行画
	for (int y = a.y; y <= c.y; y++)
	{
		bool isUpperTriangle = false;
		if (y >= b.y)//大于大于，防止a点和b点Y相等情况。此时只算上部分即可
			isUpperTriangle = true;
		//b点在ac边的左侧，每行的x横扫从b这边开始。b点在ac边右侧，x横扫从ac边开始
		//因为a是最低点，判断b在a的左右即可。如果a和b的x相同，要判断c的x位置决定三角形样式
		bool isBLeft = true;
		if (b.x < a.x)
			isBLeft = true;
		else if (b.x == a.x)
		{
			if (c.x > b.x)
				isBLeft = true;
			else
				isBLeft = false;
		}
		else
			isBLeft = false;

		if (!isUpperTriangle)
		{
			float t2 = (float)(y - a.y) / (c.y - a.y);//ac边比例
			int x2 = a.x + (c.x - a.x) * t2;
			float t0 = (float)(y - a.y) / (b.y - a.y);//ab边比例
			int x0 = a.x + (b.x - a.x) * t0;
			if (isBLeft)
			{
				//b点在左边，从x0到x2
				for (int x = x0; x <= x2; x++)
					image.set(x, y, white);
			}
			else
			{
				for (int x = x2; x <= x0; x++)
					image.set(x, y, white);
			}
		}
		else
		{
			float t2 = (float)(y - a.y) / (c.y - a.y);//ac边比例
			int x2 = a.x + (c.x - a.x) * t2;
			//bc边比例
			float t1 = (float)(y - b.y) / (c.y - b.y);
			int x1 = b.x + t1 * (c.x - b.x);
			if (isBLeft)
				for (int x = x1; x <= x2; x++)
					image.set(x, y, white);
			else
				for (int x = x2; x <= x1; x++)
					image.set(x, y, white);
		}
	}
}

int main(int argc, char** argv)
{
	//if (2 == argc)
	//	model = new Model(argv[1]);
	//else
	//	model = new Model("obj/african_head.obj");

	TGAImage image(width, height, TGAImage::RGB); //纯黑的100 * 100图

	Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
	Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
	Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
	triangle(t0[0], t0[1], t0[2], image, red);
	triangle(t1[0], t1[1], t1[2], image, white);
	triangle(t2[0], t2[1], t2[2], image, green);

	//for (int i = 0; i < model->nfaces(); i++)
	//{
	//	std::vector<int> face = model->face(i);//face是含有三个元素的，三个点形成的面，即.obj文件中的一行
	//	//v0 v1 v2是三个[-1,1]的坐标
	//	Vec3f v0 = model->vert(face[0]);
	//	Vec3f v1 = model->vert(face[1]);
	//	Vec3f v2 = model->vert(face[2]);
	//	//转换到[0,width] [0,height]屏幕坐标
	//	Vec2i v0i = Vec2i((int)((v0.x + 1.0) * width / 2.0), (int)((v0.y + 1.0) * height / 2.0));
	//	Vec2i v1i = Vec2i((int)((v1.x + 1.0) * width / 2.0), (int)((v1.y + 1.0) * height / 2.0));
	//	Vec2i v2i = Vec2i((int)((v2.x + 1.0) * width / 2.0), (int)((v2.y + 1.0) * height / 2.0));
	//	triangle(v0i, v1i, v2i, image, white);

	//	//for (int j = 0; j < 3; j++)//循环3次，分别画顶点0和顶点1连线、顶点1和顶点2连线、顶点2和顶点0连线
	//	//{
	//	//	Vec3f v0 = model->vert(face[j]);//face[0]：f行中第一个元素。vert(face[0])：指向的顶点，即v行中3个数值Vec3f
	//	//	Vec3f v1 = model->vert(face[(j + 1) % 3]);//取余数，即第二个顶点Vec3f
	//	//	int x0 = (v0.x + 1.0) * width / 2.0;
	//	//	int y0 = (v0.y + 1.0) * height / 2.0;
	//	//	int x1 = (v1.x + 1.0) * width / 2.0;
	//	//	int y1 = (v1.y + 1.0) * height / 2.0;//[-1,1] -> [0,height]
	//	//	line(x0, y0, x1, y1, image, white);
	//	//}
	//}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}