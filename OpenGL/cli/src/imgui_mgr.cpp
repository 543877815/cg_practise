#include "imgui_mgr.h"

ImGuiManager* ImGuiManager::instance = nullptr;

ImGuiManager* ImGuiManager::GetInstance(GLFWwindow* window)
{
	static std::mutex mutex;
	std::lock_guard<std::mutex> lock(mutex);

	if (instance == nullptr) {
		assert(window, "Window should not be nullptr when first init!");
		instance = new ImGuiManager(window);
	}
	return instance;
}

void ImGuiManager::Render(std::vector<std::function<void()>>& func)
{

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

ImGuiManager::~ImGuiManager()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
