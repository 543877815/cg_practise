#include "render_utils.h"
#include "aabb.h"

class EllipsoidObj : public RenderObject<glm::vec3, uint32_t> {
private:
	int m_number = 10000;
	glm::vec3 m_scale{ 0.3f, 0.7f, 1.0f };
	glm::vec4 m_rotation{ 0.0f, 0.0f, 0.0f, 0.0f };
	std::shared_ptr<AABB> m_aabb = nullptr;

	struct Uniform {
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;
		Uniform() = default;
		Uniform(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model) :
			projection(projection), model(model), view(view) {}
	};
public:
	EllipsoidObj() {
		SetUpData();
		SetUpShader();
	}

	std::shared_ptr<AABB> GetAABB() {
		return m_aabb;
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
		changed |= ImGui::SliderFloat3("scale", &m_scale[0], 0, 2);
		changed |= ImGui::SliderFloat4("rotation", &m_rotation[0], -2, 2);
		if (changed) SetUpData();
	}

	~EllipsoidObj() = default;

private:

	void SetUpData() override {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::normal_distribution<> d(0.0, 1.0);
		std::vector<glm::vec3> vertices;
		m_aabb = std::make_shared<AABB>();
		Eigen::Vector3f center(0.0f, 0.0f, 0.0f);

		std::vector<float> M{
			1.0f - 2.0f * (m_rotation[2] * m_rotation[2] + m_rotation[3] * m_rotation[3]),
			2.0f * (m_rotation[1] * m_rotation[2] + m_rotation[0] * m_rotation[3]),
			2.0f * (m_rotation[1] * m_rotation[3] - m_rotation[0] * m_rotation[2]),

			2.0f * (m_rotation[1] * m_rotation[2] - m_rotation[0] * m_rotation[3]),
			1.0f - 2.0f * (m_rotation[1] * m_rotation[1] + m_rotation[3] * m_rotation[3]),
			2.0f * (m_rotation[2] * m_rotation[3] + m_rotation[0] * m_rotation[1]),

			2.0f * (m_rotation[1] * m_rotation[3] + m_rotation[0] * m_rotation[2]),
			2.0f * (m_rotation[2] * m_rotation[3] - m_rotation[0] * m_rotation[1]),
			1.0f - 2.0f * (m_rotation[1] * m_rotation[1] + m_rotation[2] * m_rotation[2]),
		};

		for (size_t i = 0; i < M.size(); i++) {
			M[i] *= m_scale[std::floor(i / 3)];
		}

		std::vector<float> s{
			M[0] * M[0] + M[3] * M[3] + M[6] * M[6],
			M[0] * M[1] + M[3] * M[4] + M[6] * M[7],
			M[0] * M[2] + M[3] * M[5] + M[6] * M[8],
			M[1] * M[1] + M[4] * M[4] + M[7] * M[7],
			M[1] * M[2] + M[4] * M[5] + M[7] * M[8],
			M[2] * M[2] + M[5] * M[5] + M[8] * M[8],
		};

		Eigen::Matrix3f Sigma(3, 3);
		Sigma << s[0], s[1], s[2],
			s[1], s[3], s[4],
			s[2], s[4], s[5];

		Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> eigenSolver(Sigma);
		if (eigenSolver.info() != Eigen::Success) {
			std::cerr << "Eigenvalue decomposition failed!" << std::endl;
		}

		Eigen::Vector3f eigenvalues = eigenSolver.eigenvalues();
		Eigen::Matrix3f eigenvectors = eigenSolver.eigenvectors();

		for (int i = 0; i < m_number; i++) {
			Eigen::VectorXf z(3);
			z(0) = d(gen);
			z(1) = d(gen);
			z(2) = d(gen);
			auto lltOfSigma = Sigma.llt();
			Eigen::VectorXf y = lltOfSigma.matrixL() * z + center;
			float norm_y = y.norm();
			glm::vec3 radius = glm::sqrt(glm::vec3(eigenvalues(0), eigenvalues(1), eigenvalues(2)));
			glm::vec3 p = glm::vec3(y[0] * radius.x, y[1] * radius.y, y[2] * radius.z);
			vertices.emplace_back(p);
		}

		Eigen::Vector3f offsetPoint;
		for (int i = 0; i < 3; i++) {
			for (int j = -1; j <= 1; j += 2) {
				float sqrt_eigenvalue = std::sqrt(eigenvalues(i));
				Eigen::Vector3f displacement = sqrt_eigenvalue * eigenvectors.col(i).normalized();
				//displacement = displacement / displacement.norm();
				offsetPoint = center + displacement * j;
				m_aabb->expand({ offsetPoint(0), offsetPoint(1), offsetPoint(2) });
			}
		}

		std::vector<VertexInfo> vertex_info = std::vector<VertexInfo>{
			{"aPos", 0, 3, GL_FLOAT, GL_FALSE, 1, 0}
		};

		SetMesh(&vertices, &vertex_info);
		SetPrimitive(GL_POINTS);
	}

	void SetUpShader() override {
		m_shader = std::make_unique<Shader>("./shader/sphere_vs.glsl", "./shader/sphere_fs.glsl");
	}
};