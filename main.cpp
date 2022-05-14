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
		//正交裁剪之后，xyz分量需要做一次透视除法，除以w
		vec4 NDC = vec4(gl_Position[0] / gl_Position[3], gl_Position[1] / gl_Position[3], gl_Position[2] / gl_Position[3], gl_Position[3]);
		vec4 viewPort = ndc2ViewPort * NDC;
		return vec3(viewPort[0],viewPort[1],viewPort[2]);
	}
	virtual bool fragment(Vec3f bar, TGAColor& color)//bar指这个像素的重心坐标};
	{
		float lambert = varying_intensity * bar;//varying_intensity是一个Vec3f数组代表三个点分别的lambert值，用该像素重心坐标对应ABC权重分别乘以对应点的intensity
		lambert *= 255;
		color = TGAColor(lambert, lambert, lambert, 255);
		return false;
	}
};

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


	PhongShader shader;
	for (int i = 0; i < model->nfaces(); i++)//遍历faces_容器，里面每个元素代表f中一行，即3个顶点UV法线索引集合
	{
		vec3 screen_coords[3];//[3]表示一个三角形有3个顶点 vec3表示每个顶点的xyz坐标，xy是屏幕坐标[0,width],z是NDC坐标[-1,1]
		for (int j = 0; j < 3; j++)//face中3个顶点对应哪一个
		{
			screen_coords[j] = shader.vertex(i, j);
		}
		triangle(screen_coords, shader, image, zBuffer, width);//画这个三角形
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}