#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char* filename) :verts_(), faces_(), uvs()
{
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	if (in.fail()) 
		return;
	std::string line;
	while (!in.eof())
	{
		std::getline(in, line);
		std::istringstream iss(line.c_str());
		char trash;
		if (line.compare(0, 2, "v ") == 0)//������0��ʼ������2���ַ�������0��ʾ��ͬ
		{
			iss >> trash;
			Vec3f v;
			for (int i = 0; i < 3; i++) 
				iss >> v.raw[i];
			verts_.push_back(v);//v������Ԫ�أ�verts��ÿһ��Ԫ����3������ÿ��Ԫ�ر�ʾ.obj�е�һ��
		}
		else if (line.compare(0, 3, "vt ") == 0)
		{
			iss >> trash >> trash;//�ֱ���v��t�ַ�
			Vec2f vt;
			for (int i = 0; i < 2; i++)//.obj�ļ���vt�����ж���ո�û��ϵ
				iss >> vt.raw[i];
			uvs.push_back(vt);//uvs��ʾÿһ��vt�ļ���
		}
		else if (line.compare(0, 2, "f ") == 0) 
		{
			//f 24/1/24 25/2/25 26/3/26
			std::vector<int> f;
			int indexVertex, indexUV, indexNormal;
			std::vector<int> faceUV;//����������������ɣ���Ӧ����������������uvֵ
			iss >> trash;//trash = f
			//indexVertex=24 trash=/ indexUV=1 trash=/ indexNormal=24��f��ÿһ��ִ������ѭ��
			while (iss >> indexVertex >> trash >> indexUV >> trash >> indexNormal)
			{
				indexVertex--; // in wavefront obj all indices start at 1, not zero
				f.push_back(indexVertex);//1��Ӧ����vector<Vec3f> verts_�����е�0����
				indexUV--;//����Ҳ�Ǵ�1��ʼ�ۼƵģ��ŵ������д�0��ʼ
				f.push_back(indexUV);//ÿ��f�м������������uv����������f������Ԫ��
			}
			faces_.push_back(f);//f��3��Ԫ�أ����һ���棬faces�����е��漯��,faces��ÿ��Ԫ�ؼ�.obj��ÿһ��
		}
	}
	std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << std::endl;
}

Model::~Model()
{

}

int Model::nverts()
{
	return (int)verts_.size();
}
int Model::nfaces()
{
	return (int)faces_.size();
}
/// <summary>
/// ����face���ṩ������i�����Ҷ��������ж�Ӧ�ĸö���λ��[-1,1]
/// </summary>
/// <param name="i"></param>
/// <returns></returns>
Vec3f Model::vert(int i)
{
	return verts_[i];
}
/// <summary>
/// ����face���ṩ������index���ҵ�uvs�����ж�Ӧ�ĸõ��uvֵ[0,1]
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
Vec2f Model::GetUV(int index)
{
	return uvs[index];
}

/// <summary>
/// idxΪѭ������i��ѭ����������faces�������ҵ�faces������ÿ��face
/// </summary>
/// <param name="idx"></param>
/// <returns></returns>
std::vector<int> Model::face(int idx)
{
	return faces_[idx];
}