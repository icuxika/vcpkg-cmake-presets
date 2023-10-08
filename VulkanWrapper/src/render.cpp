#include "render.h"
#include "context.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace vw {
Render::Render() {}
Render::~Render() {
	for (size_t i = 0; i < Context::GetInstance().MaxFramesInFlight; i++) {
		vkDestroySemaphore(Context::GetInstance().LogicalDevice,
			RenderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(Context::GetInstance().LogicalDevice,
			ImageAvailableSemaphores[i], nullptr);
		vkDestroyFence(
			Context::GetInstance().LogicalDevice, InFlightFences[i], nullptr);
	}
	vkDestroyCommandPool(
		Context::GetInstance().LogicalDevice, CommandPool, nullptr);
}

void Render::createCommandPool() {
	auto queueFamilyIndices = Context::GetInstance().findQueueFamilies(
		Context::GetInstance().PhysicalDevice);
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();
	VkResult result = vkCreateCommandPool(
		Context::GetInstance().LogicalDevice, &poolInfo, nullptr, &CommandPool);
	if (result == VK_SUCCESS) {
		std::cout << "[Vk graphics command pool created]" << std::endl;
	} else {
		std::cout << "[Vk graphics command pool creation failed]: " << result
				  << std::endl;
		throw std::runtime_error("failed to create graphics command pool!");
	}
}

void Render::createCommandBuffers() {
	CommandBuffers.resize(Context::GetInstance().MaxFramesInFlight);
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = CommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)CommandBuffers.size();
	VkResult result =
		vkAllocateCommandBuffers(Context::GetInstance().LogicalDevice,
			&allocInfo, CommandBuffers.data());
	if (result == VK_SUCCESS) {
		std::cout << "[Vk command buffers allocated]" << std::endl;
	} else {
		std::cout << "[Vk command buffers allocation failed]: " << result
				  << std::endl;
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

void Render::createSyncObjects() {
	ImageAvailableSemaphores.resize(Context::GetInstance().MaxFramesInFlight);
	RenderFinishedSemaphores.resize(Context::GetInstance().MaxFramesInFlight);
	InFlightFences.resize(Context::GetInstance().MaxFramesInFlight);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < Context::GetInstance().MaxFramesInFlight; i++) {
		if (vkCreateSemaphore(Context::GetInstance().LogicalDevice,
				&semaphoreInfo, nullptr,
				&ImageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(Context::GetInstance().LogicalDevice,
				&semaphoreInfo, nullptr,
				&RenderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(Context::GetInstance().LogicalDevice, &fenceInfo,
				nullptr, &InFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error(
				"failed to create synchronization objects for a frame!");
		}
	}
	std::cout << "[Vk synchronization objects created]" << std::endl;
}

void Render::drawFrame() {
	vkWaitForFences(Context::GetInstance().LogicalDevice, 1,
		&InFlightFences[CurrentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(
		Context::GetInstance().LogicalDevice,
		Context::GetInstance().SwapChainContext->SwapChainKHR, UINT64_MAX,
		ImageAvailableSemaphores[CurrentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		Context::GetInstance().SwapChainContext->recreateSwapChain();
		return;
	}
	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	updateUniformBuffer(CurrentFrame);

	vkResetFences(
		Context::GetInstance().LogicalDevice, 1, &InFlightFences[CurrentFrame]);

	vkResetCommandBuffer(CommandBuffers[CurrentFrame], 0);
	recordCommandBuffer(CommandBuffers[CurrentFrame], imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {ImageAvailableSemaphores[CurrentFrame]};
	VkPipelineStageFlags waitStages[] = {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &CommandBuffers[CurrentFrame];

	VkSemaphore signalSemaphores[] = {RenderFinishedSemaphores[CurrentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(Context::GetInstance().GraphicsQueue, 1, &submitInfo,
			InFlightFences[CurrentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = {
		Context::GetInstance().SwapChainContext->SwapChainKHR};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	result =
		vkQueuePresentKHR(Context::GetInstance().PresentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
		Context::GetInstance().FramebufferResized) {
		Context::GetInstance().FramebufferResized = false;
		Context::GetInstance().SwapChainContext->recreateSwapChain();
	} else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	CurrentFrame =
		(CurrentFrame + 1) % Context::GetInstance().MaxFramesInFlight;
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
		Context::GetInstance()
			.SwapChainContext->SwapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent =
		Context::GetInstance().SwapChainContext->SwapChainExtent;

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
		(float)Context::GetInstance().SwapChainContext->SwapChainExtent.width;
	viewport.height =
		(float)Context::GetInstance().SwapChainContext->SwapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = Context::GetInstance().SwapChainContext->SwapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	VkBuffer vertexBuffers[] = {
		Context::GetInstance().BufferContext->VertexBuffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(commandBuffer,
		Context::GetInstance().BufferContext->IndexBuffer, 0,
		VK_INDEX_TYPE_UINT16);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		Context::GetInstance().RenderProcessContext->PipelineLayout, 0, 1,
		&Context::GetInstance()
			 .RenderProcessContext->DescriptorSets[CurrentFrame],
		0, nullptr);

	vkCmdDrawIndexed(commandBuffer,
		static_cast<uint32_t>(
			Context::GetInstance().BufferContext->Indices.size()),
		1, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

void Render::updateUniformBuffer(uint32_t currentImage) {
	UniformBufferObject ubo{};
	ubo.Model = glm::mat4(1.0f);
	ubo.View = glm::mat4(1.0f);
	ubo.Proj = glm::mat4(1.0f);
	ubo.Proj[1][1] *= -1;

	memcpy(Context::GetInstance()
			   .BufferContext->UniformBuffersMapped[currentImage],
		&ubo, sizeof(ubo));
}
} // namespace vw
