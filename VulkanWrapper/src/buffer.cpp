#include "buffer.h"
#include "context.h"
#include <_types/_uint32_t.h>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace vw {
Buffer::Buffer() {
	createVertexBuffer();
	createIndexBuffer();
}
Buffer::~Buffer() {
	vkFreeMemory(
		Context::GetInstance().LogicalDevice, IndexBufferMemory, nullptr);
	vkDestroyBuffer(Context::GetInstance().LogicalDevice, IndexBuffer, nullptr);
	vkFreeMemory(
		Context::GetInstance().LogicalDevice, VertexBufferMemory, nullptr);
	vkDestroyBuffer(
		Context::GetInstance().LogicalDevice, VertexBuffer, nullptr);
}

void Buffer::createVertexBuffer() {
	VkDeviceSize bufferSize = sizeof(Vertices[0]) * Vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);

	void *data;
	vkMapMemory(Context::GetInstance().LogicalDevice, stagingBufferMemory, 0,
		bufferSize, 0, &data);
	memcpy(data, Vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(Context::GetInstance().LogicalDevice, stagingBufferMemory);

	createBuffer(bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VertexBuffer, VertexBufferMemory);

	copyBuffer(stagingBuffer, VertexBuffer, bufferSize);

	vkDestroyBuffer(
		Context::GetInstance().LogicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(
		Context::GetInstance().LogicalDevice, stagingBufferMemory, nullptr);
}

void Buffer::createIndexBuffer() {
	VkDeviceSize bufferSize = sizeof(Indices[0]) * Indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);

	void *data;
	vkMapMemory(Context::GetInstance().LogicalDevice, stagingBufferMemory, 0,
		bufferSize, 0, &data);
	memcpy(data, Indices.data(), (size_t)bufferSize);
	vkUnmapMemory(Context::GetInstance().LogicalDevice, stagingBufferMemory);

	createBuffer(bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, IndexBuffer, IndexBufferMemory);

	copyBuffer(stagingBuffer, IndexBuffer, bufferSize);

	vkDestroyBuffer(
		Context::GetInstance().LogicalDevice, stagingBuffer, nullptr);
	vkFreeMemory(
		Context::GetInstance().LogicalDevice, stagingBufferMemory, nullptr);
}

void Buffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties, VkBuffer &buffer,
	VkDeviceMemory &bufferMemory) {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(Context::GetInstance().LogicalDevice, &bufferInfo,
			nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(
		Context::GetInstance().LogicalDevice, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex =
		findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(Context::GetInstance().LogicalDevice, &allocInfo,
			nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(
		Context::GetInstance().LogicalDevice, buffer, bufferMemory, 0);
}

void Buffer::copyBuffer(
	VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = Context::GetInstance().RenderContext->CommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(
		Context::GetInstance().LogicalDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(
		Context::GetInstance().GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(Context::GetInstance().GraphicsQueue);

	vkFreeCommandBuffers(Context::GetInstance().LogicalDevice,
		Context::GetInstance().RenderContext->CommandPool, 1, &commandBuffer);
}

uint32_t Buffer::findMemoryType(
	uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(
		Context::GetInstance().PhysicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & properties) ==
				properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}
} // namespace vw