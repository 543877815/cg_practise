#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<Vec2f> tex_coords;
	std::vector<Vec3f> norms_;
	std::vector<std::vector<int>> faces_;
	std::vector<std::vector<int>> normals_;
	std::vector<std::vector<int>> uv_;
	TGAImage diffusemap_;
	void load_texture(const std::string& filename, const char* suffix, TGAImage& img);

public:
	Model(const std::string& filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	Vec2f vTexCoords(int i);
	Vec3f vNormals(int i);
	TGAColor diffuse(Vec2f uv);
	TGAColor diffuse(Vec2i uv);
	std::vector<int> face(int idx);
	std::vector<int> nomral(int idx);
	std::vector<int> uv(int idx);
	int GetDiffuseMapHeight() { return diffusemap_.get_height(); }
	int GetDiffuseMapWidth() { return diffusemap_.get_width(); }
};

#endif //__MODEL_H__