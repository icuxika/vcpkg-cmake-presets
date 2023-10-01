#include "render.h"
#include "context.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace vw {
Render::Render() {
	createCommandPool();
	createCommandBuffer();
	createSyncObjects();
}
Render::~Render() {
	vkDestroyFence(Context::GetInstance().Device, InFlightFence, nullptr);
	vkDestroySemaphore(
		Context::GetInstance().Device, RenderFinishedSemaphore, nullptr);
	vkDestroySemaphore(
		Context::GetInstance().Device, ImageAvailableSemaphore, nullptr);
	vkDestroyCommandPool(Context::GetInstance().Device, CommandPool, nullptr);
}

void Render::createCommandPool() {
	auto &queueFamilyIndices = Context::GetInstance().QueueFamilyIndices;

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();

	if (vkCreateCommandPool(Context::GetInstance().Device, &poolInfo, nullptr,
			&CommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics command pool!");
	}
}

void Render::createCommandBuffer() {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = CommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(Context::GetInstance().Device, &allocInfo,
			&CommandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

void Render::createSyncObjects() {
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateSemaphore(Context::GetInstance().Device, &semaphoreInfo,
			nullptr, &ImageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(Context::GetInstance().Device, &semaphoreInfo,
			nullptr, &RenderFinishedSemaphore) != VK_SUCCESS ||
		vkCreateFence(Context::GetInstance().Device, &fenceInfo, nullptr,
			&InFlightFence) != VK_SUCCESS) {
		throw std::runtime_error(
			"failed to create synchronization objects for a frame!");
	}
}

void Render::drawFrame() {
	vkWaitForFences(
		Context::GetInstance().Device, 1, &InFlightFence, VK_TRUE, UINT64_MAX);
	vkResetFences(Context::GetInstance().Device, 1, &InFlightFence);

	uint32_t imageIndex;
	vkAcquireNextImageKHR(Context::GetInstance().Device,
		Context::GetInstance().SwapChain->SwapChain, UINT64_MAX,
		ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	vkResetCommandBuffer(CommandBuffer, /*VkCommandBufferResetFlagBits*/ 0);
	recordCommandBuffer(CommandBuffer, imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {ImageAvailableSemaphore};
	VkPipelineStageFlags waitStages[] = {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &CommandBuffer;

	VkSemaphore signalSemaphores[] = {RenderFinishedSemaphore};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(Context::GetInstance().GraphicsQueue, 1, &submitInfo,
			InFlightFence) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = {Context::GetInstance().SwapChain->SwapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(Context::GetInstance().PresentQueue, &presentInfo);
}

void Render::recordCommandBuffer(
	VkCommandBuffer commandBuffer, uint32_t imageIndex) {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass =
		Context::GetInstance().RenderProcessContext->RenderPass;
	renderPassInfo.framebuffer =
		Context::GetInstance().SwapChain->SwapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent =
		Context::GetInstance().SwapChain->SwapChainExtent;

	VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(
		commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		Context::GetInstance().RenderProcessContext->GraphicsPipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width =
		(float)Context::GetInstance().SwapChain->SwapChainExtent.width;
	viewport.height =
		(float)Context::GetInstance().SwapChain->SwapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = Context::GetInstance().SwapChain->SwapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}
} // namespace vw