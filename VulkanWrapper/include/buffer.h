#pragma once

#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <array>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include <stb_image.h>

namespace vw {

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

struct UniformBufferObject {
	alignas(16) glm::mat4 Model;
	alignas(16) glm::mat4 View;
	alignas(16) glm::mat4 Proj;
};

class Buffer {
  public:
	Buffer();
	~Buffer();

	VkImageView TextureImageView;
	VkSampler TextureSampler;
	VkBuffer VertexBuffer;
	VkBuffer IndexBuffer;
	std::vector<VkBuffer> UniformBuffers;
	std::vector<void *> UniformBuffersMapped;

	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();

	void createVertexBuffer();
	void createIndexBuffer();
	void createUniformBuffers();

	const std::vector<uint16_t> Indices = {0, 1, 2, 2, 3, 0};
	// yuv420p
	VkImageView YImageView;
	VkImageView UImageView;
	VkImageView VImageView;
	VkSampler YSampler;
	VkSampler USampler;
	VkSampler VSampler;
	void createYUV420pImage();
	void createYUVImageView();
	void createYUVSampler(VkSampler *sampler);
	void loadYUVData();
	// yuv420p

  private:
	// yuv420p
	VkImage YImage;
	VkDeviceMemory YImageMemory;
	VkImage UImage;
	VkDeviceMemory UImageMemory;
	VkImage VImage;
	VkDeviceMemory VImageMemory;
	VkImageView createYUV420ImageView(
		VkImage image, VkFormat format, VkImageAspectFlags aspectMask);
  void createYUVImage(VkImage *image, VkDeviceMemory *deviceMemory, uint32_t width, uint32_t height);
  void copyYUVData2Image(VkImage *image, uint8_t *yuvData, int width, int height);
	// yuv420p

	VkImage TextureImage;
	VkDeviceMemory TextureImageMemory;
	VkDeviceMemory VertexBufferMemory;
	VkDeviceMemory IndexBufferMemory;
	std::vector<VkDeviceMemory> UniformBuffersMemory;
	VkDescriptorPool DescriptorPool;
	std::vector<VkDescriptorSet> DescriptorSets;
	std::vector<VkCommandBuffer> CommandBuffers;

	VkImageView createImageView(VkImage image, VkFormat format);
	void createImage(uint32_t width, uint32_t height, VkFormat format,
		VkImageTiling tiling, VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties, VkImage &image,
		VkDeviceMemory &imageMemory);
	void transitionImageLayout(
		VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(
		VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties, VkBuffer &buffer,
		VkDeviceMemory &bufferMemory);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	uint32_t findMemoryType(
		uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	const std::vector<Vertex> Vertices = {
		{{-1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // 左下角
		{{1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},	// 右下角
		{{1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},	// 右上角
		{{-1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}}; // 左上角
};
} // namespace vw
