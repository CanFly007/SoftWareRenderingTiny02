#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char* filename) :verts_(), faces_()
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
		else if (line.compare(0, 2, "f ") == 0) 
		{
			//f 24/1/24 25/2/25 26/3/26
			std::vector<int> f;
			int itrash, idx;
			iss >> trash;//trash: f
			while (iss >> idx >> trash >> itrash >> trash >> itrash)//idx:24 trash:/ itrash:1 trash:/ itrash:24 ÿ��ѭ��ֻҪ��һ��idx
			{
				idx--; // in wavefront obj all indices start at 1, not zero
				f.push_back(idx);//1��Ӧ����vector<Vec3f> verts_�����е�0����
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
Vec3f Model::vert(int i)
{
	return verts_[i];
}
std::vector<int> Model::face(int idx)
{
	return faces_[idx];
}