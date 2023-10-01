#pragma once

#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vw {
class Render {
  public:
	Render();
	~Render();

	void drawFrame();

  private:
	VkCommandPool CommandPool;
	std::vector<VkCommandBuffer> CommandBuffers;
	std::vector<VkSemaphore> ImageAvailableSemaphores;
	std::vector<VkSemaphore> RenderFinishedSemaphores;
	std::vector<VkFence> InFlightFences;

	const int MaxFramesInFlight = 2;
	uint32_t CurrentFrame = 0;

	void createCommandPool();
	void createCommandBuffers();
	void createSyncObjects();
	void recordCommandBuffer(
		VkCommandBuffer commandBuffer, uint32_t imageIndex);
};
} // namespace vw