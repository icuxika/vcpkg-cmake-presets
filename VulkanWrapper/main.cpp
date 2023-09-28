#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

static void keyCallback(
	GLFWwindow *window, int key, int scancode, int action, int mods);

int main(int argc, char **argv) {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow *window =
		glfwCreateWindow(640, 480, "Vulkan Wrapper", nullptr, nullptr);
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