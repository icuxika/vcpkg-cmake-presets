#define STB_IMAGE_IMPLEMENTATION
#include "buffer.h"
#include "context.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace vw {
Buffer::Buffer() {}
Buffer::~Buffer() {
	for (size_t i = 0; i < Context::GetInstance().MaxFramesInFlight; i++) {
		vkDestroyBuffer(
			Context::GetInstance().LogicalDevice, UniformBuffers[i], nullptr);
		vkFreeMemory(Context::GetInstance().LogicalDevice,
			UniformBuffersMemory[i], nullptr);
	}
	vkFreeMemory(
		Context::GetInstance().LogicalDevice, IndexBufferMemory, nullptr);
	vkDestroyBuffer(Context::GetInstance().LogicalDevice, IndexBuffer, nullptr);
	vkFreeMemory(
		Context::GetInstance().LogicalDevice, VertexBufferMemory, nullptr);
	vkDestroyBuffer(
		Context::GetInstance().LogicalDevice, VertexBuffer, nullptr);
	vkDestroyImageView(
		Context::GetInstance().LogicalDevice, YImageView, nullptr);
	vkDestroyImageView(
		Context::GetInstance().LogicalDevice, UImageView, nullptr);
	vkDestroyImageView(
		Context::GetInstance().LogicalDevice, VImageView, nullptr);
	vkDestroyImage(Context::GetInstance().LogicalDevice, VImage, nullptr);
	vkFreeMemory(Context::GetInstance().LogicalDevice, VImageMemory, nullptr);
	vkDestroyImage(Context::GetInstance().LogicalDevice, UImage, nullptr);
	vkFreeMemory(Context::GetInstance().LogicalDevice, UImageMemory, nullptr);
	vkDestroyImage(Context::GetInstance().LogicalDevice, YImage, nullptr);
	vkFreeMemory(Context::GetInstance().LogicalDevice, YImageMemory, nullptr);
	vkDestroyBuffer(
		Context::GetInstance().LogicalDevice, VStagingBuffer, nullptr);
	vkFreeMemory(
		Context::GetInstance().LogicalDevice, VStagingBufferMemory, nullptr);
	vkDestroyBuffer(
		Context::GetInstance().LogicalDevice, UStagingBuffer, nullptr);
	vkFreeMemory(
		Context::GetInstance().LogicalDevice, UStagingBufferMemory, nullptr);
	vkDestroyBuffer(
		Context::GetInstance().LogicalDevice, YStagingBuffer, nullptr);
	vkFreeMemory(
		Context::GetInstance().LogicalDevice, YStagingBufferMemory, nullptr);
}

void Buffer::transitionImageLayout(
	VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) {
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
		newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0,
		nullptr, 0, nullptr, 1, &barrier);

	endSingleTimeCommands(commandBuffer);
}

