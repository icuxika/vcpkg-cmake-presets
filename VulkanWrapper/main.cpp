#define GLFW_INCLUDE_VULKAN
#include "context.h"
#include <GLFW/glfw3.h>
#include <iostream>

static void keyCallback(
	GLFWwindow *window, int key, int scancode, int action, int mods);

int main(int argc, char **argv) {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow *window =
		glfwCreateWindow(640, 480, "Vulkan Wrapper", nullptr, nullptr);

	vw::Context::GetInstance().initVkContext(window);

	glfwSetKeyCallback(window, keyCallback);
	glfwSetFramebufferSizeCallback(
		window, [](GLFWwindow *window, int width, int height) {
			vw::Context::GetInstance().FramebufferResized = true;
		});
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		vw::Context::GetInstance().RenderContext->drawFrame();
	}
	vkDeviceWaitIdle(vw::Context::GetInstance().LogicalDevice);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

static void keyCallback(
	GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}
