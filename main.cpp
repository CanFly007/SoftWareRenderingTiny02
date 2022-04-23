#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

int main(int argc, char** argv)
{
	TGAImage image(100, 100, TGAImage::RGB); //���ڵ�100 * 100ͼ

	image.set(52, 41, red);//��52��41λ�û�һ����㣬DX����ϵ��ԭ�������Ͻ�
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}