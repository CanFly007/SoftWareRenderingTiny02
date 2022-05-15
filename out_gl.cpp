#include "our_gl.h"

mat4 world2View;
mat4 view2Projection;
mat4 ndc2ViewPort;

IShader::~IShader() {}

//把摄像机在世界坐标系下表示的轴uvw，按行排列，就是World->view。按列排列，就是view->World
//按列排列时候，第四列可以是view在世界坐标轴表示下的原点坐标，但反过来按行则不行，因为原点坐标不是正交性，求逆不等于求转置，3x3的旋转轴因为正交性所以可以
//按行排列，也可以这样理解：view的每一个轴u、v、w和世界坐标做点积，相当于这个世界坐标分别在u、v、w单位向量上的投影，当然就变到的viewSpace
//因为平移没有正交性，需要两个旋转和平移两个矩阵，对于一个物体来说，先反方向施加平移矩阵，再做旋转矩阵，即矩阵是V * T * 物体坐标
void World2View(Vec3f cameraPos, Vec3f lookAtPos, Vec3f upDir)
{
	Vec3f z = (cameraPos - lookAtPos).normalize();
	Vec3f x = (upDir ^ z).normalize();
	Vec3f y = (z ^ x).normalize();
	mat4 viewMat = mat4::identity();//上面注释中的V，即旋转的3x3矩
	for (int j = 0; j < 3; j++)
	{
		viewMat[0][j] = x[j];//按行排列
		viewMat[1][j] = y[j];
		viewMat[2][j] = z[j];
	}

	mat4 translationMat = mat4::identity();
	for (int i = 0; i < 3; i++)
	{
		translationMat[i][3] = -cameraPos[i];//平移矩阵：第四列为负方向
	}

	world2View = viewMat * translationMat;//先平移再旋转
}
//viewSpace变换到正交裁剪空间(即ClipSpace)
void OrthoProjection(float cameraWidth, float cameraHeight, float cameraNearPlane, float cameraFarPlane)
{
	view2Projection = mat4::identity();
	view2Projection[0][0] = 2.0 / cameraWidth;
	view2Projection[1][1] = 2.0 / cameraHeight;
	view2Projection[2][2] = 2.0 / (cameraNearPlane - cameraFarPlane);
	view2Projection[2][3] = (cameraNearPlane + cameraFarPlane) / (cameraNearPlane - cameraFarPlane);
}
//视口变换，[-1,1]变到屏幕的[0,width],z不变，还是[-1,1]
void NDC2ViewPort(float screenWidth, float screenHeight)
{
	//return Vec3f((worldPos.x + 1.0) * 0.5 * width, (worldPos.y + 1.0) * 0.5 * height, worldPos.z);
	ndc2ViewPort = mat4::identity();
	ndc2ViewPort[0][0] = screenWidth / 2.0;
	ndc2ViewPort[0][3] = screenWidth / 2.0;
	ndc2ViewPort[1][1] = screenHeight / 2.0;
	ndc2ViewPort[1][3] = screenHeight / 2.0;
}
//返回 点p在三角形trianglePtr中的重心坐标
Vec3f barycentric(Vec3f* trianglePtr, Vec3f P)
{
	//重心坐标还是不对 只能用作者提供的方法是正确的
	//Vec3f A = trianglePtr[0];
	//Vec3f B = trianglePtr[1];
	//Vec3f C = trianglePtr[2];
	////求AB为一列，AC为一列构成的矩阵，他的逆矩阵
	////matrix2x2()
	//Vec3f v0 = C - A;
	//Vec3f v1 = B - A;
	//Vec3f v2 = P - A;

	//float dot00 = v0 * v0;//下面行是搬运的，没有自己在本子上画
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
		return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);//因为u的结果是(u,v,1)
	return Vec3f(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

//传入的是屏幕空间坐标,构建三角形(trianglePtr)的包围盒。逐一判断包围盒里面每个像素，是否在三角形内（重心坐标判断法）
//每个要画的像素调用shader.fragment
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

	Vec3f curPixel;//设成int传参的,AABB内部填充的
	Vec3f barCoord;//这个像素在三个点的重心坐标
	Vec3f trianglePtrV3f[3] = { Vec3f(A[0],A[1],A[2]),Vec3f(B[0],B[1],B[2]),Vec3f(C[0],C[1],C[2]) };//vec3 -> Vec3f
	for (int i = rectMin.x; i <= rectMax.x; i++)
	{
		for (int j = rectMin.y; j <= rectMax.y; j++)
		{
			curPixel = Vec3f(i, j, 1.0);//重心坐标算不对？难道问题出在z=1？
			barCoord = barycentric(trianglePtrV3f, curPixel);//这个像素的重心坐标
			if (barCoord.x < 0 || barCoord.y < 0 || barCoord.z < 0)
				continue;//AABB盒子里这个像素的重心坐标小于0，说明这个像素在三角形外，不画

			float z = A.z() * barCoord[0] + B.z() * barCoord[1] + C.z() * barCoord[2];
			if (z < zBuffer[j + width * i])//深度测试，是的！变成左手坐标系，越远z越大
			{
				zBuffer[j + width * i] = z;
				TGAColor gl_Color;//接收fragment传回的最终颜色
				bool clip = shader.fragment(barCoord, gl_Color);
				image.set(i, j, gl_Color);
			}
		}
	}
}
//通过深度纹理方式比较和存储深度纹理
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

	Vec3f curPixel;//设成int传参的,AABB内部填充的
	Vec3f barCoord;//这个像素在三个点的重心坐标
	Vec3f trianglePtrV3f[3] = { Vec3f(A[0],A[1],A[2]),Vec3f(B[0],B[1],B[2]),Vec3f(C[0],C[1],C[2]) };//vec3 -> Vec3f
	for (int i = rectMin.x; i <= rectMax.x; i++)
	{
		for (int j = rectMin.y; j <= rectMax.y; j++)
		{
			curPixel = Vec3f(i, j, 1.0);//重心坐标算不对？难道问题出在z=1？
			barCoord = barycentric(trianglePtrV3f, curPixel);//这个像素的重心坐标
			if (barCoord.x < 0 || barCoord.y < 0 || barCoord.z < 0)
				continue;//AABB盒子里这个像素的重心坐标小于0，说明这个像素在三角形外，不画

			float z = A.z() * barCoord[0] + B.z() * barCoord[1] + C.z() * barCoord[2];
			//z的范围从[-1,1]变换到[0,255]
			z = (z + 1) * 0.5 * 255.0;
			if (z < zBuffer.get(i, j).r)//深度测试，是的！变成左手坐标系，越远z越大
			{
				zBuffer.set(i, j, TGAColor(z, 0, 0, 255));
				TGAColor gl_Color;//接收fragment传回的最终颜色
				bool clip = shader.fragment(barCoord, gl_Color);
				image.set(i, j, gl_Color);
			}
		}
	}
}


