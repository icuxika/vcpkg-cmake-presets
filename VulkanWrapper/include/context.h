#pragma once

#include "buffer.h"
#include "render.h"
#include "swapchain.h"
#include <memory>
#define GLFW_INCLUDE_VULKAN
#include "render-process.h"
#include <GLFW/glfw3.h>
#include <iostream>

namespace vw {
struct QueueFamilyIndices final {
	std::optional<uint32_t> GraphicsFamily;
	std::optional<uint32_t> PresentFamily;
	bool isComplete() {
		return GraphicsFamily.has_value() && PresentFamily.has_value();
	}
};

class Context {
  public:
	static Context &GetInstance();
	unsigned long long getAddress();

	void initVkContext(GLFWwindow *window);

	GLFWwindow *Window;
	VkSurfaceKHR Surface;
	VkPhysicalDevice PhysicalDevice;
	VkQueue GraphicsQueue;
	VkQueue PresentQueue;
	VkDevice LogicalDevice;
	std::unique_ptr<SwapChain> SwapChainContext;
	std::unique_ptr<RenderProcess> RenderProcessContext;
	std::unique_ptr<Buffer> BufferContext;
	std::unique_ptr<Render> RenderContext;

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	bool FramebufferResized = false;
	const int MaxFramesInFlight = 2;

  private:
	Context();
	~Context();
	Context(const Context &) = delete;
	Context &operator=(const Context &) = delete;

	VkInstance Instance;

	void createInstance();
	void createSurface();
	void pickPhysicalDevices();
	void createLogicalDevice();
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	const std::vector<const char *> DeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};
} // namespace vw
