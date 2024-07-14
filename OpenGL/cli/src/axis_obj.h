#pragma once
#include "render_utils.h"
class AxisObj : public RenderObject<float, uint32_t> {
private:
	struct Uniform {
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;
		Uniform() = default;
		Uniform(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
			: projection(projection), view(view), model(model) {}
	};

public:
	AxisObj() {
		SetUpData();
		SetUpShader();
	}

	void Draw(const Uniform& uniform) {
		m_shader->use();
		m_shader->setMat4("projection", uniform.projection);
		m_shader->setMat4("view", uniform.view);
		m_shader->setMat4("model", uniform.model);
		RenderObject::Draw();
	}

	~AxisObj() = default;

private:
	void SetUpShader() override {
		m_shader = std::make_unique<Shader>("./shader/axis_vs.glsl", "./shader/axis_fs.glsl");
	}

	void SetUpData() override {
		std::vector<VertexInfo> vertex_info = std::vector<VertexInfo>{
			{"aPos", 0, 3, GL_FLOAT, GL_FALSE, 6, 0},
			{"aColor", 1, 3, GL_FLOAT, GL_FALSE, 6, 3}
		};

		std::vector<float>vertices = std::vector<float>{
			-100.0, 0.0,    0.0,   1.0, 0.0, 0.0,
			 100.0, 0.0 ,   0.0,   1.0, 0.0, 0.0,
			 0.0,   100.0,  0.0,   0.0, 1.0, 0.0,
			 0.0,  -100.0,  0.0,   0.0, 1.0, 0.0,
			 0.0,   0.0,    100.0, 0.0, 0.0, 1.0,
			 0.0,   0.0,   -100.0, 0.0, 0.0, 1.0,
		};

		std::vector<uint32_t> indices = std::vector<uint32_t>{
			0, 1,
			2, 3,
			4, 5
		};

		SetMesh(&vertices, &vertex_info, &indices);
		SetPrimitive(GL_LINES);
	}
};



