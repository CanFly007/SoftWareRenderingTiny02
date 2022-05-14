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
		else if (line.compare(0, 3, "vn ") == 0)
		{
			iss >> trash >> trash;
			Vec3f vn;
			for (int i = 0; i < 3; i++)
				iss >> vn.raw[i];//raw[i]����vn.x/vn.y��vn.z
			normals.push_back(vn);//������normal�ļ��ϣ�֮����������normals�����в���
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
				indexNormal--;
				f.push_back(indexNormal);
			}
			faces_.push_back(f);//f��9��Ԫ�أ����һ���棬faces�����е��漯��,faces��ÿ��Ԫ�ؼ�.obj��ÿһ��
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
/// ����face���ṩ������i�����Ҷ��������ж�Ӧ�ĸö���λ��[-1,1]
/// </summary>
/// <param name="i"></param>
/// <returns></returns>
Vec3f Model::vert(int i)
{
	return verts_[i];
}
//ifaceӦ���Ǳ�������faces�����е�ÿһ��Ԫ�أ�nthvert��ʾ�����Ԫ���У��ڼ����������ݣ����Ԫ�ذ���3����)
//���ض�Ӧ�����£��ö���Position
Vec3f Model::vertPos(int iface, int nthvert)
{
	std::vector<int> face = faces_[iface];//24/1/24 25/2/25 26/3/26  face�д洢����9��intԪ��
	//Position��¼��0 3 6����
	int index = 0 + nthvert * 3;
	int vIndex = face[index];//��vIndex���ҵ���Ӧ��
	Vec3f pos = verts_[vIndex];
	return pos;
}
Vec3f Model::vertNormal(int iface, int nthvert)
{
	std::vector<int> face = faces_[iface];
	int index = 2 + nthvert * 3;//�������ݴ�2��ʼ
	int normalIndex = face[index];
	Vec3f normal = normals[normalIndex];
	return normal;
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
/// 
/// </summary>
/// <param name="index"></param>
/// <returns></returns>
Vec3f Model::GetNormal(int index)
{
	return normals[index];
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

/// <summary>
/// ��Ӳ���м���ͼƬ��img��,��Ҫ�õ�API�ǣ�img.read_tga_file�������Ľ���ŵ�img��
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
		texfile = texfile.substr(0, dot) + std::string(suffix);//filenameȥ����׺.obj + suffix����
		std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
		img.flip_vertically();
	}
}

/// <summary>
/// ͨ��UV�������������diffuseMap����ɫ��public�������Ÿ�triangle��������
/// </summary>
/// <param name="uv"></param>
/// <returns></returns>
TGAColor Model::SamplerDiffseColor(Vec2f uv)
{
	int diffuseMapW = diffuseMap.get_width();
	int diffuseMapH = diffuseMap.get_height();
	return diffuseMap.get((int)(diffuseMapW * uv.u), (int)(diffuseMapH * uv.v));

	//return diffuseMap.get(uv.u, uv.v);//ò�Ʋ��ó���width��height��get�����Ѿ�����
}