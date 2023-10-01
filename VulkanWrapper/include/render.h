#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vw {
class Render {
  public:
	Render();
	~Render();

	VkCommandPool CommandPool;
	VkCommandBuffer CommandBuffer;

	VkSemaphore ImageAvailableSemaphore;
	VkSemaphore RenderFinishedSemaphore;
	VkFence InFlightFence;

	void drawFrame();

  private:
	void createCommandPool();
	void createCommandBuffer();
	void createSyncObjects();

	void recordCommandBuffer(
		VkCommandBuffer commandBuffer, uint32_t imageIndex);
};
} // namespace vw