#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "geometry.h"

class Model
{
private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<int>> faces_;
	std::vector<Vec2f> uvs;

public:
	Model(const char* filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	std::vector<int> face(int idx);
	Vec2f GetUV(int index);
};
#endif // !MODEL_H
