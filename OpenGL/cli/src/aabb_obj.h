#pragma once
#include "render_utils.h"
#include "aabb.h"

class AABBObj : public RenderObject<glm::vec3, uint32_t> {
private:
	struct Uniform {
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;
		Uniform() = default;
		Uniform(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model) : projection(projection), view(view), model(model) {}
	};
	std::vector<glm::vec3> m_vertices{};
	std::shared_ptr<AABB> m_aabb = std::make_shared<AABB>();

public:
	AABBObj() {}
	AABBObj(std::shared_ptr<AABB> aabb) : m_aabb(aabb) {
		m_vertices.resize(8);
		SetUpVertices();
		SetUpData();
		SetUpShader();
	}

	std::shared_ptr<AABB> GetAABB() {
		return m_aabb;
	}

	void SetUpVertices() {
		auto max_corner = m_aabb->getMaxCorner();
		auto min_corner = m_aabb->getMinCorner();
		m_vertices[0] = glm::vec3(max_corner.x, max_corner.y, max_corner.z);
		m_vertices[1] = glm::vec3(max_corner.x, max_corner.y, min_corner.z);
		m_vertices[2] = glm::vec3(min_corner.x, max_corner.y, min_corner.z);
		m_vertices[3] = glm::vec3(min_corner.x, max_corner.y, max_corner.z);
		m_vertices[4] = glm::vec3(max_corner.x, min_corner.y, max_corner.z);
		m_vertices[5] = glm::vec3(max_corner.x, min_corner.y, min_corner.z);
		m_vertices[6] = glm::vec3(min_corner.x, min_corner.y, min_corner.z);
		m_vertices[7] = glm::vec3(min_corner.x, min_corner.y, max_corner.z);
	}

	void Draw(const Uniform& uniform) {
		SetUpData();

		m_shader->use();
		m_shader->setMat4("projection", uniform.projection);
		m_shader->setMat4("view", uniform.view);
		m_shader->setMat4("model", uniform.model);
		RenderObject::Draw();
	}
private:
	void SetUpShader() override {
		m_shader = std::make_unique<Shader>("./shader/aabb_vs.glsl", "./shader/aabb_fs.glsl");
	}

	void SetUpData() override {
		std::vector<VertexInfo> vertex_info = std::vector<VertexInfo>{
			{"aPos", 0, 3, GL_FLOAT, GL_FALSE, 1, 0}
		};

		std::vector<uint32_t> indices = std::vector<uint32_t>{
			0,1,
			0,3,
			1,2,
			2,3,
			4,7,
			4,5,
			6,7,
			6,5,
			3,7,
			6,2,
			4,0,
			5,1
		};

		SetMesh(&m_vertices, &vertex_info, &indices);
		SetPrimitive(GL_LINES);
	}
};