#pragma once
#include <array>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace vw {
class RenderProcess {
  public:
	RenderProcess();
	~RenderProcess();

	VkRenderPass RenderPass;
	VkDescriptorSetLayout DescriptorSetLayout;
	VkPipelineLayout PipelineLayout;
	VkPipeline GraphicsPipeline;

  private:
	struct Vertex {
		glm::vec2 Pos;
		glm::vec3 Color;
		glm::vec2 TexCoord;

		static VkVertexInputBindingDescription GetBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 3>
		GetAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 3>
				attributeDescriptions{};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, Pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, Color);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, TexCoord);

			return attributeDescriptions;
		}
	};

	void createRenderPass();
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
};
} // namespace vw