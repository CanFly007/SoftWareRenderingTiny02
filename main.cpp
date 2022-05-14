#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "matrix.h"
#include "maths.h"
#include "our_gl.h"


const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

Model* model = NULL;
const int width = 800;
const int height = 800;

Vec3f light_dir(0, 0, 1); //��������ϵ����ʾ�ڸõ�Ϊ���Ĺ��գ������Է���
Vec3f cameraPos(0, 0, -3);
Vec3f lookAtPos(0, 0, 0);

//�������
float cameraWidth = 3.0;
float cameraHeight = 3.0;
float cameraFarPlane = 100.0;
float cameraNearPlane = 1.0;

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

struct PhongShader :public IShader
{
	Vec3f varying_intensity;

	virtual vec3 vertex(int iface, int nthvert)
	{
		Vec3f pos = model->vertPos(iface, nthvert);
		vec4 pos_homogeneous = vec4(pos[0], pos[1], pos[2], 1.0);
		varying_intensity[nthvert] = std::max(0.0f, model->vertNormal(iface, nthvert) * light_dir);

		mat4 MVP = view2Projection * world2View;
		vec4 gl_Position = MVP * pos_homogeneous;
		//�����ü�֮��xyz������Ҫ��һ��͸�ӳ���������w
		vec4 NDC = vec4(gl_Position[0] / gl_Position[3], gl_Position[1] / gl_Position[3], gl_Position[2] / gl_Position[3], gl_Position[3]);
		vec4 viewPort = ndc2ViewPort * NDC;
		return vec3(viewPort[0],viewPort[1],viewPort[2]);
	}
	virtual bool fragment(Vec3f bar, TGAColor& color)//barָ������ص���������};
	{
		float lambert = varying_intensity * bar;//varying_intensity��һ��Vec3f�������������ֱ��lambertֵ���ø��������������ӦABCȨ�طֱ���Զ�Ӧ���intensity
		lambert *= 255;
		color = TGAColor(lambert, lambert, lambert, 255);
		return false;
	}
};

//.obj�ļ��ж���Ķ�������[-1,1]��ת������Ļ����[0,width]��[0,height]��zֵ���䣬��Ȼ��[-1,1]����
Vec3f World2Screen(Vec3f worldPos)
{
	return Vec3f((worldPos.x + 1.0) * 0.5 * width, (worldPos.y + 1.0) * 0.5 * height, worldPos.z);
}

#include <iostream>
int main(int argc, char** argv)
{
	if (2 == argc)
		model = new Model(argv[1]);
	else
		model = new Model("obj/african_head.obj");

	//ͨ�����߿�֪��������������ϵ�������¿�ʼ�ģ���ʵ�����Ͽ�ʼ��������flip_vertically�ĳ������£�
	TGAImage image(width, height, TGAImage::RGB); //���ڵ�100 * 100ͼ
	
	float* zBuffer = new float[width * height];
	for (int i = 0; i < width * height; i++)
		zBuffer[i] = -std::numeric_limits<float>::max();

	//���our_gl������ת�������ֵ
	World2View(cameraPos, lookAtPos, Vec3f(0, 1, 0));
	OrthoProjection(cameraWidth, cameraHeight, cameraNearPlane, cameraFarPlane);
	NDC2ViewPort(width, height);


	PhongShader shader;
	for (int i = 0; i < model->nfaces(); i++)//����faces_����������ÿ��Ԫ�ش���f��һ�У���3������UV������������
	{
		vec3 screen_coords[3];//[3]��ʾһ����������3������ vec3��ʾÿ�������xyz���꣬xy����Ļ����[0,width],z��NDC����[-1,1]
		for (int j = 0; j < 3; j++)//face��3�������Ӧ��һ��
		{
			screen_coords[j] = shader.vertex(i, j);
		}
		triangle(screen_coords, shader, image, zBuffer, width);//�����������
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}