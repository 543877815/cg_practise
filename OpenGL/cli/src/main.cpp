#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>
// common
#include "glfw_mgr.h"
#include "imgui_mgr.h"

// custom
#include "callback.h"
#include "config_parser.h"
#include "render_obj_mgr.h"
// screen
const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

// camera
extern Camera camera;

int main() {

	// glfw
	GLFWManager* glfw_instance = GLFWManager::GetInstance(SCR_WIDTH, SCR_HEIGHT);
	glfw_instance->SetFrameBufferSizeCallback(framebufferSizeCallback);
	glfw_instance->SetMouseButtonCallback(mouseButtonCallback);
	glfw_instance->SetMouseCallback(mouseCallback);
	glfw_instance->SetScrollCallback(scrollCallback);
	glfw_instance->SetKeyCallback(keyCallback);

	// register
	auto render_obj_mgr = RenderObjectManager::GetInstance();

	// init from config
	std::vector<std::string> configs{ "./config/sphere.json" };
	render_obj_mgr->InitRenderObjs(configs);
	auto render_objs = render_obj_mgr->GetRenderObjs();
	// window
	GLFWwindow* window = glfw_instance->GetWindow();

	// imgui
	ImGuiManager* imgui_instance = ImGuiManager::GetInstance(window);

	glm::mat4 projection = glm::perspective(glm::radians(camera.m_zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);

	float lastFrame = 0;
	while (!glfwWindowShouldClose(window)) {
		float* clear_color = imgui_instance->GetClearColor();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);

		// timer
		float currentFrame = static_cast<float>(glfwGetTime());
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// mvp transform
		//processInput(window, camera, deltaTime);
		//processViewWorld(window, camera);
		processViewCamera(window, camera, SCR_WIDTH, SCR_HEIGHT);
		processModel(window, model);
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.m_zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		std::vector<std::function<void()>> functions;

		for (size_t i = 0; i < render_objs.size(); i++) {
			auto& render_obj = render_objs[i];

			// Draw Call
			std::unordered_map<std::string, std::any> uniform;
			uniform.emplace("projection", projection);
			uniform.emplace("view", view);
			uniform.emplace("model", model);
			render_obj->DrawObj(uniform);

			// ImGUI Callback
			auto callback = [&render_obj]() {
				render_obj->ImGuiCallback();
				};
			functions.emplace_back(callback);
		}

		imgui_instance->Render(functions);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}

