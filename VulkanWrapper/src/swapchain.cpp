#include "swapchain.h"
#include "context.h"
#include <vulkan/vulkan_core.h>

namespace vw {
SwapChain::SwapChain() {}

SwapChain::~SwapChain() { cleanupSwapChain(); }

void SwapChain::createSwapChain() {
	SwapChainSupportDetails swapChainSupport =
		querySwapChainSupport(Context::GetInstance().PhysicalDevice);

	VkSurfaceFormatKHR surfaceFormat =
		chooseSwapSurfaceFormat(swapChainSupport.Formats);
	VkPresentModeKHR presentMode =
		chooseSwapPresentMode(swapChainSupport.PresentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.Capabilities);

	uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
	if (swapChainSupport.Capabilities.maxImageCount > 0 &&
		imageCount > swapChainSupport.Capabilities.maxImageCount) {
		imageCount = swapChainSupport.Capabilities.maxImageCount;
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

	createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
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
}

void SwapChain::createImageViews() {
	SwapChainImageViews.resize(SwapChainImages.size());

	for (size_t i = 0; i < SwapChainImages.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = SwapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = SwapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(Context::GetInstance().LogicalDevice, &createInfo,
				nullptr, &SwapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}
	std::cout << "[Vk image views created]" << std::endl;
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
	std::cout << "[Vk framebuffer created]" << std::endl;
}

void SwapChain::cleanupSwapChain() {
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

void SwapChain::recreateSwapChain() {
	int width = 0, height = 0;
	glfwGetFramebufferSize(Context::GetInstance().Window, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(Context::GetInstance().Window, &width, &height);
		glfwWaitEvents();
	}
	vkDeviceWaitIdle(Context::GetInstance().LogicalDevice);
	cleanupSwapChain();
	createSwapChain();
	createImageViews();
	createFramebuffers();
}

SwapChainSupportDetails SwapChain::querySwapChainSupport(
	VkPhysicalDevice device) {
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		device, Context::GetInstance().Surface, &details.Capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(
		device, Context::GetInstance().Surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.Formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device,
			Context::GetInstance().Surface, &formatCount,
			details.Formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(
		device, Context::GetInstance().Surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.PresentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device,
			Context::GetInstance().Surface, &presentModeCount,
			details.PresentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(
	const std::vector<VkSurfaceFormatKHR> &availableFormats) {
	for (const auto &availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return availableFormats[0];
}

VkPresentModeKHR SwapChain::chooseSwapPresentMode(
	const std::vector<VkPresentModeKHR> &availablePresentModes) {
	for (const auto &availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::chooseSwapExtent(
	const VkSurfaceCapabilitiesKHR &capabilities) {
	if (capabilities.currentExtent.width !=
		std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	int width, height;
	glfwGetFramebufferSize(Context::GetInstance().Window, &width, &height);

	VkExtent2D actualExtent = {
		static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

	actualExtent.width = std::clamp(actualExtent.width,
		capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	actualExtent.height = std::clamp(actualExtent.height,
		capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

	return actualExtent;
}
} // namespace vw