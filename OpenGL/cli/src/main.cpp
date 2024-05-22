#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

// common
#include "utils.h"
#include "shader.h"
#include "camera.h"
#include "texture.h"
#include "glfw_mgr.h"
#include "imgui_mgr.h"

// custom
#include "callback.h"
#include "object_data.h"

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

	// window
	GLFWwindow* window = glfw_instance->GetWindow();

	// imgui
	ImGuiManager* imgui_instance = ImGuiManager::GetInstance(window);

	/****************** axis **********************/
	AxisObj axis_obj = AxisObj();

	/****************** case for rect  **********************/
	// https://learnopengl.com/code_viewer_gh.php?code=src/1.getting_started/2.2.hello_triangle_indexed/hello_triangle_indexed.cpp
	glDisable(GL_DEPTH_TEST);

	RectangleObj rect_obj = RectangleObj();

	/*************** case for 3D box  ********************/
	// https://learnopengl.com/code_viewer_gh.php?code=src/1.getting_started/7.4.camera_class/camera_class.cpp
	glEnable(GL_DEPTH_TEST);

	BoxObj box_obj = BoxObj();

	glm::mat4 projection = glm::perspective(glm::radians(camera.m_zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);

	float lastFrame = 0;
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// timer
		float currentFrame = static_cast<float>(glfwGetTime());
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// mvp transform
		//processInput(window, camera, deltaTime);
		processInput(window, camera, model);
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.m_zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		// imgui
		imgui_instance->Render();

		axis_obj.Draw({ projection, view, model });
		box_obj.Draw({ projection, view, model });

		//rect_obj.Draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}

