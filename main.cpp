#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

int main(int argc, char** argv)
{
	TGAImage image(100, 100, TGAImage::RGB); //纯黑的100 * 100图

	image.set(52, 41, red);//在52，41位置画一个红点，DX坐标系，原点在左上角
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}