//传入的是屏幕空间坐标,构建三角形(trianglePtr)的包围盒。逐一判断包围盒里面每个像素，是否在三角形内（重心坐标判断法）
void triangle(Vec3f* trianglePtr, Vec2f* triangleUVPtr, Vec3f* normalPtr, float* zBuffer, TGAImage& image, TGAColor color, const int width, const int height, Vec3f light_dir)
{
	Vec3f A = trianglePtr[0];
	Vec3f B = trianglePtr[1];
	Vec3f C = trianglePtr[2];

	Vec2i rectMin = Vec2i(width - 1, height - 1);
	Vec2i rectMax = Vec2i(0, 0);
	for (int i = 0; i < 3; i++)//三角形的3个点依次
	{
		rectMin.x = (int)(trianglePtr[i].x < rectMin.x ? trianglePtr[i].x : rectMin.x);
		rectMin.y = (int)(trianglePtr[i].y < rectMin.y ? trianglePtr[i].y : rectMin.y);
		rectMax.x = (int)(trianglePtr[i].x > rectMax.x ? trianglePtr[i].x : rectMax.x);
		rectMax.y = (int)(trianglePtr[i].y > rectMax.y ? trianglePtr[i].y : rectMax.y);
	}

	Vec3f curPixel;//设成int传参的,AABB内部填充的
	Vec3f barCoord;//这个像素在三个点的重心坐标
	for (int i = (int)rectMin.x; i <= rectMax.x; i++)
	{
		for (int j = (int)rectMin.y; j <= rectMax.y; j++)
		{
			curPixel = Vec3f(i, j, 1);
			barCoord = barycentric(trianglePtr, curPixel);//盒子内该像素在该三角形的重心坐标，判断像素是否在三角形内
			if (barCoord.x < 0 || barCoord.y < 0 || barCoord.z < 0)
				continue;//AABB盒子里这个像素的重心坐标小于0，说明这个像素在三角形外，不画

			//这个像素在三角形内，再判断这个像素的深度测试
			//先通过重心坐标找到这个像素的z深度值
			float z = A.z * barCoord.x + B.z * barCoord.y + C.z * barCoord.z;//三角形三个点的深度，按重心坐标取得该像素点的深度
			if (z > zBuffer[i * width + j])//右手坐标系
			{
				zBuffer[i * width + j] = z;
				//这个像素的uv值，通过三个点的重心坐标分别乘以三个点的uv值得到
				Vec2f uv = triangleUVPtr[0] * barCoord.x + triangleUVPtr[1] * barCoord.y + triangleUVPtr[2] * barCoord.z;
				Vec3f normal = normalPtr[0] * barCoord.x + normalPtr[1] * barCoord.y + normalPtr[2] * barCoord.z;
				normal = (normal + Vec3f(1.0, 1.0, 1.0)) / 2;
				Vec3f lightDir = light_dir.normalize();
				float lambert = normal * lightDir * 255;
				if (lambert < 0)
					lambert = 0;
				//TGAColor diffuseColor = model->SamplerDiffseColor(uv);
				//image.set(i, j, diffuseColor);
				//image.set(i, j, TGAColor(lambert, lambert, lambert, 1));//注释掉上面这行，使用lambert传来的color来算整个面的color
				image.set(i, j, color);
			}

			//image.set(i, j, TGAColor(barCoord.x * 255, barCoord.y * 255, barCoord.z * 255, 1));//输出每个像素的重心坐标
		}
	}
}
