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

Vec3f light_dir(1, 1, 0); //右手坐标系，表示在该点为起点的光照，非来自方向
Vec3f cameraPos(2, 0, 3); //obj在原点，摄像机看向原点就能看到脸部
//Vec3f cameraPos(0, 0, 3);
Vec3f lookAtPos(0, 0, 0);

//正交相机
float cameraWidth = 2.5;
float cameraHeight = 2.5;
float cameraFarPlane = 15.0;
float cameraNearPlane = 0.1;

float* shadowmapZBuffer = NULL;

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
	Vec2f varying_uv[3];//三个点分别的uv值

	Vec3f varying_WorldPos[3];
	Vec3f varying_WorldNormal[3];

	mat4 uniform_World2LightMatrix;

	PhongShader(mat4 world2LightMatrix) :uniform_World2LightMatrix(world2LightMatrix) {}

	//iface是从0遍历到faces_最后，nthvert是内循环，范围是[0,3)
	virtual vec3 vertex(int iface, int nthvert)
	{
		Vec3f pos = model->vertPos(iface, nthvert);
		vec4 pos_homogeneous = vec4(pos[0], pos[1], pos[2], 1.0);
		varying_WorldPos[nthvert] = pos;
		varying_intensity[nthvert] = std::max(0.0f, model->vertNormal(iface, nthvert) * light_dir);
		varying_WorldNormal[nthvert] = model->vertNormal(iface, nthvert);
		
		varying_uv[nthvert] = model->vertUV(iface, nthvert);

		mat4 MVP = view2Projection * world2View;
		vec4 gl_Position = MVP * pos_homogeneous;
		//正交裁剪之后，xyz分量需要做一次透视除法，除以w
		vec4 NDC = vec4(gl_Position[0] / gl_Position[3], gl_Position[1] / gl_Position[3], gl_Position[2] / gl_Position[3], gl_Position[3]);
		vec4 viewPort = ndc2ViewPort * NDC;
		return vec3(viewPort[0],viewPort[1],viewPort[2]);
	}
	virtual bool fragment(Vec3f bar, TGAColor& color)//bar指这个像素的重心坐标};
	{
		float lambert;
		lambert = varying_intensity * bar;//varying_intensity是一个Vec3f数组代表三个点分别的lambert值，用该像素重心坐标对应ABC权重分别乘以对应点的intensity
		Vec2f uv = varying_uv[0] * bar[0] + varying_uv[1] * bar[1] + varying_uv[2] * bar[2];
		Vec3f worldPos = varying_WorldPos[0] * bar[0] + varying_WorldPos[1] * bar[1] + varying_WorldPos[2] * bar[2];
		
		Vec3f worldNormal = varying_WorldNormal[0] * bar[0] + varying_WorldNormal[1] * bar[1] + varying_WorldNormal[2] * bar[2];//由三个顶点自带法线的重心权重算出 
		worldNormal.normalize();

		//上面两部插值操作，一个是插值三个顶点的float数据，一个是插值三个顶点的uv，在三角形遍历阶段已经由硬件完成，不用在fragment中完成
		
		//阴影
		vec4 worldPosHomogeneous = vec4(worldPos.x, worldPos.y, worldPos.z, 1.0);
		vec4 lightSpaceCoords = uniform_World2LightMatrix * worldPosHomogeneous;//xy是[0,width] z是[-1,1]
		//这里通过shadowmapZBuffer数组查找，而不是shadowmap贴图查找。所以z的范围还是[-1,1]左手坐标系
		int index = int(lightSpaceCoords.x()) + int(lightSpaceCoords.y()) * width;
		float shadow = shadowmapZBuffer[index] > lightSpaceCoords.z();

#pragma region 切线计算，然后构建空间算法
		//1、构建TBN坐标系，在世界空间表示TBN向量，然后按列排列即得到 TangentSpace -> WorldSpace 的矩阵
		Vec3f AB = varying_WorldPos[1] - varying_WorldPos[0];
		Vec3f AC = varying_WorldPos[2] - varying_WorldPos[0];
		float deltaU0 = varying_uv[1].u - varying_uv[0].u;//AB向量的U和V差
		float deltaV0 = varying_uv[1].v - varying_uv[0].v;
		float deltaU1 = varying_uv[2].u - varying_uv[0].u;//AC向量U和V的差
		float deltaV1 = varying_uv[2].v - varying_uv[0].v;

		float determinant = 1.0 / (deltaU0 * deltaV1 - deltaV0 * deltaU1);//详见印象笔记切线空间
		Vec3f worldSpaceT = determinant * (deltaV1 * AB - deltaV0 * AC);//这里顺序也出错了！！！
		Vec3f worldSpaceB = determinant * (-deltaU1 * AB + deltaU0 * AC);

		//Tangent和Binormal的归一化公式不知为何，详见印象笔记
		worldSpaceT = worldSpaceT - (worldSpaceT * worldNormal) * worldNormal;
		worldSpaceT.normalize();//一定记得归一化
		worldSpaceB = worldSpaceB - (worldSpaceB * worldNormal) * worldNormal - (worldSpaceB * worldSpaceT) * worldSpaceT;
		worldSpaceB.normalize();

		//可以通过TB叉乘，也可以用该像素的normal值（三个顶点normal的重心权重）,试着比较两者是否一致？
		Vec3f worldNormalDebug = worldNormal.normalize();
		Vec3f worldNormalCross = (worldSpaceT ^ worldSpaceB).normalize();//Debug精度差不多一致

		//2、TBN按列排，构成Tangent2World矩阵
		mat3 tangent2World;
		tangent2World[0][0] = worldSpaceT.x; tangent2World[0][1] = worldSpaceB.x; tangent2World[0][2] = worldNormalCross.x;
		tangent2World[1][0] = worldSpaceT.y; tangent2World[1][1] = worldSpaceB.y; tangent2World[1][2] = worldNormalCross.y;
		tangent2World[2][0] = worldSpaceT.z; tangent2World[2][1] = worldSpaceB.z; tangent2World[2][2] = worldNormalCross.z;

		TGAColor normalMap = model->SamplerNormalColor(uv);
		//解析采样到的normalMap贴图，这张是切线空间坐标表示的
		//Unity中的UnpackNormal函数是把[0,1]变成[-1,1]，这里多一步从[0,255]变成[0,1]
		//切线空间算的是对的！！！！错误出在这，TGAColor的构造函数顺序是b g r，要颠倒一下！！！
		vec3 tangentNormal = vec3((float)normalMap.raw[2] / 255.0, (float)normalMap.raw[1] / 255.0, (float)normalMap.raw[0] / 255.0);
		tangentNormal = tangentNormal * 2.0 - 1.0;
		////tangentNormal.normalize();
		vec3 convertToWorldNormal = tangent2World * tangentNormal;//convertToWorldNormal是把normalMap转换到世界空间的法线，与上面的worldNormal顶点法线不同，上面的worldNormal一般在顶点着色器计算，然后插值矩阵到片元
		Vec3f normal = Vec3f(convertToWorldNormal[0], convertToWorldNormal[1], convertToWorldNormal[2]);
		normal.normalize();
#pragma endregion

		//解析采样到的normalMap贴图，这张是世界坐标表示的（非切线空间）
		//Vec3f normal = Vec3f((normalMap.r / 255.0) * 2 - 1, (normalMap.g / 255.0) * 2 - 1, (normalMap.b / 255.0) * 2 - 1);//[0,255]->[-1,1]
		//normal = normal.normalize();

		//世界空间worldNormal * worldLightDir
		float a0 = light_dir[0];
		float a1 = light_dir[1];
		float a2 = light_dir[2];
		Vec3f worldLightDir = Vec3f(a0,a1,a2);//.normalize();
		float ndotL = std::max(0.f, normal * worldLightDir);

		TGAColor albedo = model->SamplerDiffseColor(uv);
		Vec3f diffuse = Vec3f(albedo.r * ndotL, albedo.g * ndotL, albedo.b * ndotL);
		//lambert = ndotL;

#pragma region 高光Specular
		TGAColor specMap = model->SamplerSpcularColor(uv);//这张图算成高光幂的图
		float gloss = specMap.r;
		//Vec3f test1Dir = Vec3f(cameraPos.x, cameraPos.y, cameraPos.z);
		Vec3f worldViewDir = cameraPos - worldPos;
		worldViewDir.normalize();
		Vec3f reflectDir = -worldLightDir + normal * (worldLightDir * normal) * 2;
		reflectDir.normalize();
		float rdotV = reflectDir * worldViewDir;
		Vec3f specularColor = Vec3f(1, 1, 1);//高光颜色
		//点积做底，gloss做幂。想象x平方曲线，再想象x三次方曲线（即只有当反射方向和观察方向很靠近时（点积接近1），256幂次方结果才靠近1，否则幂次方的值很小）
		Vec3f specular = specularColor * pow(std::max(0.f, rdotV), gloss);
		specular = specular * 255;
#pragma endregion

		Vec3f ambient = Vec3f(5.0, 5.0, 5.0);
		Vec3f result = ambient + diffuse;// +specular;
		//result = result * shadow;
		color = TGAColor(result[0], result[1], result[2], albedo.a);
		return false;
	}
};

