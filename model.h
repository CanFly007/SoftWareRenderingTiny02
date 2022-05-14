#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model
{
private:
	std::vector<Vec3f> verts_; //v开头的容器 顶点位置
	//里面的vector存的是： f开头 一个面的索引 顶点1索引/UV1索引/法线1索引 顶点2索引/UV2索引/法线2索引 顶点3索引/UV3索引/法线3索引
	std::vector<std::vector<int>> faces_; //里面的vector存的是9个元素（即f中一行），外面的vector是所有9个元素的集合
	std::vector<Vec2f> uvs;//vt开头的容器
	TGAImage diffuseMap;
	void load_texture(std::string filename, const char* suffix, TGAImage& img);
	std::vector<Vec3f> normals;//vn开头的容器

public:
	Model(const char* filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	Vec3f vertPos(int iface, int nthvert);
	Vec3f vertNormal(int iface, int nthvert);

	std::vector<int> face(int idx);
	Vec2f GetUV(int index);
	TGAColor SamplerDiffseColor(Vec2f uv);
	Vec3f GetNormal(int index);
};
#endif // !MODEL_H
