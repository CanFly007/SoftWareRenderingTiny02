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

//���� ��p��������trianglePtr�е���������
Vec3f barycentric(Vec3f* trianglePtr, Vec3f P)
{
	Vec3f A = trianglePtr[0];
	Vec3f B = trianglePtr[1];
	Vec3f C = trianglePtr[2];

	Vec3f crossABC = (B - A) ^ (C - A); //ABC ABP APC PBC���ǰ���P����������˳ʱ��˳����ϵ�
	Vec3f crossABP = (B - A) ^ (P - A);
	Vec3f crossAPC = (P - A) ^ (C - A);
	Vec3f crossPBC = (B - P) ^ (C - P);

	//����ķ��������P���������⣬�������ͻ�������֮��
	//��ѧ���������õ���жϷ���,ͬ����Ϊ��
	float outsideAB = crossABC * crossABP;//������Ϊ��������ʾABP����ʱ��ģ�P��AB�����
	float outsideAC = crossABC * crossAPC;
	float outsideBC = crossABC * crossPBC;
	if (outsideAB < 0 || outsideAC < 0 || outsideBC < 0)
		return Vec3f(-1, -1, -1);//��������һ���������������ڣ�������������ˣ�Ҳ����������uvwʱ��Ӹ���,AB���������wȨ��Ϊ����

	//�����ģ��ʾ������������Χ�ɵ�ƽ���ı������������������������
	float areaABC = crossABC.norm() * 0.5; //�����P���������⣬�������ͻ�������֮��
	float areaABP = crossABP.norm() * 0.5;
	float areaAPC = crossAPC.norm() * 0.5;
	float areaPBC = crossPBC.norm() * 0.5;


	float u = areaPBC / areaABC;
	float v = areaAPC / areaABC;
	float w = areaABP / areaABC;
	return Vec3f(u, v, w);
}

//���������Ļ�ռ�����,����������(trianglePtr)�İ�Χ�С���һ�жϰ�Χ������ÿ�����أ��Ƿ����������ڣ����������жϷ���
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
			barCoord = barycentric(trianglePtr, curPixel);//�����ڸ������ڸ������ε��������꣬�ж������Ƿ�����������
			if (barCoord.x < 0 || barCoord.y < 0 || barCoord.z < 0)
				continue;//������ص���������С��0��˵������������������⣬����

			image.set(i, j, color);
		}
	}
}

int main(int argc, char** argv)
{
	//if (2 == argc)
	//	model = new Model(argv[1]);
	//else
	//	model = new Model("obj/african_head.obj");

	TGAImage image(width, height, TGAImage::RGB); //���ڵ�100 * 100ͼ

	Vec3f t0[3] = { Vec3f(10, 70,1),   Vec3f(50, 160,1),  Vec3f(70, 80,1) };
	Vec3f t1[3] = { Vec3f(180, 50,1),  Vec3f(150, 1,1),   Vec3f(70, 180,1) };
	Vec3f t2[3] = { Vec3f(180, 150,1), Vec3f(120, 160,1), Vec3f(130, 180,1) };
	triangle(t0, image, red);
	triangle(t1, image, white);
	triangle(t2, image, green);

	//for (int i = 0; i < model->nfaces(); i++)
	//{
	//	std::vector<int> face = model->face(i);//face�Ǻ�������Ԫ�صģ��������γɵ��棬��.obj�ļ��е�һ��
	//	//v0 v1 v2������[-1,1]������
	//	Vec3f v0 = model->vert(face[0]);
	//	Vec3f v1 = model->vert(face[1]);
	//	Vec3f v2 = model->vert(face[2]);
	//	//ת����[0,width] [0,height]��Ļ����
	//	Vec2i v0i = Vec2i((int)((v0.x + 1.0) * width / 2.0), (int)((v0.y + 1.0) * height / 2.0));
	//	Vec2i v1i = Vec2i((int)((v1.x + 1.0) * width / 2.0), (int)((v1.y + 1.0) * height / 2.0));
	//	Vec2i v2i = Vec2i((int)((v2.x + 1.0) * width / 2.0), (int)((v2.y + 1.0) * height / 2.0));
	//	triangle(v0i, v1i, v2i, image, white);

	//	//for (int j = 0; j < 3; j++)//ѭ��3�Σ��ֱ𻭶���0�Ͷ���1���ߡ�����1�Ͷ���2���ߡ�����2�Ͷ���0����
	//	//{
	//	//	Vec3f v0 = model->vert(face[j]);//face[0]��f���е�һ��Ԫ�ء�vert(face[0])��ָ��Ķ��㣬��v����3����ֵVec3f
	//	//	Vec3f v1 = model->vert(face[(j + 1) % 3]);//ȡ���������ڶ�������Vec3f
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