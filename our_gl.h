#ifndef __OUR_GL_H__
#define __OUR_GL_H__

#include "maths.h"
#include "geometry.h"
#include "tgaimage.h"

extern mat4 world2View;//extern表示其他外部模块(main.cpp）可以直接用
extern mat4 view2Projection;
extern mat4 ndc2ViewPort;

void World2View(Vec3f cameraPos, Vec3f lookAtPos, Vec3f upDir);
void OrthoProjection(float cameraWidth, float cameraHeight, float cameraNearPlane, float cameraFarPlane);
void NDC2ViewPort(float screenWidth, float screenHeight);

struct IShader
{
	virtual ~IShader();
	virtual vec3 vertex(int iface, int nthvert) = 0;
	virtual bool fragment(Vec3f bar, TGAColor& color) = 0;//bar指这个像素的重心坐标
};

void triangle(vec3* trianglePtr, IShader& shader, TGAImage& image, float* zBuffer, const int width);
//传入的是屏幕空间坐标,构建三角形(trianglePtr)的包围盒。逐一判断包围盒里面每个像素，是否在三角形内（重心坐标判断法）
void triangle(Vec3f* trianglePtr, Vec2f* triangleUVPtr, Vec3f* normalPtr, float* zBuffer, TGAImage& image, TGAColor colorconst, int width, const int height,  Vec3f light_dir);

#endif