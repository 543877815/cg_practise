#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char* filename) : verts_(), faces_() {
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
		else if (!line.compare(0, 4, "vt  ")) {
			iss >> trash;
			Vec3f v;
			for (int i = 0; i < 3; i++) iss >> v[i];
			vTexCoords_.push_back(v);
		}
		else if (!line.compare(0, 4, "vn  ")) {
			iss >> trash;
			Vec3f v;
			for (int i = 0; i < 3; i++) iss >> v[i];
			vNormals.push_back(v);
		}
		else if (!line.compare(0, 2, "f ")) {
			std::vector<int> fv, fvt, fvn;
			int v_idx, vt_idx, vn_idx;
			iss >> trash;
			while (iss >> v_idx >> trash >> vt_idx >> trash >> vn_idx) {
				v_idx--; // in wavefront obj all indices start at 1, not zero
				vt_idx--;
				vn_idx--;
				fv.push_back(v_idx);
				fvt.push_back(vt_idx);
				fvn.push_back(vn_idx);
			}
			faces_.push_back(fv);
			uv_.push_back(fvt);
			normals_.push_back(fvn);

		}
	}
	std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << std::endl;
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

Vec3f Model::vert(int i) {
	return verts_[i];
}