#pragma once

#include "common.h"
class RectangleObj : public RenderObject<float, uint32_t> {
public:

	RectangleObj() {
		SetUpData();
		SetUpShader();
		SetUpTexture();
	}

	void Draw() {
		m_shader->use();
		RenderObject::Draw();
	}

private:
	void SetUpGLStatus() {
		glDisable(GL_DEPTH_TEST);
	}

	void SetUpTexture(int num = 0) override {}

	void SetUpShader() override {
		m_shader = std::make_unique<Shader>("./shader/rect_vs.glsl", "./shader/rect_fs.glsl");
	}

	void SetUpData() override {
		std::vector<VertexInfo> vertex_info = std::vector<VertexInfo>{
			{"aPos", 0, 3, GL_FLOAT, GL_FALSE, 3, 0}
		};

		std::vector<float>vertices = std::vector<float>{
			0.5f,  0.5f, 0.0f,   // top right
			 0.5f, -0.5f, 0.0f,  // bottom right
			-0.5f, -0.5f, 0.0f,  // bottom left
			-0.5f,  0.5f, 0.0f   // top left 
		};

		std::vector<uint32_t> indices = std::vector<uint32_t>{
			0, 1, 3,  // first Triangle
			1, 2, 3   // second Triangle
		};

		SetMesh(&vertices, &vertex_info, &indices);
	}
};



