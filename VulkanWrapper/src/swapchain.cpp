#include "swapchain.h"
#include "context.h"
#include <vulkan/vulkan_core.h>

namespace vw {
SwapChain::SwapChain() {
	SwapChainSupportDetails swapChainSupportDetails;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		Context::GetInstance().PhysicalDevice, Context::GetInstance().Surface,
		&swapChainSupportDetails.Capabilities);
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(Context::GetInstance().PhysicalDevice,
		Context::GetInstance().Surface, &formatCount, nullptr);
	if (formatCount != 0) {
		swapChainSupportDetails.Formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(
			Context::GetInstance().PhysicalDevice,
			Context::GetInstance().Surface, &formatCount,
			swapChainSupportDetails.Formats.data());
	}
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(
		Context::GetInstance().PhysicalDevice, Context::GetInstance().Surface,
		&presentModeCount, nullptr);
	if (presentModeCount != 0) {
		swapChainSupportDetails.PresentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			Context::GetInstance().PhysicalDevice,
			Context::GetInstance().Surface, &presentModeCount,
			swapChainSupportDetails.PresentModes.data());
	}

	VkSurfaceFormatKHR surfaceFormat = swapChainSupportDetails.Formats[0];
	for (const auto &availableFormat : swapChainSupportDetails.Formats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			surfaceFormat = availableFormat;
		}
	}

	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
	for (const auto &availablePresentMode :
		swapChainSupportDetails.PresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			presentMode = availablePresentMode;
		}
	}

	VkExtent2D extent;
	if (swapChainSupportDetails.Capabilities.currentExtent.width !=
		std::numeric_limits<uint32_t>::max()) {
		extent = swapChainSupportDetails.Capabilities.currentExtent;
	} else {
		int width, height;
		glfwGetFramebufferSize(Context::GetInstance().Window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

		actualExtent.width = std::clamp(actualExtent.width,
			swapChainSupportDetails.Capabilities.minImageExtent.width,
			swapChainSupportDetails.Capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height,
			swapChainSupportDetails.Capabilities.minImageExtent.height,
			swapChainSupportDetails.Capabilities.maxImageExtent.height);
		extent = actualExtent;
	}

	uint32_t imageCount =
		swapChainSupportDetails.Capabilities.minImageCount + 1;
	if (swapChainSupportDetails.Capabilities.maxImageCount > 0 &&
		imageCount > swapChainSupportDetails.Capabilities.maxImageCount) {
		imageCount = swapChainSupportDetails.Capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = Context::GetInstance().Surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	auto &indices = Context::GetInstance().QueueFamilyIndices;
	uint32_t queueFamilyIndices[] = {
		indices.GraphicsFamily.value(), indices.PresentFamily.value()};

	if (indices.GraphicsFamily != indices.PresentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform =
		swapChainSupportDetails.Capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	VkResult result = vkCreateSwapchainKHR(Context::GetInstance().LogicalDevice,
		&createInfo, nullptr, &SwapChainKHR);
	if (result == VK_SUCCESS) {
		std::cout << "[Vk swapchain created]" << std::endl;
	} else {
		std::cout << "[Vk swapchain creation failed]: " << result << std::endl;
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(Context::GetInstance().LogicalDevice, SwapChainKHR,
		&imageCount, nullptr);
	SwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(Context::GetInstance().LogicalDevice, SwapChainKHR,
		&imageCount, SwapChainImages.data());

	SwapChainImageFormat = surfaceFormat.format;
	SwapChainExtent = extent;

	SwapChainImageViews.resize(SwapChainImages.size());
	for (size_t i = 0; i < SwapChainImages.size(); i++) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = SwapChainImages[i];
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = SwapChainImageFormat;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		VkImageView imageView;
		if (vkCreateImageView(Context::GetInstance().LogicalDevice, &viewInfo,
				nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}
		SwapChainImageViews[i] = imageView;
	}
}

SwapChain::~SwapChain() {
	for (auto *framebuffer : SwapChainFramebuffers) {
		vkDestroyFramebuffer(
			Context::GetInstance().LogicalDevice, framebuffer, nullptr);
	}
	for (auto *imageView : SwapChainImageViews) {
		vkDestroyImageView(
			Context::GetInstance().LogicalDevice, imageView, nullptr);
	}
	vkDestroySwapchainKHR(
		Context::GetInstance().LogicalDevice, SwapChainKHR, nullptr);
}

void SwapChain::createFramebuffers() {
	SwapChainFramebuffers.resize(SwapChainImageViews.size());

	for (size_t i = 0; i < SwapChainImageViews.size(); i++) {
		VkImageView attachments[] = {SwapChainImageViews[i]};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass =
			Context::GetInstance().RenderProcessContext->RenderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = SwapChainExtent.width;
		framebufferInfo.height = SwapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(Context::GetInstance().LogicalDevice,
				&framebufferInfo, nullptr,
				&SwapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}
} // namespace vw