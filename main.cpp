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

Vec3f light_dir(0, 0, 1); //右手坐标系，表示在该点为起点的光照，非来自方向
Vec3f cameraPos(0, 0, -3);
Vec3f lookAtPos(0, 0, 0);

//正交相机
float cameraWidth = 3.0;
float cameraHeight = 3.0;
float cameraFarPlane = 100.0;
float cameraNearPlane = 1.0;

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
	//比如(80,200)和(70,100)这段特殊点
	//特殊1：x方向上是减小的而不是增大的
	//特殊2：Y幅度更大，应该按照Y轴每步走
	bool yStep = false;
	if (abs(x0 - x1) < abs(y0 - y1))
	{
		yStep = true;
	}
	if (yStep)
	{
		if (y0 > y1)//y1小，从y1到y0
		{
			//交换两个点，保持小的在左侧
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

//.obj文件中定义的顶点坐标[-1,1]，转换到屏幕坐标[0,width]和[0,height]，z值不变，仍然是[-1,1]区间
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

	//通过画线可知，这是右手坐标系，从左下开始的（其实从左上开始，被下面flip_vertically改成了左下）
	TGAImage image(width, height, TGAImage::RGB); //纯黑的100 * 100图
	
	float* zBuffer = new float[width * height];
	for (int i = 0; i < width * height; i++)
		zBuffer[i] = -std::numeric_limits<float>::max();

	//算出our_gl中三个转换矩阵的值
	World2View(cameraPos, lookAtPos, Vec3f(0, 1, 0));
	OrthoProjection(cameraWidth, cameraHeight, cameraNearPlane, cameraFarPlane);
	NDC2ViewPort(width, height);


	for (int i = 0; i < model->nfaces(); i++)
	{
		std::vector<int> face = model->face(i);//face是含有三个元素的，三个点形成的面，即.obj文件中的一行
		//v0 v1 v2是三个[-1,1]的坐标
		Vec3f v0 = model->vert(face[0]);
		Vec2f uv0 = model->GetUV(face[1]);
		Vec3f normal0 = model->GetNormal(face[2]);

		Vec3f v1 = model->vert(face[3]);
		Vec2f uv1 = model->GetUV(face[4]);
		Vec3f normal1 = model->GetNormal(face[5]);

		Vec3f v2 = model->vert(face[6]);
		Vec2f uv2 = model->GetUV(face[7]);
		Vec3f normal2 = model->GetNormal(face[8]);

		vec4 v0_homogeneous = vec4(v0[0], v0[1], v0[2], 1.0);//大写的Vec3类转到小写的类，之后可以统一成一个类
		vec4 v1_homogeneous = vec4(v1[0], v1[1], v1[2], 1.0);
		vec4 v2_homogeneous = vec4(v2[0], v2[1], v2[2], 1.0);

		vec4 v0_view = world2View * v0_homogeneous;
		vec4 v1_view = world2View * v1_homogeneous;
		vec4 v2_view = world2View * v2_homogeneous;

		vec4 v0_projection = view2Projection * v0_view;
		vec4 v1_projection = view2Projection * v1_view;
		vec4 v2_projection = view2Projection * v2_view;
		
		//正交裁剪之后，xyz分量需要做一次透视除法，除以w
		vec4 v0_division = vec4(v0_projection[0] / v0_projection[3], v0_projection[1] / v0_projection[3], v0_projection[2] / v0_projection[3], v0_projection[3]);
		vec4 v1_division = vec4(v1_projection[0] / v1_projection[3], v1_projection[1] / v1_projection[3], v1_projection[2] / v1_projection[3], v1_projection[3]);
		vec4 v2_division = vec4(v2_projection[0] / v2_projection[3], v2_projection[1] / v2_projection[3], v2_projection[2] / v2_projection[3], v2_projection[3]);

		vec4 v0_viewPort = ndc2ViewPort * v0_division;
		vec4 v1_viewPort = ndc2ViewPort * v1_division;
		vec4 v2_viewPort = ndc2ViewPort * v2_division;

		Vec3f v0screenCoord = Vec3f(v0_viewPort[0], v0_viewPort[1], v0_viewPort[2]);
		Vec3f v1screenCoord = Vec3f(v1_viewPort[0], v1_viewPort[1], v1_viewPort[2]);
		Vec3f v2screenCoord = Vec3f(v2_viewPort[0], v2_viewPort[1], v2_viewPort[2]);

		//转换到[0,width] [0,height]屏幕坐标
		//Vec3f v0screenCoord = World2Screen(v0);
		//Vec3f v1screenCoord = World2Screen(v1);
		//Vec3f v2screenCoord = World2Screen(v2);
		Vec3f screenTriangle[3] = { v0screenCoord,v1screenCoord,v2screenCoord };

		//用面法线代替顶点法线插值到像素，画每个三角形的颜色
		Vec3f faceNormal = (v1 - v0) ^ (v2 - v0);//两个向量的叉积，既是垂直于他们的法向量
		//注意：faceNormal如果叉积左右换一下，算出来的normal方向也会换180度，通过输出重心坐标可以看v0 v1 v2的顺序是顺时针的
		faceNormal.normalize();
		float lambert = faceNormal * light_dir * 255;//整个三角形面是一个颜色

		//获得这个三角形三个顶点的uv值
		Vec2f triangleUV[3] = {uv0,uv1,uv2};

		Vec3f triangleNormals[3] = { normal0,normal1,normal2 };//到像素里面通过重心坐标插值算每个像素的normal，算lambert

		triangle(screenTriangle, triangleUV, triangleNormals,
			zBuffer, image, TGAColor(lambert, lambert, lambert, 1),width,height,light_dir);//加了zBuffer检测，lambert>0的条件可以去掉了
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}