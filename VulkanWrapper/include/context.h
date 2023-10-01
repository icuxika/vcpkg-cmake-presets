#pragma once

#include "swapchain-context.h"
#include <memory>
#define GLFW_INCLUDE_VULKAN
#include "swapchain-context.h"
#include <GLFW/glfw3.h>
#include <iostream>

namespace vw {
class Context {
  private:
	Context();
	~Context();
	Context(const Context &) = delete;
	Context &operator=(const Context &) = delete;

	void createInstance();
	void createSurface();
	void pickPhysicalDevices();
	void findQueueFamilies();
	void createDevice();

  public:
	static Context &GetInstance();
	unsigned long long getAddress();

	struct VkQueueFamilyIndices final {
		std::optional<uint32_t> GraphicsFamily;
		std::optional<uint32_t> PresentFamily;
		bool isComplete() {
			return GraphicsFamily.has_value() && PresentFamily.has_value();
		}
	};

	void initVkContext(GLFWwindow *window);

	GLFWwindow *Window;
	VkInstance Instance;
	VkSurfaceKHR Surface;
	VkPhysicalDevice PhysicalDevice;
	VkQueueFamilyIndices QueueFamilyIndices;
	VkQueue GraphicsQueue;
	VkQueue PresentQueue;
	VkDevice Device;
	std::unique_ptr<SwapChainContext> SwapChain;
};
} // namespace vw