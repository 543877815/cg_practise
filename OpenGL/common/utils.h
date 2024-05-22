//#pragma once
//
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <functional>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include <iostream>
//
//void DefaultFramebufferSizeCallback(GLFWwindow* window, int width, int height) {
//	// 设置视口大小以匹配新窗口尺寸
//	glViewport(0, 0, width, height);
//}
//
//void DefaultMouseCallback(GLFWwindow* window, double xpos, double ypos) {
//	// 记录鼠标的位置，xpos 和 ypos 是鼠标在窗口中的坐标
//	std::cout << "Mouse position: (" << xpos << ", " << ypos << ")" << std::endl;
//}
//
//void DefaultScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
//	// xoffset 和 yoffset 是滚轮的滚动偏移量
//	std::cout << "Mouse scroll: (" << xoffset << ", " << yoffset << ")" << std::endl;
//}
//
//void DefaultMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
//	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
//		std::cout << "Left mouse button pressed" << std::endl;
//	}
//}
//
//void DefaultKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
//	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
//		if (key == GLFW_KEY_ESCAPE) {
//			glfwSetWindowShouldClose(window, GLFW_TRUE);
//		}
//	}
//}
//
//GLFWwindow* InitOpenGL(int screen_width, int screen_height,
//	void (*framebuffer_size_callback)(GLFWwindow*, int, int) = DefaultFramebufferSizeCallback,
//	void (*mouse_button_callback)(GLFWwindow*, int, int, int) = DefaultMouseButtonCallback,
//	void (*mouse_callback)(GLFWwindow*, double, double) = DefaultMouseCallback,
//	void (*scroll_callback)(GLFWwindow*, double, double) = DefaultScrollCallback,
//	void (*key_callback)(GLFWwindow*, int, int, int, int) = DefaultKeyCallback) {
//
//	// glfw: initialize and configure
//	// ------------------------------
//	glfwInit();
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif
//
//	// glfw window creation
//	// --------------------
//	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "LearnOpenGL", NULL, NULL);
//	if (window == NULL)
//	{
//		std::cout << "Failed to create GLFW window" << std::endl;
//		glfwTerminate();
//		return nullptr;
//	}
//	glfwMakeContextCurrent(window);
//	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//	glfwSetCursorPosCallback(window, mouse_callback);
//	glfwSetScrollCallback(window, scroll_callback);
//	glfwSetMouseButtonCallback(window, mouse_button_callback);
//	glfwSetKeyCallback(window, key_callback);
//
//	// tell GLFW to capture our mouse
//	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//	// tell GLFW to show our mouse
//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//
//	// glad: load all OpenGL function pointers
//	// ---------------------------------------
//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//	{
//		std::cout << "Failed to initialize GLAD" << std::endl;
//		return nullptr;
//	}
//
//
//	return window;
//}
//
//
