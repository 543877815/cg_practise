#include <vector>
#include <cmath>
#include <cstdlib>
#include <limits>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
Model* model = NULL;
const int width = 800;
const int height = 800;

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	for (int x = x0; x <= x1; x++) {
		float t = (x - x0) / (float)(x1 - x0);
		int y = y0 * (1. - t) + y1 * t;
		if (steep) {
			image.set(y, x, color);
		}
		else {
			image.set(x, y, color);
		}
	}
}

Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
	Vec3f s[2];
	for (int i = 1; i >= 0; i--) {
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}
	Vec3f u = cross(s[0], s[1]); // 判断有向面积是否为0，若为0则共面
	if (std::abs(u[2]) > 1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate 
		return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	return Vec3f(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

Vec3f barycentric(Vec3f pts[3], Vec3f P) {
	int Xa = pts[0].x;
	int Xb = pts[1].x;
	int Xc = pts[2].x;
	int Ya = pts[0].y;
	int Yb = pts[1].y;
	int Yc = pts[2].y;
	float u1 = (float)Xa * Yb - Xb * Ya;
	float u = ((Ya - Yb) * P.x + (Xb - Xa) * P.y + u1) / ((Ya - Yb) * Xc + (Xb - Xa) * Yc + u1);
	float v1 = (float)Xa * Yc - Xc * Ya;
	float v = ((Ya - Yc) * P.x + (Xc - Xa) * P.y + v1) / ((Ya - Yc) * Xb + (Xc - Xa) * Yb + v1);
	float a = 1 - u - v;
	//return Vec3f(1 - u - v, u, v);

	//The order matters！！！！！
	return Vec3f(1 - u - v, v, u);
}

// 线性插值
std::vector<double> binearColor(std::vector<TGAColor>& colors, std::vector<double>& as) {
	assert(colors.size() == as.size());
	std::vector<double> result(4, 0.0);

	for (unsigned int i = 0; i < colors.size(); i++) {
		result[0] += static_cast<double>(colors[i].r) * as[i];
		result[1] += static_cast<double>(colors[i].g) * as[i];
		result[2] += static_cast<double>(colors[i].b) * as[i];
		result[3] += static_cast<double>(colors[i].a) * as[i];
	}
	return result;
}

std::vector<double> binearColor(std::vector<double>& color1, std::vector<double>& color2, std::vector<double>& as) {
	assert(color1.size() == color2.size());
	size_t n = color1.size();
	assert(2 == as.size());
	std::vector<double> result(4, 0.0);
	for (size_t i = 0; i < 4; i++) {
		result[i] = color1[i] * as[0] + color2[i] * as[1];
	}
	return result;
}


void clamp(double& val, double min, double max) {
	if (val > max) val = max;
	if (val < min) val = min;
}

// 获取坐标点下的纹理颜色
TGAColor getTextureColor(Vec2f& uv, TGAImage& texture) {
	int textureWidth = texture.get_width();
	int textureHeight = texture.get_height();

	double uv_x = uv.x * textureWidth;
	double uv_y = uv.y * textureHeight;

	double x1 = std::floor(uv_x), x2 = std::ceil(uv_x);
	double y1 = std::floor(uv_y), y2 = std::ceil(uv_y);

	std::vector<double> xs = { x2 - uv_x, uv_x - x1 };
	std::vector<double> ys = { y2 - uv_y, uv_y - y1 };

	TGAColor color11 = texture.get(x1, y1);
	TGAColor color12 = texture.get(x1, y2);
	TGAColor color21 = texture.get(x2, y1);
	TGAColor color22 = texture.get(x2, y2);

	//std::vector<TGAColor> colors = { color11, color12 };
	//std::vector<double> color_x1 = binearColor(colors, xs);
	////colors = { color21, color22 };
	//std::vector<double> color_x2 = binearColor(colors, xs);
	//std::vector<double> color = binearColor(color_x1, color_x2, ys);

	//TGAColor result(color_x1[0], color_x1[1], color_x1[2], color_x1[3]);
	return color11;
}

void triangle_rgb(Vec3f* pts, Vec3f* normal, Vec2f* uv, float* zbuffer, TGAImage& image, TGAImage& texture) {
	Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	Vec2f clamp(image.get_width() - 1, image.get_height() - 1);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
		}
	}
	Vec3f P;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
			Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);
			int w = texture.get_width();
			int h = texture.get_height();
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			P.z = 0;
			std::vector<TGAColor> textureColors;
			std::vector<double> as;
			for (int i = 0; i < 3; i++) {
				P.z += pts[i][2] * bc_screen[i];  // z轴插值
				textureColors.emplace_back(getTextureColor(uv[i], texture));
				as.emplace_back(bc_screen[i]);
			}
			if (zbuffer[int(P.x + P.y * width)] < P.z) {
				zbuffer[int(P.x + P.y * width)] = P.z;
				std::vector<double> interpolate_color = binearColor(textureColors, as);
				TGAColor color(static_cast<int>(interpolate_color[0]),
					static_cast<int>(interpolate_color[1]),
					static_cast<int>(interpolate_color[2]),
					static_cast<int>(interpolate_color[3]));
				image.set(P.x, P.y, color);
			}
		}
	}
}

