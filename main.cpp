#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
	for (int x = x0; x <= x1; x++)//画必要步：直线按照每个横坐标走一步
	{
		//当前的x走到的位置，在直线中的比例
		float t = (x - x0) / (float)(x1 - x0);//除以float而不是int
		int y = y0 + t * (y1 - y0);//根据百分比t，此时y同时在哪里
		image.set(x, y, color);
	}
}

int main(int argc, char** argv)
{
	TGAImage image(100, 100, TGAImage::RGB); //纯黑的100 * 100图
	line(13, 20, 80, 40, image, white);
	line(20, 13, 40, 80, image, red);
	line(80, 40, 13, 20, image, red);
	image.set(52, 41, red);//在52，41位置画一个红点，DX坐标系，原点在左上角
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}