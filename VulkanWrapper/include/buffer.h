#pragma once

#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "structs.h"
#include <stb_image.h>

namespace vw {
class Buffer {
  public:
	Buffer();
	~Buffer();

	// yuv420p
	VkImageView YImageView;
	VkImageView UImageView;
	VkImageView VImageView;
	VkSampler YSampler;
	VkSampler USampler;
	VkSampler VSampler;
	void setupVideoSize(int width, int height);
	void createYUV420pImage();
	void createYUV420pImageView();
	void loadYUVData(uint8_t *yuvData);
	void loadYUVData(std::vector<uint8_t> yuvData);
	// yuv420p
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

  private:
	// yuv420p
	int VideoWidth;
	int VideoHeight;
	VkImage YImage;
	VkDeviceMemory YImageMemory;
	VkImage UImage;
	VkDeviceMemory UImageMemory;
	VkImage VImage;
	VkDeviceMemory VImageMemory;

	void *YBufferData;
	VkBuffer YStagingBuffer;
	VkDeviceMemory YStagingBufferMemory;
	void *UBufferData;
	VkBuffer UStagingBuffer;
	VkDeviceMemory UStagingBufferMemory;
	void *VBufferData;
	VkBuffer VStagingBuffer;
	VkDeviceMemory VStagingBufferMemory;

	void createYUVImage(VkImage *image, VkDeviceMemory *deviceMemory,
		uint32_t width, uint32_t height);
	VkImageView createYUVImageView(
		VkImage image, VkFormat format, VkImageAspectFlags aspectMask);
	void createYUVSampler(VkSampler *sampler);
	void createStagingBuffer();
	void copyYUVDataToImage(VkImage *image, uint8_t *yuvData, void *bufferData,
		int width, int height, VkBuffer stagingBuffer,
		VkDeviceMemory stagingBufferMemory);
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
