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

	static ImGuiManager* GetInstance(GLFWwindow* window = nullptr) {
		static std::mutex mutex;
		std::lock_guard<std::mutex> lock(mutex);

		if (instance == nullptr) {
			assert(window, "Window should not be nullptr when first init!");
			instance = new ImGuiManager(window);
		}
		return instance;
	}

	float* GetClearColor() { return m_clear_color; }

	void Render(std::vector <std::function<void()>>& func) {

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("IMGUI CLI");

		ImGui::Text("Hello, ImGui with GLFW and OpenGL!");
		ImGui::SetNextItemWidth(150);
		ImGui::ColorPicker4("Background", m_clear_color, ImGuiColorEditFlags_None | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoAlpha);

		for (auto f : func) {
			f();
		}
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

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

	~ImGuiManager() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

};


ImGuiManager* ImGuiManager::instance = nullptr;
