#pragma once
#include "render_object.h"

class AxisObj : public RenderObject<float, uint32_t> {

public:

	struct Uniform {
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;
		Uniform() = default;
		Uniform(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
			: projection(projection), view(view), model(model) {}
	};

	AxisObj() {
		SetUpData();
		SetUpShader();
		SetUpTexture();
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

	void SetUpTexture(int num = 0) override {}

	void SetUpShader() override {
		m_shader = std::make_unique<Shader>("./shader/axis.vs", "./shader/axis.fs");
	}

	void SetUpData() override {
		std::vector<VertexInfo> vertex_info = std::vector<VertexInfo>{
			{"aPos", 0, 3, GL_FLOAT, GL_FALSE, 6, 0},
			{"aColor", 1, 3, GL_FLOAT, GL_FALSE, 6, 3}
		};

		std::vector<float>vertices = std::vector<float>{
			-10.0, 0.0,   0.0, 1.0, 0.0, 0.0,
			 10.0, 0.0 ,  0.0, 1.0, 0.0, 0.0,
			 0.0,  10.0,  0.0, 0.0, 1.0, 0.0,
			 0.0, -10.0,  0.0, 0.0, 1.0, 0.0,
			 0.0,  0.0,  10.0, 0.0, 0.0, 1.0,
			 0.0,  0.0, -10.0, 0.0, 0.0, 1.0,
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

	void SetUpTexture(int num = 0) override {}

	void SetUpShader() override {
		m_shader = std::make_unique<Shader>("./shader/rect.vs", "./shader/rect.fs");
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

class BoxObj : public RenderObject<float, uint32_t> {

public:
	struct Uniform {
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;
		Uniform() = default;
		Uniform(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
			: projection(projection), view(view), model(model) {}
	};

	BoxObj() {
		SetUpData();
		SetUpShader();
		SetUpTexture(2);
	}

	void Draw(const Uniform& uniform) {
		m_shader->use();
		m_shader->setMat4("projection", uniform.projection);
		m_shader->setMat4("view", uniform.view);
		m_shader->setMat4("model", uniform.model);
		for (auto idx : m_textureIdx) {
			m_textures->BindTexture(idx);
		}
		RenderObject::Draw();
	}

private:

	void SetUpTexture(int num = 0) override {
		m_textures = std::make_unique<Texture>(num);
		size_t idx1 = m_textures->GenerateTexture("./textures/awesomeface.png");
		m_textureIdx.emplace_back(idx1);
		size_t idx2 = m_textures->GenerateTexture("./textures/container.jpg");
		m_textureIdx.emplace_back(idx2);

		m_shader->use();
		m_shader->setInt("texture1", idx1);
		m_shader->setInt("texture2", idx2);

	}

	void SetUpShader() override {
		m_shader = std::make_unique<Shader>("./shader/box.vs", "./shader/box.fs");
	}

	void SetUpData() override {
		std::vector<VertexInfo> vertex_info = std::vector<VertexInfo>{
			{"aPos", 0, 3, GL_FLOAT, GL_FALSE, 5, 0},
			{"aTexCoord", 1, 2, GL_FLOAT, GL_FALSE, 5, 3}
		};

		std::vector<float>vertices = std::vector<float>{
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		};

		SetMesh(&vertices, &vertex_info);
	}
};

void getBoxData(std::vector<float>& vertices, std::vector<VertexInfo>& vertex_info)
{

}