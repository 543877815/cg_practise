#pragma once
#include <unordered_map>
#include <iostream>
#include <assert.h>

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <functional>
#include <mutex>

class ImGuiManager {
public:
	ImGuiManager(const ImGuiManager&) = delete;
	ImGuiManager& operator=(const ImGuiManager&) = delete;
	static ImGuiManager* GetInstance(GLFWwindow* window = nullptr);
	float* GetClearColor() { return m_clear_color; }
	void Render(std::vector <std::function<void()>>& func);

private:
	static ImGuiManager* instance;
	GLFWwindow* m_window = nullptr;
	float m_clear_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	explicit ImGuiManager(GLFWwindow* window) : m_window(window) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	~ImGuiManager();

};


