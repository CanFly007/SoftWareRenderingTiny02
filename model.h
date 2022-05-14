#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model
{
private:
	std::vector<Vec3f> verts_; //v��ͷ������ ����λ��
	//�����vector����ǣ� f��ͷ һ��������� ����1����/UV1����/����1���� ����2����/UV2����/����2���� ����3����/UV3����/����3����
	std::vector<std::vector<int>> faces_; //�����vector�����9��Ԫ�أ���f��һ�У��������vector������9��Ԫ�صļ���
	std::vector<Vec2f> uvs;//vt��ͷ������
	TGAImage diffuseMap;
	void load_texture(std::string filename, const char* suffix, TGAImage& img);
	std::vector<Vec3f> normals;//vn��ͷ������

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
