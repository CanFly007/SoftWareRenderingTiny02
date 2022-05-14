#ifndef __OUR_GL_H__
#define __OUR_GL_H__

#include "maths.h"
#include "geometry.h"
#include "tgaimage.h"

extern mat4 world2View;//extern��ʾ�����ⲿģ��(main.cpp������ֱ����
extern mat4 view2Projection;
extern mat4 ndc2ViewPort;

void World2View(Vec3f cameraPos, Vec3f lookAtPos, Vec3f upDir);
void OrthoProjection(float cameraWidth, float cameraHeight, float cameraNearPlane, float cameraFarPlane);
void NDC2ViewPort(float screenWidth, float screenHeight);

struct IShader
{
	virtual ~IShader();
	virtual vec3 vertex(int iface, int nthvert) = 0;
	virtual bool fragment(Vec3f bar, TGAColor& color) = 0;//barָ������ص���������
};

void triangle(vec3* trianglePtr, IShader& shader, TGAImage& image, float* zBuffer, const int width);
//���������Ļ�ռ�����,����������(trianglePtr)�İ�Χ�С���һ�жϰ�Χ������ÿ�����أ��Ƿ����������ڣ����������жϷ���
void triangle(Vec3f* trianglePtr, Vec2f* triangleUVPtr, Vec3f* normalPtr, float* zBuffer, TGAImage& image, TGAColor colorconst, int width, const int height,  Vec3f light_dir);

#endif