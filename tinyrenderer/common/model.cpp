#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char* filename) : verts_(), tex_coords(), norms_(), faces_(), normals_(), uv_() {
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	if (in.fail()) return;
	std::string line;
	while (!in.eof()) {
		std::getline(in, line);
		std::istringstream iss(line.c_str());
		char trash;
		if (!line.compare(0, 2, "v ")) {
			iss >> trash;
			Vec3f v;
			for (int i = 0; i < 3; i++) iss >> v[i];
			verts_.push_back(v);
		}
		else if (!line.compare(0, 3, "vn ")) {
			iss >> trash >> trash;
			Vec3f n;
			for (int i = 0; i < 3; i++) iss >> n[i];
			norms_.push_back(n.normalize());
		}
		else if (!line.compare(0, 3, "vt ")) {
			iss >> trash >> trash;
			Vec2f uv;
			for (int i = 0; i < 2; i++) iss >> uv[i];
			tex_coords.push_back({ uv.x, 1 - uv.y });
		}
		else if (!line.compare(0, 2, "f ")) {
			std::vector<int> fv, fvt, fvn;
			int v_idx, vt_idx, vn_idx;
			iss >> trash;
			while (iss >> v_idx >> trash >> vt_idx >> trash >> vn_idx) {
				fv.push_back(--v_idx);
				fvt.push_back(--vt_idx);
				fvn.push_back(--vn_idx);
			}
			faces_.push_back(fv);
			uv_.push_back(fvt);
			normals_.push_back(fvn);
		}
	}
	std::cerr << "# v# " << verts_.size() <<
		" vt# " << tex_coords.size() <<
		" vn# " << norms_.size() <<
		" f# " << faces_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
	return (int)verts_.size();
}

int Model::nfaces() {
	return (int)faces_.size();
}

std::vector<int> Model::face(int idx) {
	return faces_[idx];
}

std::vector<int> Model::nomral(int idx) {
	return normals_[idx];
}

std::vector<int> Model::uv(int idx) {
	return uv_[idx];
}

Vec3f Model::vert(int i) {
	return verts_[i];
}

Vec2f Model::vTexCoords(int i) {
	return tex_coords[i];
}

Vec3f Model::vNormals(int i) {
	return norms_[i];
}