struct ShadowMapShader :IShader //光源的shadowmap图
{
	float viewPortZ[3];//z在视图空间范围，[-1,1]

	virtual vec3 vertex(int iface, int nthvert)//三角形每个顶点调用一次，每个三角形调用三次
	{
		Vec3f pos = model->vertPos(iface, nthvert);
		vec4 pos_homogeneous = vec4(pos[0], pos[1], pos[2], 1.0);
		vec4 gl_Position = view2Projection * world2View * pos_homogeneous;
		//透视除法变成[-1,1]
		vec4 NDC = vec4(gl_Position[0] / gl_Position[3], gl_Position[1] / gl_Position[3], gl_Position[2] / gl_Position[3], gl_Position[3]);
		vec4 viewPort = ndc2ViewPort * NDC;
		viewPortZ[nthvert] = viewPort[2];
		return vec3(viewPort[0], viewPort[1], viewPort[2]);
	}
	virtual bool fragment(Vec3f bar, TGAColor& color)//AABB盒子中在三角形内部的每个像素调用一次fragment
	{
		float interpolationZ = bar[0] * viewPortZ[0] + bar[1] * viewPortZ[1] + bar[2] * viewPortZ[2];
		//从[-1,1] -> [0,1] 左手坐标系，越远越白
		interpolationZ = (interpolationZ + 1.0) * 0.5;
		float zColor = interpolationZ * 255.0;
		color = TGAColor(zColor, zColor, zColor, 255);
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
		model = new Model("obj/african_head/african_head.obj");
		//model = new Model("obj/diablo3_pose/diablo3_pose.obj");
		//model = new Model("obj/test.obj");

	//通过画线可知，这是右手坐标系，从左下开始的（其实从左上开始，被下面flip_vertically改成了左下）
	TGAImage image(width, height, TGAImage::RGB); //纯黑的100 * 100图
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			image.set(i, j, white);
	
	//纹理存深度贴图方式，far平面过远时可能会引发z-Fight，TODO压缩到4个通道
	TGAImage zBuffer(width, height, TGAImage::RGB);
	TGAColor white = TGAColor(255, 255, 255, 255);
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			zBuffer.set(i, j, white);

	//float精度存储方式，用来比较眼睛部位是否是Z-Fight引起的
	float* zBufferFloat = new float[width * height];
	for (int i = 0; i < width * height; i++)
		zBufferFloat[i] = std::numeric_limits<float>::max();//初始zBuffer应该是无限大的值

	//计算Shadowmap
	{
		TGAImage shadowmap(width, height, TGAImage::RGB);
		//使用float数组存储shadowmap计算过程中的zBuffer，取较近的记录在shadowmap上
		shadowmapZBuffer = new float[width * height];
		for (int i = 0; i < width * height; i++)
			shadowmapZBuffer[i] = std::numeric_limits<float>::max();

		//得到算shadowmap的转换矩阵
		World2View(light_dir, lookAtPos, Vec3f(0, 1, 0));//用光源位置来代替摄像机位置
		OrthoProjection(cameraWidth, cameraHeight, cameraNearPlane, cameraFarPlane);
		NDC2ViewPort(width, height);

		//画一遍场景，只为得到shadowmap
		ShadowMapShader shadowmapShader;
		for (int i = 0; i < model->nfaces(); i++)
		{
			//即顶点着色器返回的viewPortCoord，[3]表示一个三角形有3个顶点 vec3表示每个顶点的xyz坐标，xy是屏幕坐标[0,width],z是NDC坐标[-1,1]
			vec3 screen_coords[3];
			for (int j = 0; j < 3; j++)
			{
				screen_coords[j] = shadowmapShader.vertex(i, j);
			}
			//片元着色器，在每个AABB盒子里找每个像素
			triangle(screen_coords, shadowmapShader, shadowmap, shadowmapZBuffer, width);
		}
		shadowmap.flip_vertically();
		shadowmap.write_tga_file("shadowmap.tga");
	}


	//一个像素从世界坐标转换到光源空间的坐标，给正常的shader用
	mat4 world2OrthoProjection_LightMat4 = view2Projection * world2View;
	mat4 ndc2ViewPort_LightMat4 = ndc2ViewPort * world2OrthoProjection_LightMat4;//因为是正交摄像机，所以透视除法这一步骤不要了试下对不对

	//算出our_gl中三个转换矩阵的值
	World2View(cameraPos, lookAtPos, Vec3f(0, 1, 0));
	OrthoProjection(cameraWidth, cameraHeight, cameraNearPlane, cameraFarPlane);
	NDC2ViewPort(width, height);

	PhongShader shader(ndc2ViewPort_LightMat4);
	for (int i = 0; i < model->nfaces(); i++)//遍历faces_容器，里面每个元素代表f中一行，即3个顶点UV法线索引集合
	{
		vec3 screen_coords[3];//[3]表示一个三角形有3个顶点 vec3表示每个顶点的xyz坐标，xy是屏幕坐标[0,width],z是NDC坐标[-1,1]
		for (int j = 0; j < 3; j++)//face中3个顶点对应哪一个
		{
			screen_coords[j] = shader.vertex(i, j);
		}
		//画这个三角形
		//triangle(screen_coords, shader, image, zBufferFloat, width);//使用float数组存储深度
		triangle(screen_coords, shader, image, zBuffer);//使用纹理存储深度
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	zBuffer.flip_vertically();
	zBuffer.write_tga_file("zBuffer.tga");

	delete model;
	delete[] zBufferFloat;
	delete[] shadowmapZBuffer;
	return 0;
}