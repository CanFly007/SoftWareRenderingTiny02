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


	for (int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);//face�Ǻ�������Ԫ�صģ��������γɵ��棬��.obj�ļ��е�һ��
		//v0 v1 v2������[-1,1]������
		Vec3f v0 = model->vert(face[0]);
		Vec2f uv0 = model->GetUV(face[1]);
		Vec3f normal0 = model->GetNormal(face[2]);

		Vec3f v1 = model->vert(face[3]);
		Vec2f uv1 = model->GetUV(face[4]);
		Vec3f normal1 = model->GetNormal(face[5]);

		Vec3f v2 = model->vert(face[6]);
		Vec2f uv2 = model->GetUV(face[7]);
		Vec3f normal2 = model->GetNormal(face[8]);

		vec4 v0_homogeneous = vec4(v0[0], v0[1], v0[2], 1.0);//��д��Vec3��ת��Сд���֮࣬�����ͳһ��һ����
		vec4 v1_homogeneous = vec4(v1[0], v1[1], v1[2], 1.0);
		vec4 v2_homogeneous = vec4(v2[0], v2[1], v2[2], 1.0);

		vec4 v0_view = world2View * v0_homogeneous;
		vec4 v1_view = world2View * v1_homogeneous;
		vec4 v2_view = world2View * v2_homogeneous;

		vec4 v0_projection = view2Projection * v0_view;
		vec4 v1_projection = view2Projection * v1_view;
		vec4 v2_projection = view2Projection * v2_view;
		
		//�����ü�֮��xyz������Ҫ��һ��͸�ӳ���������w
		vec4 v0_division = vec4(v0_projection[0] / v0_projection[3], v0_projection[1] / v0_projection[3], v0_projection[2] / v0_projection[3], v0_projection[3]);
		vec4 v1_division = vec4(v1_projection[0] / v1_projection[3], v1_projection[1] / v1_projection[3], v1_projection[2] / v1_projection[3], v1_projection[3]);
		vec4 v2_division = vec4(v2_projection[0] / v2_projection[3], v2_projection[1] / v2_projection[3], v2_projection[2] / v2_projection[3], v2_projection[3]);

		vec4 v0_viewPort = ndc2ViewPort * v0_division;
		vec4 v1_viewPort = ndc2ViewPort * v1_division;
		vec4 v2_viewPort = ndc2ViewPort * v2_division;

		Vec3f v0screenCoord = Vec3f(v0_viewPort[0], v0_viewPort[1], v0_viewPort[2]);
		Vec3f v1screenCoord = Vec3f(v1_viewPort[0], v1_viewPort[1], v1_viewPort[2]);
		Vec3f v2screenCoord = Vec3f(v2_viewPort[0], v2_viewPort[1], v2_viewPort[2]);

		//ת����[0,width] [0,height]��Ļ����
		//Vec3f v0screenCoord = World2Screen(v0);
		//Vec3f v1screenCoord = World2Screen(v1);
		//Vec3f v2screenCoord = World2Screen(v2);
		Vec3f screenTriangle[3] = { v0screenCoord,v1screenCoord,v2screenCoord };

		//���淨�ߴ��涥�㷨�߲�ֵ�����أ���ÿ�������ε���ɫ
		Vec3f faceNormal = (v1 - v0) ^ (v2 - v0);//���������Ĳ�������Ǵ�ֱ�����ǵķ�����
		//ע�⣺faceNormal���������һ�һ�£��������normal����Ҳ�ỻ180�ȣ�ͨ���������������Կ�v0 v1 v2��˳����˳ʱ���
		faceNormal.normalize();
		float lambert = faceNormal * light_dir * 255;//��������������һ����ɫ

		//���������������������uvֵ
		Vec2f triangleUV[3] = {uv0,uv1,uv2};

		Vec3f triangleNormals[3] = { normal0,normal1,normal2 };//����������ͨ�����������ֵ��ÿ�����ص�normal����lambert

		triangle(screenTriangle, triangleUV, triangleNormals,
			zBuffer, image, TGAColor(lambert, lambert, lambert, 1),width,height,light_dir);//����zBuffer��⣬lambert>0����������ȥ����
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}