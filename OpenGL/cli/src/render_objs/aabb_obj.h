#pragma once
#include "common.h"
#include "../aabb.h"

namespace renderable {
	class AABBObj : public RenderObject<glm::vec3, uint32_t> {
	private:
		struct Uniform {
			glm::mat4 projection;
			glm::mat4 view;
			glm::mat4 model;
			Uniform() = default;
			Uniform(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model) : projection(projection), view(view), model(model) {}
		};
		std::vector<glm::vec3> m_vertices = std::vector<glm::vec3>(8, glm::vec3(0.0f));
		std::shared_ptr<AABB> m_aabb = std::make_shared<AABB>();

	public:
		AABBObj() {
			SetUpShader();
		}

		AABBObj(std::shared_ptr<AABB> aabb) : m_aabb(aabb) {
			SetUpVertices();
			SetUpData();
			AABBObj::AABBObj();
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

		void DrawObj(const std::unordered_map<std::string, std::any>& uniform) {
			SetUpVertices();
			SetUpData();

			auto projection = std::any_cast<glm::mat4>(uniform.at("projection"));
			auto view = std::any_cast<glm::mat4>(uniform.at("view"));
			auto model = std::any_cast<glm::mat4>(uniform.at("model"));

			m_shader->use();
			m_shader->setMat4("projection", projection);
			m_shader->setMat4("view", view);
			m_shader->setMat4("model", model);
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
}
