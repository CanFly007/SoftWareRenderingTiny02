#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "matrix.h"


const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

Model* model = NULL;
const int width = 600;
const int height = 600;

Vec3f light_dir(0, 0, 1); //��������ϵ����ʾ�ڸõ�Ϊ���Ĺ��գ������Է���
Vec3f cameraPos(0, 0, 0);
Vec3f lookAtPos(0, 0, 0);

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

//���� ��p��������trianglePtr�е���������
Vec3f barycentric(Vec3f* trianglePtr, Vec3f P)
{
	//�������껹�ǲ���
	//Vec3f A = trianglePtr[0];
	//Vec3f B = trianglePtr[1];
	//Vec3f C = trianglePtr[2];
	////��ABΪһ�У�ACΪһ�й��ɵľ������������
	////matrix2x2()
	//Vec3f v0 = C - A;
	//Vec3f v1 = B - A;
	//Vec3f v2 = P - A;
	//float fenmu = (v0 * v0) * (v1 * v1) - (v0 * v1) * (v1 * v0);//���������ǰ��˵ģ�û���Լ��ڱ����ϻ�
	//float u = ((v1 * v1) * (v2 * v0) - (v1 * v0) * (v2 * v1)) / fenmu;
	//float v = ((v0 * v0) * (v2 * v1) - (v0 * v1) * (v2 * v0)) / fenmu;
	//float s = 1 - u - v;
	//if (s < 0 || u < 0 || v < 0)
	//	return Vec3f(-1, -1, -1);
	//return Vec3f(s, u, v);



	Vec3f A = trianglePtr[0];
	Vec3f B = trianglePtr[1];
	Vec3f C = trianglePtr[2];

	Vec3f s[2];
	s[0] = Vec3f(C.x - A.x, B.x - A.x, A.x - P.x);
	s[1] = Vec3f(C.y - A.y, B.y - A.y, A.y - P.y);
	//for (int i = 2; i--; ) 
	//{
	//    s[i].x = C[i] - A[i];
	//    s[i].y = B[i] - A[i];
	//    s[i].z = A[i] - P[i];
	//}
	Vec3f u = s[0] ^ s[1];
	if (std::abs(u.z) > 1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
		return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);//��Ϊu�Ľ����(u,v,1)
	return Vec3f(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizator




	//Vec3f A = trianglePtr[0];
	//Vec3f B = trianglePtr[1];
	//Vec3f C = trianglePtr[2];

	//Vec3f crossABC = (B - A) ^ (C - A); //ABC ABP APC PBC���ǰ���P����������˳ʱ��˳����ϵ�
	//Vec3f crossABP = (B - A) ^ (P - A);
	//Vec3f crossAPC = (P - A) ^ (C - A);
	//Vec3f crossPBC = (B - P) ^ (C - P);

	////����ķ��������P���������⣬�������ͻ�������֮��
	////��ѧ���������õ���жϷ���,ͬ����Ϊ��
	//float outsideAB = crossABC * crossABP;//������Ϊ��������ʾABP����ʱ��ģ�P��AB�����
	//float outsideAC = crossABC * crossAPC;
	//float outsideBC = crossABC * crossPBC;
	//if (outsideAB < 0 || outsideAC < 0 || outsideBC < 0)
	//	return Vec3f(-1, -1, -1);//��������һ���������������ڣ�������������ˣ�Ҳ����������uvwʱ��Ӹ���,AB���������wȨ��Ϊ����

	////�����ģ��ʾ������������Χ�ɵ�ƽ���ı������������������������
	//float areaABC = crossABC.norm() * 0.5; //�����P���������⣬�������ͻ�������֮��
	//float areaABP = crossABP.norm() * 0.5;
	//float areaAPC = crossAPC.norm() * 0.5;
	//float areaPBC = crossPBC.norm() * 0.5;


	//float u = areaPBC / areaABC;
	//float v = areaAPC / areaABC;
	//float w = areaABP / areaABC;
	//return Vec3f(u, v, w);
}

//���������Ļ�ռ�����,����������(trianglePtr)�İ�Χ�С���һ�жϰ�Χ������ÿ�����أ��Ƿ����������ڣ����������жϷ���
void triangle(Vec3f* trianglePtr, Vec2f* triangleUVPtr, Vec3f* normalPtr, float* zBuffer, TGAImage& image, TGAColor color)
{
	Vec3f A = trianglePtr[0];
	Vec3f B = trianglePtr[1];
	Vec3f C = trianglePtr[2];

	Vec2i rectMin = Vec2i(width - 1, height - 1);
	Vec2i rectMax = Vec2i(0, 0);
	for (int i = 0; i < 3; i++)//�����ε�3��������
	{
		rectMin.x = (int)(trianglePtr[i].x < rectMin.x ? trianglePtr[i].x : rectMin.x);
		rectMin.y = (int)(trianglePtr[i].y < rectMin.y ? trianglePtr[i].y : rectMin.y);
		rectMax.x = (int)(trianglePtr[i].x > rectMax.x ? trianglePtr[i].x : rectMax.x);
		rectMax.y = (int)(trianglePtr[i].y > rectMax.y ? trianglePtr[i].y : rectMax.y);
	}

	Vec3f curPixel;//���int���ε�,AABB�ڲ�����
	Vec3f barCoord;//������������������������
	for (int i = (int)rectMin.x; i <= rectMax.x; i++)
	{
		for (int j = (int)rectMin.y; j <= rectMax.y; j++)
		{
			curPixel = Vec3f(i, j, 1);
			barCoord = barycentric(trianglePtr, curPixel);//�����ڸ������ڸ������ε��������꣬�ж������Ƿ�����������
			if (barCoord.x < 0 || barCoord.y < 0 || barCoord.z < 0)
				continue;//AABB������������ص���������С��0��˵������������������⣬����

			//����������������ڣ����ж�������ص���Ȳ���
			//��ͨ�����������ҵ�������ص�z���ֵ
			float z = A.z * barCoord.x + B.z * barCoord.y + C.z * barCoord.z;//���������������ȣ�����������ȡ�ø����ص�����
			if (z > zBuffer[i * width + j])//��������ϵ
			{
				zBuffer[i * width + j] = z;
				//������ص�uvֵ��ͨ�����������������ֱ�����������uvֵ�õ�
				Vec2f uv = triangleUVPtr[0] * barCoord.x + triangleUVPtr[1] * barCoord.y + triangleUVPtr[2] * barCoord.z;
				Vec3f normal = normalPtr[0] * barCoord.x + normalPtr[1] * barCoord.y + normalPtr[2] * barCoord.z;
				normal = (normal + Vec3f(1.0, 1.0, 1.0)) / 2;
				Vec3f lightDir = light_dir.normalize();
				float lambert = normal * lightDir * 255;
				if (lambert < 0)
					lambert = 0;
				TGAColor diffuseColor = model->SamplerDiffseColor(uv);
				//image.set(i, j, diffuseColor);
				image.set(i, j, TGAColor(lambert, lambert, lambert, 1));//ע�͵��������У�ʹ��lambert������color�����������color
			}

			//image.set(i, j, TGAColor(barCoord.x * 255, barCoord.y * 255, barCoord.z * 255, 1));//���ÿ�����ص���������
		}
	}
}

//.obj�ļ��ж���Ķ�������[-1,1]��ת������Ļ����[0,width]��[0,height]��zֵ���䣬��Ȼ��[-1,1]����
Vec3f World2Screen(Vec3f worldPos)
{
	return Vec3f((worldPos.x + 1.0) * 0.5 * width, (worldPos.y + 1.0) * 0.5 * height, worldPos.z);
}

//�����������������ϵ�±�ʾ����uvw���������У�����World->view���������У�����view->World
//��������ʱ�򣬵����п�����view�������������ʾ�µ�ԭ�����꣬���������������У���Ϊԭ�����겻�������ԣ����治������ת�ã�3x3����ת����Ϊ���������Կ���
//�������У�Ҳ����������⣺view��ÿһ����u��v��w������������������൱�������������ֱ���u��v��w��λ�����ϵ�ͶӰ����Ȼ�ͱ䵽��viewSpace
//��Ϊƽ��û�������ԣ���Ҫ������ת��ƽ���������󣬶���һ��������˵���ȷ�����ʩ��ƽ�ƾ���������ת���󣬼�������V * T * ��������
Matrix4x4 World2View(Vec3f cameraPos,Vec3f lookAtPos,Vec3f upDir)
{
	Vec3f z = (cameraPos - lookAtPos).normalize();
	Vec3f x = (upDir ^ z).normalize();
	Vec3f y = (z ^ x).normalize();
	Matrix4x4 viewMat = Matrix4x4::identity();//����ע���е�V������ת��3x3����
	for (int j = 0; j < 3; j++)
	{
		viewMat[0][j] = x[j];//��������
		viewMat[1][j] = y[j];
		viewMat[2][j] = z[j];
	}

	Matrix4x4 translationMat = Matrix4x4::identity();
	for (int i = 0; i < 3; i++)
	{
		translationMat[i][3] = -cameraPos[i];//ƽ�ƾ��󣺵�����Ϊ������
	}

	return viewMat * translationMat;//��ƽ������ת
}
#include <iostream>
int main(int argc, char** argv)
{
	const int aa = 4;
	Matrix<float> m(4, 4);	//Matrix<float>* m = new Matrix<float>(4, 4);
	m.Identity();
	std::cout << m.matrix[0][0] << std::endl;//matrixҪ���private������
	m[0][0] = 3;
	std::cout << m[0][0] << std::endl;//��һ��[]��Matrix�������[]���õ�����T*����ָ�롣�ڶ���[]��Ϊ��T*ָ�룬��������Ҳ������T*

	//Matrix4x4 m = Matrix4x4::identity();
	//Matrix4x4 m1 = Matrix4x4::identity();
	//m[1][1] = 3.6;
	//Matrix4x4 result = m * m1;
	//std::cout << result[1][1] << std::endl;
	return 1;

	if (2 == argc)
		model = new Model(argv[1]);
	else
		model = new Model("obj/african_head.obj");

	//ͨ�����߿�֪��������������ϵ�������¿�ʼ�ģ���ʵ�����Ͽ�ʼ��������flip_vertically�ĳ������£�
	TGAImage image(width, height, TGAImage::RGB); //���ڵ�100 * 100ͼ
	
	float* zBuffer = new float[width * height];
	for (int i = 0; i < width * height; i++)
		zBuffer[i] = -std::numeric_limits<float>::max();

	Matrix4x4 world2View = World2View(cameraPos, lookAtPos, Vec3f(0, 1, 0));

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

		v0 = (world2View * Matrix4x1(v0)).ToVec3();
		v1 = (world2View * Matrix4x1(v1)).ToVec3();
		v2 = (world2View * Matrix4x1(v2)).ToVec3();

		//ת����[0,width] [0,height]��Ļ����
		Vec3f v0screenCoord = World2Screen(v0);
		Vec3f v1screenCoord = World2Screen(v1);
		Vec3f v2screenCoord = World2Screen(v2);
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
			zBuffer, image, TGAColor(lambert, lambert, lambert, 1));//����zBuffer��⣬lambert>0����������ȥ����
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}