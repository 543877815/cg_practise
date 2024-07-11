#pragma once
#include<glm/glm.hpp>
#include<random>
#include<cmath>
#include<chrono>
#include "shader.h"
#include "vertexbuffer.h"
#include "imgui_mgr.h"
#include "render_object.h"
#include "aabb.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class SphereObj : public RenderObject<glm::vec3, uint32_t> {
public:

	struct Uniform {
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;
		Uniform() = default;
		Uniform(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model)
			: projection(projection), view(view), model(model) {}
	};

	SphereObj() {
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

	void ImGuiCallback() {
		bool changed = false;
		changed |= ImGui::SliderInt("number", &m_number, 0, 100000);
		changed |= ImGui::SliderFloat("radius", &m_radius, 0.0, 5.0);
		if (changed) SetUpData();
	}

	~SphereObj() = default;

private:
	int m_number = 10000;
	float m_radius = 1.0;
	std::shared_ptr<AABB> m_aabb;
	std::uniform_real_distribution<double> m_uniform01;

	void SetUpShader() override {
		m_shader = std::make_unique<Shader>("./shader/sphere_vs.glsl", "./shader/sphere_fs.glsl");
	}

	void SetUpData() override {
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::mt19937 generator(seed);
		m_uniform01 = std::uniform_real_distribution<double>(0.0, 1.0);
		std::vector<glm::vec3>vertices;
		m_aabb = std::make_shared<AABB>();

		for (int i = 0; i < m_number; i++) {
			float theta = 2 * M_PI * m_uniform01(generator);
			float phi = acos(1 - 2 * m_uniform01(generator));
			float x = sin(phi) * cos(theta) * m_radius;
			float y = sin(phi) * sin(theta) * m_radius;
			float z = cos(phi) * m_radius;
			vertices.emplace_back(glm::vec3(x, y, z));
			m_aabb->expand({ x, y, z });
		}

		std::vector<VertexInfo> vertex_info = std::vector<VertexInfo>{
			{"aPos", 0, 3, GL_FLOAT, GL_FALSE, 3, 0},
		};

		SetMesh(&vertices, &vertex_info);
		SetPrimitive(GL_POINTS);
	}

};