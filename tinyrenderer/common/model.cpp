#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const std::string& filename) : verts_(), tex_coords(), norms_(), faces_(), normals_(), uv_() {
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
			int v_idx, vt_idx, vn_idx;  // index of vertices, texcoord, normal
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

	load_texture(filename, "_diffuse.tga", diffusemap_);
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

//Vec2i Model::uv(int idx) {
//	std::vector<int> uv_idx = uv_[idx];
//	return Vec2i(tex_coords[uv_idx[0]].x * diffusemap_.get_width(), tex_coords[uv_idx[0]].y * diffusemap_.get_height());
//}

Vec3f Model::vert(int i) {
	return verts_[i];
}

Vec2f Model::vTexCoords(int i) {
	return tex_coords[i];
}

Vec3f Model::vNormals(int i) {
	return norms_[i];
}

void Model::load_texture(const std::string& filename, const char* suffix, TGAImage& img) {
	std::string texfile(filename);
	size_t dot = texfile.find_last_of(".");
	if (dot != std::string::npos) {
		texfile = texfile.substr(0, dot) + std::string(suffix);
		std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
		img.flip_vertically();
	}
}

TGAColor Model::diffuse(Vec2i uv) {
	return diffusemap_.get(uv.x, uv.y);
}