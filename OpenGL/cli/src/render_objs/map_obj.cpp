#include "map_obj.h"
#include <math.h>
RENDERABLE_BEGIN
MapObj::MapObj()
{
	SetUpData();
	SetUpShader();
	SetUpTexture(1);
}


#define PI 3.1415926535
Point MapObj::UV2XYZ(const Point& uv, float radius) {
	float longitube = (uv.x * 180 + 90) * PI / 180;
	float latitube = uv.y * 90 * PI / 180;
	Point xyz;
	xyz.x = -radius * cos(latitube) * cos(longitube);
	xyz.y = radius * sin(latitube);
	xyz.z = -radius * cos(latitube) * sin(longitube);

	return xyz;
}


void MapObj::SetUpData()
{
	uint32_t num_vertices_per_row = m_grid_width + 1;
	float grid_unit_width = (m_grid_right - m_grid_left) / m_grid_width;
	float grid_unit_height = (m_grid_top - m_grid_bottom) / m_grid_height;
	float uv_unit_width = 1.0 / m_grid_width;
	float uv_unit_height = 1.0 / m_grid_height;

	std::vector<Point> m_vertices;
	std::vector<uint32_t> indices;

	for (uint32_t i = 0; i <= m_grid_width; i++)
	{
		for (uint32_t j = 0; j <= m_grid_height; j++)
		{
			Point start(m_grid_left + i * grid_unit_width, m_grid_bottom + j * grid_unit_height, 0.0);
			Point end(UV2XYZ(start, 1));

			m_vertices.emplace_back(Point());
			m_vertices.back().x = start.x * (1 - m_transform_scale) + end.x * m_transform_scale;
			m_vertices.back().y = start.y * (1 - m_transform_scale) + end.y * m_transform_scale;
			m_vertices.back().z = start.z * (1 - m_transform_scale) + end.z * m_transform_scale;

			m_vertices.back().u = uv_unit_width * i;
			m_vertices.back().v = uv_unit_height * j;
			m_vertices.back().r = m_vertices.back().u;
			m_vertices.back().g = m_vertices.back().v;
			m_vertices.back().b = 1.0f;

			if (i != m_grid_width && j != m_grid_height) {
				indices.emplace_back(i + j * num_vertices_per_row);
				indices.emplace_back(i + 1 + (j + 1) * num_vertices_per_row);
				indices.emplace_back(i + 1 + j * num_vertices_per_row);

				indices.emplace_back(i + (j + 1) * num_vertices_per_row);
				indices.emplace_back(i + 1 + (j + 1) * num_vertices_per_row);
				indices.emplace_back(i + j * num_vertices_per_row);
			}
		}
	}

	std::vector<VertexInfo> vertex_info = std::vector<VertexInfo>{
		{"aPos", 0, 3, GL_FLOAT, GL_FALSE, 1, 0},
		{"aColor", 1, 3, GL_FLOAT, GL_FALSE, 1, 3},
		{"aTexCoord", 2, 2, GL_FLOAT, GL_FALSE, 1, 6}
	};

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	SetMesh(&m_vertices, &vertex_info, &indices);
}

void MapObj::SetUpShader()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	m_shader = std::make_unique<Shader>("./shader/map_vs.glsl", "./shader/map_fs.glsl");
}

void MapObj::DrawObj(const std::unordered_map<std::string, std::any>& uniform)
{
	auto projection = std::any_cast<glm::mat4>(uniform.at("projection"));
	auto view = std::any_cast<glm::mat4>(uniform.at("view"));
	auto model = std::any_cast<glm::mat4>(uniform.at("model"));

	m_shader->use();
	m_shader->setMat4("projection", projection);
	m_shader->setMat4("view", view);
	m_shader->setMat4("model", model);
	for (auto idx : m_textureIdx) {
		m_textures->BindTexture(idx);
	}
	RenderObject::Draw();
}

void MapObj::SetUpTexture(int num)
{
	m_textures = std::make_unique<Texture>(num);
	size_t idx1 = m_textures->GenerateTexture("D:/jupyter_notebook/cg_practise/httplib/output/0/0/0.png");
	m_textureIdx.emplace_back(idx1);

	m_shader->use();
	m_shader->setInt("texture1", idx1);
}


void MapObj::ImGuiCallback()
{
	bool is_change = false;

	static int selection = 0;
	if (ImGui::CollapsingHeader("Rasterization Mode")) {
		is_change |= ImGui::RadioButton("point", &selection, 0);
		ImGui::SameLine();
		is_change |= ImGui::RadioButton("line", &selection, 1);
		ImGui::SameLine();
		is_change |= ImGui::RadioButton("fill", &selection, 2);
	}
	if (selection == 0) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	else if (selection == 1) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else if (selection == 2) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	is_change |= ImGui::SliderInt("grid_width", &m_grid_width, 2, 50);
	is_change |= ImGui::SliderInt("grid_height", &m_grid_height, 2, 50);
	is_change |= ImGui::SliderFloat("grid_left", &m_grid_left, -1.0f, 0.0f);
	is_change |= ImGui::SliderFloat("grid_bottom", &m_grid_bottom, -1.0f, 0.0f);
	is_change |= ImGui::SliderFloat("grid_right", &m_grid_right, 0.0f, 1.0f);
	is_change |= ImGui::SliderFloat("grid_top", &m_grid_top, 0.0f, 1.0f);
	is_change |= ImGui::SliderFloat("transform_scale", &m_transform_scale, 0.0f, 1.0f);
	if (is_change) SetUpData();
}


RENDERABLE_END

