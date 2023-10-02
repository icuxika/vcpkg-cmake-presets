#pragma once

#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <chrono>

namespace vw {
class Render {
  public:
	Render();
	~Render();

	VkCommandPool CommandPool;

	void createCommandPool();
	void createCommandBuffers();
	void createSyncObjects();
	void drawFrame();

  private:
	std::vector<VkCommandBuffer> CommandBuffers;
	std::vector<VkSemaphore> ImageAvailableSemaphores;
	std::vector<VkSemaphore> RenderFinishedSemaphores;
	std::vector<VkFence> InFlightFences;

	uint32_t CurrentFrame = 0;

	void recordCommandBuffer(
		VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void updateUniformBuffer(uint32_t currentImage);
};
} // namespace vw