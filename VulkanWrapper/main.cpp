#define GLFW_INCLUDE_VULKAN
#include "context.h"
#include <GLFW/glfw3.h>
#include <iostream>

static void keyCallback(
	GLFWwindow *window, int key, int scancode, int action, int mods);

int main(int argc, char **argv) {
	vw::Context &instance1 = vw::Context::GetInstance();
	std::cout << "Address1: " << instance1.getAddress() << std::endl;
	vw::Context &instance2 = vw::Context::GetInstance();
	std::cout << "Address2: " << instance2.getAddress() << std::endl;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow *window =
		glfwCreateWindow(640, 480, "Vulkan Wrapper", nullptr, nullptr);

	instance1.initVkContext(window);

	glfwSetKeyCallback(window, keyCallback);
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

static void keyCallback(
	GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}