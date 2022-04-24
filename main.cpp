#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

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
			for (int y = y1; y <= y0; y++)
			{
				float t = (y - y1) / (float)(y0 - y1);
				int x = x1 + t * (x0 - x1);
				image.set(x, y, color);
			}
		}
		else
		{
			for (int y = y0; y <= y1; y++)
			{
				float t = (y - y0) / (float)(y1 - y0);
				int x = x0 + t * (x1 - x0);
				image.set(x, y, color);
			}
		}
	}
	else
	{
		if (x0 > x1)
		{
			std::swap(x0, x1);
			std::swap(y0, y1);
			for (int x = x0; x < x1; x++)
			{
				float t = (x - x0) / (float)(x1 - x0);
				int y = y0 + t * (y1 - y0);
				image.set(x, y, color);
			}
		}
	}
}

int main(int argc, char** argv)
{
	TGAImage image(100, 100, TGAImage::RGB); //���ڵ�100 * 100ͼ
	line(13, 20, 80, 40, image, white);
	line(20, 13, 40, 80, image, red);
	line(80, 40, 13, 20, image, red);
	image.set(52, 41, red);//��52��41λ�û�һ����㣬DX����ϵ��ԭ�������Ͻ�
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}