void Buffer::copyBufferToImage(
	VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = {0, 0, 0};
	region.imageExtent = {width, height, 1};

	vkCmdCopyBufferToImage(commandBuffer, buffer, image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	endSingleTimeCommands(commandBuffer);
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
	std::cout << "[Vk vertex buffer created]" << std::endl;
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
	std::cout << "[Vk index buffer created]" << std::endl;
}

void Buffer::createUniformBuffers() {
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	UniformBuffers.resize(Context::GetInstance().MaxFramesInFlight);
	UniformBuffersMemory.resize(Context::GetInstance().MaxFramesInFlight);
	UniformBuffersMapped.resize(Context::GetInstance().MaxFramesInFlight);

	for (size_t i = 0; i < Context::GetInstance().MaxFramesInFlight; i++) {
		createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			UniformBuffers[i], UniformBuffersMemory[i]);

		vkMapMemory(Context::GetInstance().LogicalDevice,
			UniformBuffersMemory[i], 0, bufferSize, 0,
			&UniformBuffersMapped[i]);
	}
	std::cout << "[Vk uniform buffers created]" << std::endl;
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
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
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

VkCommandBuffer Buffer::beginSingleTimeCommands() {
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

	return commandBuffer;
}

void Buffer::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
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
void Buffer::setupVideoSize(int width, int height) {
	VideoWidth = width;
	VideoHeight = height;
}

void Buffer::createYUV420pImage() {
	createYUVImage(&YImage, &YImageMemory, VideoWidth, VideoHeight);
	createYUVImage(&UImage, &UImageMemory, VideoWidth / 2, VideoHeight / 2);
	createYUVImage(&VImage, &VImageMemory, VideoWidth / 2, VideoHeight / 2);
	transitionImageLayout(YImage, VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	transitionImageLayout(YImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	transitionImageLayout(UImage, VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	transitionImageLayout(UImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	transitionImageLayout(VImage, VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	transitionImageLayout(VImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void Buffer::createYUVImage(VkImage *image, VkDeviceMemory *deviceMemory,
	uint32_t width, uint32_t height) {
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = VK_FORMAT_R8_UNORM;
	imageInfo.tiling = VK_IMAGE_TILING_LINEAR;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage =
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;

	if (vkCreateImage(Context::GetInstance().LogicalDevice, &imageInfo, nullptr,
			image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(
		Context::GetInstance().LogicalDevice, *image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(Context::GetInstance().LogicalDevice, &allocInfo,
			nullptr, deviceMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(
		Context::GetInstance().LogicalDevice, *image, *deviceMemory, 0);
}

void Buffer::createYUV420pImageView() {
	YImageView = createYUVImageView(
		YImage, VK_FORMAT_R8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
	UImageView = createYUVImageView(
		UImage, VK_FORMAT_R8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
	VImageView = createYUVImageView(
		VImage, VK_FORMAT_R8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
	createYUVSampler(&YSampler);
	createYUVSampler(&USampler);
	createYUVSampler(&VSampler);

	createStagingBuffer();
}

VkImageView Buffer::createYUVImageView(
	VkImage image, VkFormat format, VkImageAspectFlags aspectMask) {
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectMask;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	VkImageView imageView;
	if (vkCreateImageView(Context::GetInstance().LogicalDevice, &viewInfo,
			nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}
	return imageView;
}

void Buffer::createYUVSampler(VkSampler *sampler) {
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(
		Context::GetInstance().PhysicalDevice, &properties);

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

	if (vkCreateSampler(Context::GetInstance().LogicalDevice, &samplerInfo,
			nullptr, sampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
}

void Buffer::createStagingBuffer() {
	createBuffer(VideoWidth * VideoHeight, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		YStagingBuffer, YStagingBufferMemory);
	vkMapMemory(Context::GetInstance().LogicalDevice, YStagingBufferMemory, 0,
		VideoWidth * VideoHeight, 0, &YBufferData);

	createBuffer(VideoWidth * VideoHeight / 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		UStagingBuffer, UStagingBufferMemory);
	vkMapMemory(Context::GetInstance().LogicalDevice, UStagingBufferMemory, 0,
		VideoWidth * VideoHeight / 4, 0, &UBufferData);

	createBuffer(VideoWidth * VideoHeight / 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		VStagingBuffer, VStagingBufferMemory);
	vkMapMemory(Context::GetInstance().LogicalDevice, VStagingBufferMemory, 0,
		VideoWidth * VideoHeight / 4, 0, &VBufferData);
}

void Buffer::loadYUVData(uint8_t *yuvData) {
	uint8_t *yData = yuvData;
	uint8_t *uData = yData + VideoWidth * VideoHeight;
	uint8_t *vData = uData + (VideoWidth / 2) * (VideoHeight / 2);
	copyYUVDataToImage(&YImage, yData, YBufferData, VideoWidth, VideoHeight,
		YStagingBuffer, YStagingBufferMemory);
	copyYUVDataToImage(&UImage, uData, UBufferData, VideoWidth / 2,
		VideoHeight / 2, UStagingBuffer, UStagingBufferMemory);
	copyYUVDataToImage(&VImage, vData, VBufferData, VideoWidth / 2,
		VideoHeight / 2, VStagingBuffer, VStagingBufferMemory);
}

void Buffer::loadYUVData(std::vector<uint8_t> yuvData) {
	uint8_t *yData = yuvData.data();
	uint8_t *uData = yData + VideoWidth * VideoHeight;
	uint8_t *vData = uData + (VideoWidth / 2) * (VideoHeight / 2);
	copyYUVDataToImage(&YImage, yData, YBufferData, VideoWidth, VideoHeight,
		YStagingBuffer, YStagingBufferMemory);
	copyYUVDataToImage(&UImage, uData, UBufferData, VideoWidth / 2,
		VideoHeight / 2, UStagingBuffer, UStagingBufferMemory);
	copyYUVDataToImage(&VImage, vData, VBufferData, VideoWidth / 2,
		VideoHeight / 2, VStagingBuffer, VStagingBufferMemory);
}

void Buffer::copyYUVDataToImage(VkImage *image, uint8_t *yuvData,
	void *bufferData, int width, int height, VkBuffer stagingBuffer,
	VkDeviceMemory stagingBufferMemory) {
	size_t imageSize = width * height;
	memcpy(bufferData, yuvData, static_cast<size_t>(imageSize));
	transitionImageLayout(*image, VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(stagingBuffer, *image, static_cast<uint32_t>(width),
		static_cast<uint32_t>(height));
	transitionImageLayout(*image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}
} // namespace vw
