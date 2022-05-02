#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model
{
private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<int>> faces_;
	std::vector<Vec2f> uvs;
	TGAImage diffuseMap;
	void load_texture(std::string filename, const char* suffix, TGAImage& img);
	std::vector<Vec3f> normals;

public:
	Model(const char* filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	std::vector<int> face(int idx);
	Vec2f GetUV(int index);
	TGAColor SamplerDiffseColor(Vec2f uv);
	Vec3f GetNormal(int index);
};
#endif // !MODEL_H
