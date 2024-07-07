#include <vector>
#include <iostream>

#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "our_gl.h"

Model* model = NULL;
const int width = 800;
const int height = 800;

Vec3f light_dir(1, 1, 1);
Vec3f       eye(1, 1, 3);
Vec3f    center(0, 0, 0);
Vec3f        up(0, 1, 0);

struct GouraudShader : public IShader {
	Vec3f varying_intensity; // written by vertex shader, read by fragment shader

	virtual Vec4f vertex(int iface, int nthvert) {
		auto face_i = model->face(iface);
		auto normal_i = model->nomral(iface);
		Vec4f gl_Vertex = embed(model->vert(face_i[nthvert])); // read the vertex from .obj file
		auto a = ModelView * gl_Vertex;
		gl_Vertex = Viewport * Projection * ModelView * gl_Vertex;     // transform it to screen coordinates
		varying_intensity[nthvert] = std::max(0.f, model->vNormals(normal_i[nthvert]) * light_dir); // get diffuse lighting intensity
		return gl_Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor& color) {
		float intensity = varying_intensity * bar;   // interpolate intensity for the current pixel
		color = TGAColor(255, 255, 255) * intensity; // well duh
		return false;                              // no, we do not discard this pixel
	}
};

struct FirstModificationShader : public GouraudShader {
	virtual bool fragment(Vec3f bar, TGAColor& color) {
		float intensity = varying_intensity * bar;
		if (intensity > .85) intensity = 1;
		else if (intensity > .60) intensity = .80;
		else if (intensity > .45) intensity = .60;
		else if (intensity > .30) intensity = .45;
		else if (intensity > .15) intensity = .30;
		else intensity = 0;
		color = TGAColor(255, 155, 0) * intensity;
		return false;
	}
};

struct PhoneShader : public GouraudShader {
	Vec3f varying_intensity; // written by vertex shader, read by fragment shader
	Matrix varying_uv = Matrix(2, 3);        // same as above

	virtual Vec4f vertex(int iface, int nthvert) {
		auto face_i = model->face(iface);
		auto normal_i = model->nomral(iface);
		auto uv_i = model->uv(iface);
		varying_uv.set_col(nthvert, model->vTexCoords(uv_i[nthvert]));

		Vec4f gl_Vertex = embed(model->vert(face_i[nthvert])); // read the vertex from .obj file
		auto a = ModelView * gl_Vertex;
		gl_Vertex = Viewport * Projection * ModelView * gl_Vertex;     // transform it to screen coordinates
		varying_intensity[nthvert] = std::max(0.f, model->vNormals(normal_i[nthvert]) * light_dir); // get diffuse lightingintensity
		return gl_Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor& color) {
		float intensity = varying_intensity * bar;   // interpolate intensity for the current pixel
		Vec2f uv = varying_uv * bar;                // interpolate uv for the current pixel
		uv.y = 1.0f - uv.y;
		color = model->diffuse(uv) * intensity;      // well duh
		return false;                              // no, we do not discard this pixel
	}
};

struct PhoneShaderWithNormal : public GouraudShader {
	Matrix uniform_M = Matrix(4, 4); //  Projection*ModelView
	Matrix uniform_MIT = Matrix(4, 4); // (Projection*ModelView).invert_transpose()
};

int main(int argc, char** argv) {

	model = new Model("../resources/obj/african_head.obj");

	lookat(eye, center, up);
	viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	projection(-1.f / (eye - center).norm());
	light_dir.normalize();

	TGAImage image(width, height, TGAImage::RGB);
	TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

	//GouraudShader shader;
	//FirstModificationShader shader;
	PhoneShader shader;

	for (int i = 0; i < model->nfaces(); i++) {
		Vec4f screen_coords[3];
		for (int j = 0; j < 3; j++) {
			screen_coords[j] = shader.vertex(i, j);
		}
		triangle(screen_coords, shader, image, zbuffer);
	}

	image.flip_vertically(); // to place the origin in the bottom left corner of the image
	zbuffer.flip_vertically();
	image.write_tga_file("output.tga");
	zbuffer.write_tga_file("zbuffer.tga");

	delete model;
	return 0;
}