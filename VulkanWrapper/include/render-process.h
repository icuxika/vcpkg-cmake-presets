#pragma once
#include <array>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include "buffer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace vw {
class RenderProcess {
  public:
	RenderProcess();
	~RenderProcess();

	VkRenderPass RenderPass;
	VkPipelineLayout PipelineLayout;
	VkPipeline GraphicsPipeline;
	std::vector<VkDescriptorSet> DescriptorSets;

	void createRenderPass();
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	void createDescriptorPool();
	void createDescriptorSets();

  private:
	VkDescriptorSetLayout DescriptorSetLayout;
	VkDescriptorPool DescriptorPool;
};
} // namespace vw