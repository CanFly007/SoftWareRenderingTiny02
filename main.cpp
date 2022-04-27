#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

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

//�������Σ���������ABC�����λ�AB��BC��CA����line
//�������꣬������Vec2i intֵ
void triangle(Vec2i a, Vec2i b, Vec2i c, TGAImage& image, TGAColor color)
{
	line(a.x, a.y, b.x, b.y, image, color);
	line(b.x, b.y, c.x, c.y, image, color);
	line(c.x, c.y, a.x, a.y, image, color);
}

int main(int argc, char** argv)
{
	if (2 == argc)
		model = new Model(argv[1]);
	else
		model = new Model("obj/african_head.obj");

	TGAImage image(width, height, TGAImage::RGB); //���ڵ�100 * 100ͼ

	//Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
	//Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
	//Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
	//triangle(t0[0], t0[1], t0[2], image, red);
	//triangle(t1[0], t1[1], t1[2], image, white);
	//triangle(t2[0], t2[1], t2[2], image, green);

	for (int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);//face�Ǻ�������Ԫ�صģ��������γɵ��棬��.obj�ļ��е�һ��
		//v0 v1 v2������[-1,1]������
		Vec3f v0 = model->vert(face[0]);
		Vec3f v1 = model->vert(face[1]);
		Vec3f v2 = model->vert(face[2]);
		//ת����[0,width] [0,height]��Ļ����
		Vec2i v0i = Vec2i((int)((v0.x + 1.0) * width / 2.0), (int)((v0.y + 1.0) * height / 2.0));
		Vec2i v1i = Vec2i((int)((v1.x + 1.0) * width / 2.0), (int)((v1.y + 1.0) * height / 2.0));
		Vec2i v2i = Vec2i((int)((v2.x + 1.0) * width / 2.0), (int)((v2.y + 1.0) * height / 2.0));
		triangle(v0i, v1i, v2i, image, white);

		//for (int j = 0; j < 3; j++)//ѭ��3�Σ��ֱ𻭶���0�Ͷ���1���ߡ�����1�Ͷ���2���ߡ�����2�Ͷ���0����
		//{
		//	Vec3f v0 = model->vert(face[j]);//face[0]��f���е�һ��Ԫ�ء�vert(face[0])��ָ��Ķ��㣬��v����3����ֵVec3f
		//	Vec3f v1 = model->vert(face[(j + 1) % 3]);//ȡ���������ڶ�������Vec3f
		//	int x0 = (v0.x + 1.0) * width / 2.0;
		//	int y0 = (v0.y + 1.0) * height / 2.0;
		//	int x1 = (v1.x + 1.0) * width / 2.0;
		//	int y1 = (v1.y + 1.0) * height / 2.0;//[-1,1] -> [0,height]
		//	line(x0, y0, x1, y1, image, white);
		//}
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}