// TGAImage image(width, height, TGAImage::GRAYSCALE);
void triangle_grayScale(Vec3f* pts, Vec3f* normal, Vec2f* uv, float* zbuffer, TGAImage& image, TGAImage& texture) {
	Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	Vec2f clamp(image.get_width() - 1, image.get_height() - 1);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
		}
	}
	Vec3f P;
	int textureWidth = texture.get_width();
	int textureHeight = texture.get_height();

	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
			//Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);
			Vec3f bc_screen = barycentric(pts, P);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			P.z = 0;
			for (int i = 0; i < 3; i++) {
				P.z += pts[i][2] * bc_screen[i];  // z轴插值
				float uv_x = uv[i].x * textureWidth;
				float uv_y = uv[i].y * textureHeight;
			}
			if (zbuffer[int(P.x + P.y * width)] < P.z) {
				zbuffer[int(P.x + P.y * width)] = P.z;
				TGAColor color(static_cast<int>(P.z * 255.0), 1);  // for grayscale
				image.set(P.x, P.y, color);
			}
		}
	}
}

// 将 x 分量从 [-1, 1] 映射到 [0, width]。
// 将 y 分量从[-1, 1] 映射到[0, height]。
// z 分量保持不变。
Vec3f world2screen(Vec3f v) {
	return Vec3f(int((v.x + 1.) * width / 2. + .5), int((v.y + 1.) * height / 2. + .5), v.z);
}

int main(int argc, char** argv) {
	if (2 == argc) {
		model = new Model(argv[1]);
	}
	else {
		model = new Model("../resources/obj/african_head.obj");
	}

	float* zbuffer = new float[width * height];
	for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

	//TGAImage image(width, height, TGAImage::GRAYSCALE);
	TGAImage image(width, height, TGAImage::RGB);
	TGAImage texture(width, height, TGAImage::RGB);
	texture.read_tga_file("../resources/texture/african_head_diffuse.tga");

	Vec3f light_dir = Vec3f(1.0f, 0.0f, 0.0f);

	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> face_idx = model->face(i);
		std::vector<int> nomral_idx = model->nomral(i);
		std::vector<int> uv_idx = model->uv(i);
		Vec3f screen_coords[3];
		Vec3f world_coords[3];
		Vec2f uv_coords[3];
		Vec3f normal[3];
		for (int i = 0; i < 3; i++) {
			world_coords[i] = model->vert(face_idx[i]);
			screen_coords[i] = world2screen(model->vert(face_idx[i]));
			normal[i] = model->vNormals(nomral_idx[i]);
			uv_coords[i] = model->vTexCoords(uv_idx[i]);
		}

		Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		n.normalize();
		float intensity = n * light_dir;

		if (image.get_bytespp() == TGAImage::GRAYSCALE) {
			triangle_grayScale(screen_coords, normal, uv_coords, zbuffer, image, texture);
		}
		else {
			triangle_rgb(screen_coords, normal, uv_coords, zbuffer, image, texture);
		}
	}

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	if (image.get_bytespp() == TGAImage::GRAYSCALE) {
		image.write_tga_file("output_gray.tga");
	}
	else {
		image.write_tga_file("output.tga");
	}
	delete model;
	return 0;
}