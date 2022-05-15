#include "our_gl.h"

mat4 world2View;
mat4 view2Projection;
mat4 ndc2ViewPort;

IShader::~IShader() {}

//�����������������ϵ�±�ʾ����uvw���������У�����World->view���������У�����view->World
//��������ʱ�򣬵����п�����view�������������ʾ�µ�ԭ�����꣬���������������У���Ϊԭ�����겻�������ԣ����治������ת�ã�3x3����ת����Ϊ���������Կ���
//�������У�Ҳ����������⣺view��ÿһ����u��v��w������������������൱�������������ֱ���u��v��w��λ�����ϵ�ͶӰ����Ȼ�ͱ䵽��viewSpace
//��Ϊƽ��û�������ԣ���Ҫ������ת��ƽ���������󣬶���һ��������˵���ȷ�����ʩ��ƽ�ƾ���������ת���󣬼�������V * T * ��������
void World2View(Vec3f cameraPos, Vec3f lookAtPos, Vec3f upDir)
{
	Vec3f z = (cameraPos - lookAtPos).normalize();
	Vec3f x = (upDir ^ z).normalize();
	Vec3f y = (z ^ x).normalize();
	mat4 viewMat = mat4::identity();//����ע���е�V������ת��3x3��
	for (int j = 0; j < 3; j++)
	{
		viewMat[0][j] = x[j];//��������
		viewMat[1][j] = y[j];
		viewMat[2][j] = z[j];
	}

	mat4 translationMat = mat4::identity();
	for (int i = 0; i < 3; i++)
	{
		translationMat[i][3] = -cameraPos[i];//ƽ�ƾ��󣺵�����Ϊ������
	}

	world2View = viewMat * translationMat;//��ƽ������ת
}
//viewSpace�任�������ü��ռ�(��ClipSpace)
void OrthoProjection(float cameraWidth, float cameraHeight, float cameraNearPlane, float cameraFarPlane)
{
	view2Projection = mat4::identity();
	view2Projection[0][0] = 2.0 / cameraWidth;
	view2Projection[1][1] = 2.0 / cameraHeight;
	view2Projection[2][2] = 2.0 / (cameraNearPlane - cameraFarPlane);
	view2Projection[2][3] = (cameraNearPlane + cameraFarPlane) / (cameraNearPlane - cameraFarPlane);
}
//�ӿڱ任��[-1,1]�䵽��Ļ��[0,width],z���䣬����[-1,1]
void NDC2ViewPort(float screenWidth, float screenHeight)
{
	//return Vec3f((worldPos.x + 1.0) * 0.5 * width, (worldPos.y + 1.0) * 0.5 * height, worldPos.z);
	ndc2ViewPort = mat4::identity();
	ndc2ViewPort[0][0] = screenWidth / 2.0;
	ndc2ViewPort[0][3] = screenWidth / 2.0;
	ndc2ViewPort[1][1] = screenHeight / 2.0;
	ndc2ViewPort[1][3] = screenHeight / 2.0;
}
//���� ��p��������trianglePtr�е���������
Vec3f barycentric(Vec3f* trianglePtr, Vec3f P)
{
	//�������껹�ǲ��� ֻ���������ṩ�ķ�������ȷ��
	//Vec3f A = trianglePtr[0];
	//Vec3f B = trianglePtr[1];
	//Vec3f C = trianglePtr[2];
	////��ABΪһ�У�ACΪһ�й��ɵľ������������
	////matrix2x2()
	//Vec3f v0 = C - A;
	//Vec3f v1 = B - A;
	//Vec3f v2 = P - A;

	//float dot00 = v0 * v0;//�������ǰ��˵ģ�û���Լ��ڱ����ϻ�
	//float dot01 = v0 * v1;
	//float dot02 = v0 * v2;
	//float dot11 = v1 * v1;
	//float dot12 = v1 * v2;
	//float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	//float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	//float v = (dot00 * dot12 - dot01 * dot02) * invDenom;
	//if (u < 0 || v < 0 || u + v >= 1)
	//	return Vec3f(-1, -1, -1);
	//return Vec3f(1.0 - u - v, u, v);

	Vec3f A = trianglePtr[0];
	Vec3f B = trianglePtr[1];
	Vec3f C = trianglePtr[2];

	Vec3f s[2];
	s[0] = Vec3f(C.x - A.x, B.x - A.x, A.x - P.x);
	s[1] = Vec3f(C.y - A.y, B.y - A.y, A.y - P.y);

	Vec3f u = s[0] ^ s[1];
	if (std::abs(u.z) > 1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
		return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);//��Ϊu�Ľ����(u,v,1)
	return Vec3f(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

//���������Ļ�ռ�����,����������(trianglePtr)�İ�Χ�С���һ�жϰ�Χ������ÿ�����أ��Ƿ����������ڣ����������жϷ���
//ÿ��Ҫ�������ص���shader.fragment
void triangle(vec3* trianglePtr, IShader& shader, TGAImage& image, float* zBuffer, const int width)
{
	vec3 A = trianglePtr[0];
	vec3 B = trianglePtr[1];
	vec3 C = trianglePtr[2];

	Vec2i rectMin = Vec2i(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
	Vec2i rectMax = Vec2i(0,0);
	for (int i = 0; i < 3; i++)
	{
		rectMin.x = (int)(trianglePtr[i].x() < rectMin.x ? trianglePtr[i].x() : rectMin.x);
		rectMin.y = (int)(trianglePtr[i].y() < rectMin.y ? trianglePtr[i].y() : rectMin.y);
		rectMax.x = (int)(trianglePtr[i].x() > rectMax.x ? trianglePtr[i].x() : rectMax.x);
		rectMax.y = (int)(trianglePtr[i].y() > rectMax.y ? trianglePtr[i].y() : rectMax.y);
	}

	Vec3f curPixel;//���int���ε�,AABB�ڲ�����
	Vec3f barCoord;//������������������������
	Vec3f trianglePtrV3f[3] = { Vec3f(A[0],A[1],A[2]),Vec3f(B[0],B[1],B[2]),Vec3f(C[0],C[1],C[2]) };//vec3 -> Vec3f
	for (int i = rectMin.x; i <= rectMax.x; i++)
	{
		for (int j = rectMin.y; j <= rectMax.y; j++)
		{
			curPixel = Vec3f(i, j, 1.0);//���������㲻�ԣ��ѵ��������z=1��
			barCoord = barycentric(trianglePtrV3f, curPixel);//������ص���������
			if (barCoord.x < 0 || barCoord.y < 0 || barCoord.z < 0)
				continue;//AABB������������ص���������С��0��˵������������������⣬����

			float z = A.z() * barCoord[0] + B.z() * barCoord[1] + C.z() * barCoord[2];
			if (z < zBuffer[j + width * i])//��Ȳ��ԣ��ǵģ������������ϵ��ԽԶzԽ��
			{
				zBuffer[j + width * i] = z;
				TGAColor gl_Color;//����fragment���ص�������ɫ
				bool clip = shader.fragment(barCoord, gl_Color);
				image.set(i, j, gl_Color);
			}
		}
	}
}
//ͨ���������ʽ�ȽϺʹ洢�������
void triangle(vec3* trianglePtr, IShader& shader, TGAImage& image, TGAImage& zBuffer)
{
	vec3 A = trianglePtr[0];
	vec3 B = trianglePtr[1];
	vec3 C = trianglePtr[2];

	Vec2i rectMin = Vec2i(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
	Vec2i rectMax = Vec2i(0, 0);
	for (int i = 0; i < 3; i++)
	{
		rectMin.x = (int)(trianglePtr[i].x() < rectMin.x ? trianglePtr[i].x() : rectMin.x);
		rectMin.y = (int)(trianglePtr[i].y() < rectMin.y ? trianglePtr[i].y() : rectMin.y);
		rectMax.x = (int)(trianglePtr[i].x() > rectMax.x ? trianglePtr[i].x() : rectMax.x);
		rectMax.y = (int)(trianglePtr[i].y() > rectMax.y ? trianglePtr[i].y() : rectMax.y);
	}

	Vec3f curPixel;//���int���ε�,AABB�ڲ�����
	Vec3f barCoord;//������������������������
	Vec3f trianglePtrV3f[3] = { Vec3f(A[0],A[1],A[2]),Vec3f(B[0],B[1],B[2]),Vec3f(C[0],C[1],C[2]) };//vec3 -> Vec3f
	for (int i = rectMin.x; i <= rectMax.x; i++)
	{
		for (int j = rectMin.y; j <= rectMax.y; j++)
		{
			curPixel = Vec3f(i, j, 1.0);//���������㲻�ԣ��ѵ��������z=1��
			barCoord = barycentric(trianglePtrV3f, curPixel);//������ص���������
			if (barCoord.x < 0 || barCoord.y < 0 || barCoord.z < 0)
				continue;//AABB������������ص���������С��0��˵������������������⣬����

			float z = A.z() * barCoord[0] + B.z() * barCoord[1] + C.z() * barCoord[2];
			//z�ķ�Χ��[-1,1]�任��[0,255]
			z = (z + 1) * 0.5 * 255.0;
			if (z < zBuffer.get(i, j).r)//��Ȳ��ԣ��ǵģ������������ϵ��ԽԶzԽ��
			{
				zBuffer.set(i, j, TGAColor(z, 0, 0, 255));
				TGAColor gl_Color;//����fragment���ص�������ɫ
				bool clip = shader.fragment(barCoord, gl_Color);
				image.set(i, j, gl_Color);
			}
		}
	}
}


//���������Ļ�ռ�����,����������(trianglePtr)�İ�Χ�С���һ�жϰ�Χ������ÿ�����أ��Ƿ����������ڣ����������жϷ���
void triangle(Vec3f* trianglePtr, Vec2f* triangleUVPtr, Vec3f* normalPtr, float* zBuffer, TGAImage& image, TGAColor color, const int width, const int height, Vec3f light_dir)
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
				//TGAColor diffuseColor = model->SamplerDiffseColor(uv);
				//image.set(i, j, diffuseColor);
				//image.set(i, j, TGAColor(lambert, lambert, lambert, 1));//ע�͵��������У�ʹ��lambert������color�����������color
				image.set(i, j, color);
			}

			//image.set(i, j, TGAColor(barCoord.x * 255, barCoord.y * 255, barCoord.z * 255, 1));//���ÿ�����ص���������
		}
	}
}
