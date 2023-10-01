#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace vw {

struct Vertex {
	glm::vec2 Pos;
	glm::vec3 Color;

	static VkVertexInputBindingDescription GetBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2>
	GetAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2>
			attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, Pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, Color);

		return attributeDescriptions;
	}
};

class Buffer {
  public:
	Buffer();
	~Buffer();

	VkBuffer VertexBuffer;
	VkBuffer IndexBuffer;

	void createVertexBuffer();
	void createIndexBuffer();

	const std::vector<uint16_t> Indices = {0, 1, 2, 2, 3, 0};

  private:
	VkDeviceMemory VertexBufferMemory;
	VkDeviceMemory IndexBufferMemory;

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties, VkBuffer &buffer,
		VkDeviceMemory &bufferMemory);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	uint32_t findMemoryType(
		uint32_t typeFilter, VkMemoryPropertyFlags properties);

	const std::vector<Vertex> Vertices = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}, {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};
};
} // namespace vw