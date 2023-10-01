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
	VkPipeline GraphicsPipeline;

  private:
	VkDescriptorSetLayout DescriptorSetLayout;
	VkPipelineLayout PipelineLayout;

	void createRenderPass();
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
};
} // namespace vw