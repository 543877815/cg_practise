#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <string>
#include <iostream>
#include "shader_s.h"
#include "camera.h"
#include "geometry.h"

GLFWwindow* initializeOpenGL();
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void createPlaneObject(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, float* vertices, size_t size_v, unsigned int* indices, size_t size_i);
void createLineObject(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, float* vertices, size_t size_v, unsigned int* indices, size_t size_i);
void createPointObject(unsigned int& VAO, unsigned int& VBO, float* vertices, size_t size_v);
template<typename T>void createRetangle(float* triangleVertices);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

template<typename T>
void calculatePointToLineDistance(Vec3<T>& v1, Vec3<T>& v2, Vec3<T>& intersectionPoint, Vec3<T>& minDistancePoint, T& minDistance) {
	Vec3<T> E1 = v1 - intersectionPoint, E2 = v2 - v1;
	T k = -(E1 * E2) / (E2 * E2);
	Vec3<T> perpendicularFoot = (E2 * k) + v1;  // 垂足
	auto updateMinDistance = [&](Vec3<T> p) {
		T distance = (intersectionPoint - p) * (intersectionPoint - p);
		if (minDistance > distance) {
			minDistance = distance;
			minDistancePoint = p;
		}
		};
	if ((perpendicularFoot - v1) * (perpendicularFoot - v2) < 0) // 垂足在线段上才需要计算
		updateMinDistance(perpendicularFoot);
	updateMinDistance(v1);
	updateMinDistance(v2);
}

template<typename T>
void calculateMinDistance(Vec3<T>& v0, Vec3<T>& v1, Vec3<T>& v2, Vec3<T>& intersectionPoint, Vec3<T>& minDistancePoint, T& minDistance) {
	calculatePointToLineDistance<T>(v0, v1, intersectionPoint, minDistancePoint, minDistance);
	calculatePointToLineDistance<T>(v1, v2, intersectionPoint, minDistancePoint, minDistance);
	calculatePointToLineDistance<T>(v0, v2, intersectionPoint, minDistancePoint, minDistance);
}

template<typename T>
bool calculateRayPlainIntersection(Vec3<T>& v0, Vec3<T>& v1, Vec3<T>& v2, float* lineVertices, float& u, float& v, Vec3<T>& intersectionPoint) {
	Line<float> line(lineVertices);  // 只取前两个点
	Vec3<T> E1 = v1 - v0, E2 = v2 - v0, S = line.m_origin - v0;
	Vec3<T> S1 = line.m_direction ^ E2, S2 = S ^ E1;
	float S1E1 = S1 * E1;
	float invDet = 1 / S1E1;
	float t = invDet * (S2 * E2);
	u = invDet * (S1 * S);
	v = invDet * (S2 * line.m_direction);
	if (t > 0 && u >= 0 && u <= 1 && v >= 0 && v <= 1 && 1 - u - v >= 0 && 1 - u - v <= 1) {
		intersectionPoint = line.m_origin + line.m_direction * t;
		return true;
	}
	return false;
}

