#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>

// common
#include "camera.h"
#include "glfw_mgr.h"
#include "imgui_mgr.h"
#include "config.h"

// custom
#include "callback.h"
#include "config_parser.h"
#include "render_obj_mgr.h"

#include "render_main.h"

extern const int SCR_WIDTH;
extern const int SCR_HEIGHT;

int main() {
	auto render_main = RenderMain::GetInstance();
	std::vector<std::string> configs{ "./config/map.json" };
	render_main->SetRenderObjs(configs);

	while (!glfwWindowShouldClose(render_main->GetWindow())) {
		render_main->PrepareDraw();
		render_main->Draw();
		render_main->FinishDraw();
	}

	return 0;
}

