#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<Vec2f> tex_coords;
	std::vector<Vec3f> norms_;
	std::vector<std::vector<int>> faces_;
	std::vector<std::vector<int>> normals_;
	std::vector<std::vector<int>> uv_;
public:
	Model(const char* filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	Vec2f vTexCoords(int i);
	Vec3f vNormals(int i);
	std::vector<int> face(int idx);
	std::vector<int> nomral(int idx);
	std::vector<int> uv(int idx);
};

#endif //__MODEL_H__