int main()
{
	GLFWwindow* window = initializeOpenGL();
	Shader shader("./shader/planeShader.vs", "./shader/planeShader.fs");
	float triangleVertices[12] = {
		0.0f,  -0.5f, 0.0f, // bottom
		-0.5f, -0.0f, 0.0f, // left
		 0.5f, -0.0f, 0.0f, // right 
		 0.0f,  0.5f, 0.0f  // top
	};
	unsigned int triangleIndices[] = {  // 逆时针
		0, 1, 2,  // first Triangle
		1, 3, 2,    // second Triangle
	};

	float lineVertices[] = {
		0.0f, 1.5f, -0.5f,
		0.0f, 1.0f, 0.5f,
		0.0f, 0.0f, 0.0f,  // 射线与平面交点
		0.0f, 0.0f, 0.0f   // 最短距离点
	};

	unsigned int lineIndices[] = {
		0, 1,   // 射线
		2, 3,   // 射线与平面交点，交点和边最近点
	};

	unsigned int planeVBO, planeVAO, planeEBO;
	unsigned int lineVBO, lineVAO, lineEBO;
	unsigned int pointVBO, pointVAO;

	// Our state
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 主要逻辑
		bool found = false;
		float u, v;
		float minDistance = FLT_MAX;
		Vec3f intersectionPoint = { 0.0f, 0.0f ,0.0f };
		Vec3f minDistancePoint = { 0.0f, 0.0f, 0.0f };
		Vec3f v0, v1, v2;
		// caculate ray plain intersecion
		for (size_t i = 0; i < sizeof(triangleIndices) / sizeof(float) / 3; i++) {
			size_t idx1 = triangleIndices[i * 3 + 0] * 3, idx2 = triangleIndices[i * 3 + 1] * 3, idx3 = triangleIndices[i * 3 + 2] * 3;
			v0 = { triangleVertices[idx1 + 0] ,triangleVertices[idx1 + 1] ,triangleVertices[idx1 + 2] };
			v1 = { triangleVertices[idx2 + 0] ,triangleVertices[idx2 + 1] ,triangleVertices[idx2 + 2] };
			v2 = { triangleVertices[idx3 + 0] ,triangleVertices[idx3 + 1] ,triangleVertices[idx3 + 2] };
			// 判断是否和三角形相交.
			found = calculateRayPlainIntersection<float>(v0, v1, v2, lineVertices, u, v, intersectionPoint);
			if (found) { // 如果相交计算最短距离
				calculateMinDistance(v0, v1, v2, intersectionPoint, minDistancePoint, minDistance);
				break;
			}
		}
		if (!found) { // 如果与所有三角形都不相交
			intersectionPoint = v0 * (1 - u - v) + v1 * u + v2 * v;   // 计算在平面上交点
			calculateMinDistance(v0, v1, v2, intersectionPoint, minDistancePoint, minDistance);  // 计算最短距离
		}

		float pointVertices[] = {
			intersectionPoint.x,intersectionPoint.y,intersectionPoint.z,
			minDistancePoint.x,minDistancePoint.y,minDistancePoint.z
		};

		// 射线与平面交点
		lineVertices[6] = intersectionPoint.x;
		lineVertices[7] = intersectionPoint.y;
		lineVertices[8] = intersectionPoint.z;
		// 最短距离点
		lineVertices[9] = minDistancePoint.x;
		lineVertices[10] = minDistancePoint.y;
		lineVertices[11] = minDistancePoint.z;

		// 手动生成第四个点
		//createRetangle<float>(triangleVertices);

		createPlaneObject(planeVAO, planeVBO, planeEBO, triangleVertices, sizeof(triangleVertices), triangleIndices, sizeof(triangleIndices));
		createLineObject(lineVAO, lineVBO, lineEBO, lineVertices, sizeof(lineVertices), lineIndices, sizeof(lineIndices));
		createPointObject(pointVAO, pointVBO, pointVertices, sizeof(pointVertices));


		// input
		processInput(window);

		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// draw our first triangle
		shader.use();

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shader.setMat4("projection", projection);

		// camera/view transformation
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);

		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		shader.setMat4("model", model);

		// draw triangle
		shader.setVec4("color", glm::vec4(0.0f, 0.0f, 1.0f, 0.5f));
		glBindVertexArray(planeVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// drawline
		shader.setVec4("color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		glBindVertexArray(lineVAO);
		glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);

		// drawpoint
		shader.setVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		glBindVertexArray(pointVAO);
		glDrawArrays(GL_POINTS, 0, 2);


		{
			static float f = 0.0f;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			std::string distanceText = "Distance: " + std::to_string(minDistance);
			ImGui::Text(distanceText.c_str());               // Display some text (you can use a format strings too)

			ImGui::SliderFloat3("rayStart", &lineVertices[0], -5.0f, 5.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat3("rayEnd", &lineVertices[3], -5.0f, 5.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat3("bottom", &triangleVertices[0], -5.0f, 5.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat3("left", &triangleVertices[3], -5.0f, 5.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat3("right", &triangleVertices[6], -5.0f, 5.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderFloat3("top", &triangleVertices[9], -5.0f, 5.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);
	glDeleteBuffers(1, &planeEBO);
	glDeleteBuffers(1, &lineVAO);
	glDeleteBuffers(1, &lineVBO);
	glDeleteBuffers(1, &lineEBO);
	glDeleteBuffers(1, &pointVAO);
	glDeleteBuffers(1, &pointVBO);

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}


GLFWwindow* initializeOpenGL()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
	// setup Imgui binding
	ImGui::CreateContext();
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
	ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif
	ImGui_ImplOpenGL3_Init(glsl_version);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return nullptr;
	}
	return window;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void createPlaneObject(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO,
	float* vertices, size_t size_v, unsigned int* indices, size_t size_i) {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size_v, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_i, indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void createLineObject(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO,
	float* vertices, size_t size_v, unsigned int* indices, size_t size_i) {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size_v, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_i, indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glLineWidth(3.0f);
}

void createPointObject(unsigned int& VAO, unsigned int& VBO, float* vertices, size_t size_v) {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, size_v, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glPointSize(10.0f);
}

template<typename T>
void createRetangle(float* triangleVertices) {
	Vec3<T> v1{ triangleVertices[0], triangleVertices[1], triangleVertices[2] };
	Vec3<T> v2{ triangleVertices[3], triangleVertices[4], triangleVertices[5] };
	Vec3<T> v3{ triangleVertices[6], triangleVertices[7], triangleVertices[8] };

	Line <T> line{ v2, v3 };
	float distance = (v1 - line.m_direction) * line.m_direction / (line.m_direction * line.m_direction);
	Vec3<T> symmetricPoint = (line.m_direction + line.m_direction * distance) * 2 - v1;

	triangleVertices[9] = symmetricPoint.x;
	triangleVertices[10] = symmetricPoint.y;
	triangleVertices[11] = symmetricPoint.z;
}

