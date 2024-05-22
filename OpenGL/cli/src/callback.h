#pragma once
#include "glfw_mgr.h"
#include "camera.h"

extern Camera camera;

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_ESCAPE) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	}
}
void processInput(GLFWwindow* window, Camera& camera, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
			camera.ProcessKeyboard(UP, deltaTime);
		}
		else {
			camera.ProcessKeyboard(FORWARD, deltaTime);
		}
	}

	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
			camera.ProcessKeyboard(DOWN, deltaTime);
		}
		else {
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}
// https://github.com/antimatter15/splat
void processInput(GLFWwindow* window, Camera& camera, glm::mat4& modelMatrix)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	glm::vec3 translateVector(0.0f);
	float translateSpeed = 0.001f;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
			translateVector = glm::vec3(0.0f, -translateSpeed, 0.0f);
		}
		else {
			translateVector = glm::vec3(0.0f, 0.0f, translateSpeed);
		}
	}

	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
			translateVector = glm::vec3(0.0f, translateSpeed, 0.0f);
		}
		else {
			translateVector = glm::vec3(0.0f, 0.0f, -translateSpeed);
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		translateVector = glm::vec3(translateSpeed, 0.0f, 0.0f);

	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		translateVector = glm::vec3(-translateSpeed, 0.0f, 0.0f);
	}

	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translateVector);
	camera.m_position += translateVector;
	camera.m_viewMatrix *= translationMatrix;

	glm::vec3 axis = glm::vec3(1.0f, 0.0f, 0.0f);
	float rotationSpeed = 0.1f;
	float angle = glm::radians(0.0f);

	auto set_rotation = [&](float _angle, glm::vec3 _axis) {
		angle = _angle;
		axis = _axis;
		};

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		set_rotation(glm::radians(-rotationSpeed), glm::vec3(0, 1, 0));
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		set_rotation(glm::radians(rotationSpeed), glm::vec3(0, 1, 0));
	}
	else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		set_rotation(glm::radians(rotationSpeed), glm::vec3(0, 0, 1));
	}
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		set_rotation(glm::radians(-rotationSpeed), glm::vec3(0, 0, 1));
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		set_rotation(glm::radians(rotationSpeed), glm::vec3(1, 0, 0));
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		set_rotation(glm::radians(-rotationSpeed), glm::vec3(1, 0, 0));
	}

	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);
	camera.m_viewMatrix *= rotationMatrix;

	// modelMatrix
	axis = glm::vec3(1.0f, 0.0f, 0.0f);
	rotationSpeed = 0.1f;
	angle = glm::radians(0.0f);

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		set_rotation(glm::radians(-rotationSpeed), glm::vec3(1, 0, 0));
	}
	else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		set_rotation(glm::radians(rotationSpeed), glm::vec3(1, 0, 0));
	}
	else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		set_rotation(glm::radians(-rotationSpeed), glm::vec3(0, 0, 1));
	}
	else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		set_rotation(glm::radians(rotationSpeed), glm::vec3(0, 0, 1));
	}

	rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);
	modelMatrix *= rotationMatrix;
}
