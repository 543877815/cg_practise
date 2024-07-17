#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>
// common
#include "shader_s.h"
#include "shader_c.h"
#include "camera.h"
#include "texture.h"
#include "glfw_mgr.h"
#include "imgui_mgr.h"
#include "vertexbuffer.h"

// custom
#include "callback.h"
#include "render_object/axis_obj.h"
#include "render_object/sphere_obj.h"
#include "config_parser.h"
#include "render_object/register_obj.h"

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
	InitRegisterObj();
	auto register_obj = GetRegisterObj();

	// configuration
	std::vector<JsonInfo> obj_infos;
	ParseCameraConfig("./config/sphere.json", obj_infos);

	// init render object
	std::vector<RenderObjectBase> render_objs;
	for (size_t i = 0; i < obj_infos.size(); i++) {
		auto& obj = obj_infos[i];
		render_objs.emplace_back(register_obj[obj.obj_type]);
	}

	// window
	GLFWwindow* window = glfw_instance->GetWindow();

	// imgui
	ImGuiManager* imgui_instance = ImGuiManager::GetInstance(window);

	/****************** axis **********************/
	AxisObj axis_obj = AxisObj();

	/****************** case for rect  **********************/
	// https://learnopengl.com/code_viewer_gh.php?code=src/1.getting_started/2.2.hello_triangle_indexed/hello_triangle_indexed.cpp
	//RectangleObj rect_obj = RectangleObj();

	/*************** case for 3D box  ********************/
	// https://learnopengl.com/code_viewer_gh.php?code=src/1.getting_started/7.4.camera_class/camera_class.cpp
	//BoxObj box_obj = BoxObj();

	/******************* sphere ****************************/
	SphereObj sphere_obj = SphereObj();

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

		// imgui
		std::vector<std::function<void()>> functions = {
			//std::bind(&BoxObj::ImGuiCallback, &box_obj),
			std::bind(&SphereObj::ImGuiCallback, &sphere_obj),
		};
		imgui_instance->Render(functions);

		// mvp transform
		//processInput(window, camera, deltaTime);
		//processViewWorld(window, camera);
		processViewCamera(window, camera, SCR_WIDTH, SCR_HEIGHT);
		processModel(window, model);
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.m_zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		axis_obj.Draw({ projection, view, model });
		//box_obj.Draw({ projection, view, model });
		//rect_obj.Draw();
		sphere_obj.Draw({ projection, view, model });

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}

