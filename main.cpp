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

Vec3f light_dir(1, 1, 0); //��������ϵ����ʾ�ڸõ�Ϊ���Ĺ��գ������Է���
Vec3f cameraPos(2, 0, 3); //obj��ԭ�㣬���������ԭ����ܿ�������
//Vec3f cameraPos(0, 0, 3);
Vec3f lookAtPos(0, 0, 0);

//�������
float cameraWidth = 2.5;
float cameraHeight = 2.5;
float cameraFarPlane = 15.0;
float cameraNearPlane = 0.1;

float* shadowmapZBuffer = NULL;

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
	Vec2f varying_uv[3];//������ֱ��uvֵ

	Vec3f varying_WorldPos[3];
	Vec3f varying_WorldNormal[3];

	mat4 uniform_World2LightMatrix;

	PhongShader(mat4 world2LightMatrix) :uniform_World2LightMatrix(world2LightMatrix) {}

	//iface�Ǵ�0������faces_���nthvert����ѭ������Χ��[0,3)
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
		//�����ü�֮��xyz������Ҫ��һ��͸�ӳ���������w
		vec4 NDC = vec4(gl_Position[0] / gl_Position[3], gl_Position[1] / gl_Position[3], gl_Position[2] / gl_Position[3], gl_Position[3]);
		vec4 viewPort = ndc2ViewPort * NDC;
		return vec3(viewPort[0],viewPort[1],viewPort[2]);
	}
	virtual bool fragment(Vec3f bar, TGAColor& color)//barָ������ص���������};
	{
		float lambert;
		lambert = varying_intensity * bar;//varying_intensity��һ��Vec3f�������������ֱ��lambertֵ���ø��������������ӦABCȨ�طֱ���Զ�Ӧ���intensity
		Vec2f uv = varying_uv[0] * bar[0] + varying_uv[1] * bar[1] + varying_uv[2] * bar[2];
		Vec3f worldPos = varying_WorldPos[0] * bar[0] + varying_WorldPos[1] * bar[1] + varying_WorldPos[2] * bar[2];
		
		Vec3f worldNormal = varying_WorldNormal[0] * bar[0] + varying_WorldNormal[1] * bar[1] + varying_WorldNormal[2] * bar[2];//�����������Դ����ߵ�����Ȩ����� 
		worldNormal.normalize();

		//����������ֵ������һ���ǲ�ֵ���������float���ݣ�һ���ǲ�ֵ���������uv���������α����׶��Ѿ���Ӳ����ɣ�������fragment�����
		
		//��Ӱ
		vec4 worldPosHomogeneous = vec4(worldPos.x, worldPos.y, worldPos.z, 1.0);
		vec4 lightSpaceCoords = uniform_World2LightMatrix * worldPosHomogeneous;//xy��[0,width] z��[-1,1]
		//����ͨ��shadowmapZBuffer������ң�������shadowmap��ͼ���ҡ�����z�ķ�Χ����[-1,1]��������ϵ
		int index = int(lightSpaceCoords.x()) + int(lightSpaceCoords.y()) * width;
		float shadow = shadowmapZBuffer[index] > lightSpaceCoords.z();

#pragma region ���߼��㣬Ȼ�󹹽��ռ��㷨
		//1������TBN����ϵ��������ռ��ʾTBN������Ȼ�������м��õ� TangentSpace -> WorldSpace �ľ���
		Vec3f AB = varying_WorldPos[1] - varying_WorldPos[0];
		Vec3f AC = varying_WorldPos[2] - varying_WorldPos[0];
		float deltaU0 = varying_uv[1].u - varying_uv[0].u;//AB������U��V��
		float deltaV0 = varying_uv[1].v - varying_uv[0].v;
		float deltaU1 = varying_uv[2].u - varying_uv[0].u;//AC����U��V�Ĳ�
		float deltaV1 = varying_uv[2].v - varying_uv[0].v;

		float determinant = 1.0 / (deltaU0 * deltaV1 - deltaV0 * deltaU1);//���ӡ��ʼ����߿ռ�
		Vec3f worldSpaceT = determinant * (deltaV1 * AB - deltaV0 * AC);//����˳��Ҳ�����ˣ�����
		Vec3f worldSpaceB = determinant * (-deltaU1 * AB + deltaU0 * AC);

		//Tangent��Binormal�Ĺ�һ����ʽ��֪Ϊ�Σ����ӡ��ʼ�
		worldSpaceT = worldSpaceT - (worldSpaceT * worldNormal) * worldNormal;
		worldSpaceT.normalize();//һ���ǵù�һ��
		worldSpaceB = worldSpaceB - (worldSpaceB * worldNormal) * worldNormal - (worldSpaceB * worldSpaceT) * worldSpaceT;
		worldSpaceB.normalize();

		//����ͨ��TB��ˣ�Ҳ�����ø����ص�normalֵ����������normal������Ȩ�أ�,���űȽ������Ƿ�һ�£�
		Vec3f worldNormalDebug = worldNormal.normalize();
		Vec3f worldNormalCross = (worldSpaceT ^ worldSpaceB).normalize();//Debug���Ȳ��һ��

		//2��TBN�����ţ�����Tangent2World����
		mat3 tangent2World;
		tangent2World[0][0] = worldSpaceT.x; tangent2World[0][1] = worldSpaceB.x; tangent2World[0][2] = worldNormalCross.x;
		tangent2World[1][0] = worldSpaceT.y; tangent2World[1][1] = worldSpaceB.y; tangent2World[1][2] = worldNormalCross.y;
		tangent2World[2][0] = worldSpaceT.z; tangent2World[2][1] = worldSpaceB.z; tangent2World[2][2] = worldNormalCross.z;

		TGAColor normalMap = model->SamplerNormalColor(uv);
		//������������normalMap��ͼ�����������߿ռ������ʾ��
		//Unity�е�UnpackNormal�����ǰ�[0,1]���[-1,1]�������һ����[0,255]���[0,1]
		//���߿ռ�����ǶԵģ���������������⣬TGAColor�Ĺ��캯��˳����b g r��Ҫ�ߵ�һ�£�����
		vec3 tangentNormal = vec3((float)normalMap.raw[2] / 255.0, (float)normalMap.raw[1] / 255.0, (float)normalMap.raw[0] / 255.0);
		tangentNormal = tangentNormal * 2.0 - 1.0;
		////tangentNormal.normalize();
		vec3 convertToWorldNormal = tangent2World * tangentNormal;//convertToWorldNormal�ǰ�normalMapת��������ռ�ķ��ߣ��������worldNormal���㷨�߲�ͬ�������worldNormalһ���ڶ�����ɫ�����㣬Ȼ���ֵ����ƬԪ
		Vec3f normal = Vec3f(convertToWorldNormal[0], convertToWorldNormal[1], convertToWorldNormal[2]);
		normal.normalize();
#pragma endregion

		//������������normalMap��ͼ�����������������ʾ�ģ������߿ռ䣩
		//Vec3f normal = Vec3f((normalMap.r / 255.0) * 2 - 1, (normalMap.g / 255.0) * 2 - 1, (normalMap.b / 255.0) * 2 - 1);//[0,255]->[-1,1]
		//normal = normal.normalize();

		//����ռ�worldNormal * worldLightDir
		float a0 = light_dir[0];
		float a1 = light_dir[1];
		float a2 = light_dir[2];
		Vec3f worldLightDir = Vec3f(a0,a1,a2);//.normalize();
		float ndotL = std::max(0.f, normal * worldLightDir);

		TGAColor albedo = model->SamplerDiffseColor(uv);
		Vec3f diffuse = Vec3f(albedo.r * ndotL, albedo.g * ndotL, albedo.b * ndotL);
		//lambert = ndotL;

#pragma region �߹�Specular
		TGAColor specMap = model->SamplerSpcularColor(uv);//����ͼ��ɸ߹��ݵ�ͼ
		float gloss = specMap.r;
		//Vec3f test1Dir = Vec3f(cameraPos.x, cameraPos.y, cameraPos.z);
		Vec3f worldViewDir = cameraPos - worldPos;
		worldViewDir.normalize();
		Vec3f reflectDir = -worldLightDir + normal * (worldLightDir * normal) * 2;
		reflectDir.normalize();
		float rdotV = reflectDir * worldViewDir;
		Vec3f specularColor = Vec3f(1, 1, 1);//�߹���ɫ
		//������ף�gloss���ݡ�����xƽ�����ߣ�������x���η����ߣ���ֻ�е����䷽��͹۲췽��ܿ���ʱ������ӽ�1����256�ݴη�����ſ���1�������ݴη���ֵ��С��
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

struct ShadowMapShader :IShader //��Դ��shadowmapͼ
{
	float viewPortZ[3];//z����ͼ�ռ䷶Χ��[-1,1]

	virtual vec3 vertex(int iface, int nthvert)//������ÿ���������һ�Σ�ÿ�������ε�������
	{
		Vec3f pos = model->vertPos(iface, nthvert);
		vec4 pos_homogeneous = vec4(pos[0], pos[1], pos[2], 1.0);
		vec4 gl_Position = view2Projection * world2View * pos_homogeneous;
		//͸�ӳ������[-1,1]
		vec4 NDC = vec4(gl_Position[0] / gl_Position[3], gl_Position[1] / gl_Position[3], gl_Position[2] / gl_Position[3], gl_Position[3]);
		vec4 viewPort = ndc2ViewPort * NDC;
		viewPortZ[nthvert] = viewPort[2];
		return vec3(viewPort[0], viewPort[1], viewPort[2]);
	}
	virtual bool fragment(Vec3f bar, TGAColor& color)//AABB���������������ڲ���ÿ�����ص���һ��fragment
	{
		float interpolationZ = bar[0] * viewPortZ[0] + bar[1] * viewPortZ[1] + bar[2] * viewPortZ[2];
		//��[-1,1] -> [0,1] ��������ϵ��ԽԶԽ��
		interpolationZ = (interpolationZ + 1.0) * 0.5;
		float zColor = interpolationZ * 255.0;
		color = TGAColor(zColor, zColor, zColor, 255);
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
		model = new Model("obj/african_head/african_head.obj");
		//model = new Model("obj/diablo3_pose/diablo3_pose.obj");
		//model = new Model("obj/test.obj");

	//ͨ�����߿�֪��������������ϵ�������¿�ʼ�ģ���ʵ�����Ͽ�ʼ��������flip_vertically�ĳ������£�
	TGAImage image(width, height, TGAImage::RGB); //���ڵ�100 * 100ͼ
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			image.set(i, j, white);
	
	//����������ͼ��ʽ��farƽ���Զʱ���ܻ�����z-Fight��TODOѹ����4��ͨ��
	TGAImage zBuffer(width, height, TGAImage::RGB);
	TGAColor white = TGAColor(255, 255, 255, 255);
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
			zBuffer.set(i, j, white);

	//float���ȴ洢��ʽ�������Ƚ��۾���λ�Ƿ���Z-Fight�����
	float* zBufferFloat = new float[width * height];
	for (int i = 0; i < width * height; i++)
		zBufferFloat[i] = std::numeric_limits<float>::max();//��ʼzBufferӦ�������޴��ֵ

	//����Shadowmap
	{
		TGAImage shadowmap(width, height, TGAImage::RGB);
		//ʹ��float����洢shadowmap��������е�zBuffer��ȡ�Ͻ��ļ�¼��shadowmap��
		shadowmapZBuffer = new float[width * height];
		for (int i = 0; i < width * height; i++)
			shadowmapZBuffer[i] = std::numeric_limits<float>::max();

		//�õ���shadowmap��ת������
		World2View(light_dir, lookAtPos, Vec3f(0, 1, 0));//�ù�Դλ�������������λ��
		OrthoProjection(cameraWidth, cameraHeight, cameraNearPlane, cameraFarPlane);
		NDC2ViewPort(width, height);

		//��һ�鳡����ֻΪ�õ�shadowmap
		ShadowMapShader shadowmapShader;
		for (int i = 0; i < model->nfaces(); i++)
		{
			//��������ɫ�����ص�viewPortCoord��[3]��ʾһ����������3������ vec3��ʾÿ�������xyz���꣬xy����Ļ����[0,width],z��NDC����[-1,1]
			vec3 screen_coords[3];
			for (int j = 0; j < 3; j++)
			{
				screen_coords[j] = shadowmapShader.vertex(i, j);
			}
			//ƬԪ��ɫ������ÿ��AABB��������ÿ������
			triangle(screen_coords, shadowmapShader, shadowmap, shadowmapZBuffer, width);
		}
		shadowmap.flip_vertically();
		shadowmap.write_tga_file("shadowmap.tga");
	}


	//һ�����ش���������ת������Դ�ռ�����꣬��������shader��
	mat4 world2OrthoProjection_LightMat4 = view2Projection * world2View;
	mat4 ndc2ViewPort_LightMat4 = ndc2ViewPort * world2OrthoProjection_LightMat4;//��Ϊ�����������������͸�ӳ�����һ���費Ҫ�����¶Բ���

	//���our_gl������ת�������ֵ
	World2View(cameraPos, lookAtPos, Vec3f(0, 1, 0));
	OrthoProjection(cameraWidth, cameraHeight, cameraNearPlane, cameraFarPlane);
	NDC2ViewPort(width, height);

	PhongShader shader(ndc2ViewPort_LightMat4);
	for (int i = 0; i < model->nfaces(); i++)//����faces_����������ÿ��Ԫ�ش���f��һ�У���3������UV������������
	{
		vec3 screen_coords[3];//[3]��ʾһ����������3������ vec3��ʾÿ�������xyz���꣬xy����Ļ����[0,width],z��NDC����[-1,1]
		for (int j = 0; j < 3; j++)//face��3�������Ӧ��һ��
		{
			screen_coords[j] = shader.vertex(i, j);
		}
		//�����������
		//triangle(screen_coords, shader, image, zBufferFloat, width);//ʹ��float����洢���
		triangle(screen_coords, shader, image, zBuffer);//ʹ������洢���
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