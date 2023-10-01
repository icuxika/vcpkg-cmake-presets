#include "buffer.h"
#include "context.h"
#include <_types/_uint32_t.h>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace vw {
Buffer::Buffer() { createVertexBuffer(); }
Buffer::~Buffer() {
	vkFreeMemory(
		Context::GetInstance().LogicalDevice, VertexBufferMemory, nullptr);
	vkDestroyBuffer(
		Context::GetInstance().LogicalDevice, VertexBuffer, nullptr);
}

void Buffer::createVertexBuffer() {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(Vertices[0]) * Vertices.size();
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(Context::GetInstance().LogicalDevice, &bufferInfo,
			nullptr, &VertexBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create vertex buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(
		Context::GetInstance().LogicalDevice, VertexBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(Context::GetInstance().LogicalDevice, &allocInfo,
			nullptr, &VertexBufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(Context::GetInstance().LogicalDevice, VertexBuffer,
		VertexBufferMemory, 0);

	void *data;
	vkMapMemory(Context::GetInstance().LogicalDevice, VertexBufferMemory, 0,
		bufferInfo.size, 0, &data);
	memcpy(data, Vertices.data(), (size_t)bufferInfo.size);
	vkUnmapMemory(Context::GetInstance().LogicalDevice, VertexBufferMemory);
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