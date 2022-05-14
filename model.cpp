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
		if (line.compare(0, 2, "v ") == 0)//从索引0开始，包含2个字符，等于0表示相同
		{
			iss >> trash;
			Vec3f v;
			for (int i = 0; i < 3; i++) 
				iss >> v.raw[i];
			verts_.push_back(v);//v是三个元素，verts的每一个元素是3个，即每个元素表示.obj中的一行
		}
		else if (line.compare(0, 3, "vt ") == 0)
		{
			iss >> trash >> trash;//分别是v和t字符
			Vec2f vt;
			for (int i = 0; i < 2; i++)//.obj文件中vt后面有多个空格，没关系
				iss >> vt.raw[i];
			uvs.push_back(vt);//uvs表示每一行vt的集合
		}
		else if (line.compare(0, 3, "vn ") == 0)
		{
			iss >> trash >> trash;
			Vec3f vn;
			for (int i = 0; i < 3; i++)
				iss >> vn.raw[i];//raw[i]代表vn.x/vn.y和vn.z
			normals.push_back(vn);//存所有normal的集合，之后用索引在normals容器中查找
		}
		else if (line.compare(0, 2, "f ") == 0) 
		{
			//f 24/1/24 25/2/25 26/3/26
			std::vector<int> f;
			int indexVertex, indexUV, indexNormal;
			std::vector<int> faceUV;//容器由三个索引组成，对应这个三角形三个点的uv值
			iss >> trash;//trash = f
			//indexVertex=24 trash=/ indexUV=1 trash=/ indexNormal=24，f中每一行执行三次循环
			while (iss >> indexVertex >> trash >> indexUV >> trash >> indexNormal)
			{
				indexVertex--; // in wavefront obj all indices start at 1, not zero
				f.push_back(indexVertex);//1对应的是vector<Vec3f> verts_数组中的0索引
				indexUV--;//索引也是从1开始累计的，放到容器中从0开始
				f.push_back(indexUV);//每个f中加入三个顶点的uv索引，现在f有六个元素
				indexNormal--;
				f.push_back(indexNormal);
			}
			faces_.push_back(f);//f是9个元素，组成一个面，faces是所有的面集合,faces中每个元素即.obj中每一行
		}
	}
	std::cerr << "# v# " << verts_.size() << " vt# " << uvs.size() << " f# " << faces_.size() << std::endl;
	
	load_texture(filename, "_diffuse.tga", diffuseMap);
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
/// 根据face中提供的索引i，查找顶点容器中对应的该顶点位置[-1,1]
/// </summary>
/// <param name="i"></param>
/// <returns></returns>
Vec3f Model::vert(int i)
{
	return verts_[i];
}
//iface应该是遍历所有faces容器中的每一个元素，nthvert表示在这个元素中，第几个顶点数据（这个元素包含3个点)
//返回对应索引下，该顶点Position
Vec3f Model::vertPos(int iface, int nthvert)
{
	std::vector<int> face = faces_[iface];//24/1/24 25/2/25 26/3/26  face中存储了这9个int元素
	//Position记录在0 3 6索引
	int index = 0 + nthvert * 3;
	int vIndex = face[index];//在vIndex中找到对应点
	Vec3f pos = verts_[vIndex];
	return pos;
}
Vec3f Model::vertNormal(int iface, int nthvert)
{
	std::vector<int> face = faces_[iface];
	int index = 2 + nthvert * 3;//法线数据从2开始
	int normalIndex = face[index];
	Vec3f normal = normals[normalIndex];
	return normal;
}

/// <summary>
/// 根据face中提供的索引index，找到uvs容器中对应的该点的uv值[0,1]
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
Vec2f Model::GetUV(int index)
{
	return uvs[index];
}
/// <summary>
/// 
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
Vec3f Model::GetNormal(int index)
{
	return normals[index];
}

/// <summary>
/// idx为循环因子i，循环遍历整个faces容器，找到faces容器中每个face
/// </summary>
/// <param name="idx"></param>
/// <returns></returns>
std::vector<int> Model::face(int idx)
{
	return faces_[idx];
}

/// <summary>
/// 从硬盘中加载图片到img中,主要用到API是：img.read_tga_file，读到的结果放到img中
/// </summary>
/// <param name="filename"></param>
/// <param name="suffix"></param>
/// <param name="img"></param>
void Model::load_texture(std::string filename, const char* suffix, TGAImage& img)
{
	std::string texfile(filename);
	size_t dot = texfile.find_last_of(".");
	if (dot != std::string::npos) 
	{
		texfile = texfile.substr(0, dot) + std::string(suffix);//filename去掉后缀.obj + suffix名称
		std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
		img.flip_vertically();
	}
}

/// <summary>
/// 通过UV坐标采样到这张diffuseMap的颜色，public方法开放给triangle函数调用
/// </summary>
/// <param name="uv"></param>
/// <returns></returns>
TGAColor Model::SamplerDiffseColor(Vec2f uv)
{
	int diffuseMapW = diffuseMap.get_width();
	int diffuseMapH = diffuseMap.get_height();
	return diffuseMap.get((int)(diffuseMapW * uv.u), (int)(diffuseMapH * uv.v));

	//return diffuseMap.get(uv.u, uv.v);//貌似不用乘以width和height，get里面已经包含
}