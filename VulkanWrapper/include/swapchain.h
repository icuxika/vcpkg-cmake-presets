#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>

namespace vw {
class SwapChain {
  public:
	SwapChain();
	~SwapChain();

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;
	};

	void createFramebuffers();

	VkSwapchainKHR SwapChainKHR;
	std::vector<VkImage> SwapChainImages;
	VkFormat SwapChainImageFormat;
	VkExtent2D SwapChainExtent;
	std::vector<VkImageView> SwapChainImageViews;
	std::vector<VkFramebuffer> SwapChainFramebuffers;
};
} // namespace vw