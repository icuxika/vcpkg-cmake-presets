#include "render-process.h"
#include "context.h"
#include "shader.h"
#include <vulkan/vulkan_core.h>

namespace vw {
RenderProcess::RenderProcess() {}
RenderProcess::~RenderProcess() {
	vkDestroyDescriptorPool(
		Context::GetInstance().LogicalDevice, DescriptorPool, nullptr);
	vkDestroyPipeline(
		Context::GetInstance().LogicalDevice, GraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(
		Context::GetInstance().LogicalDevice, PipelineLayout, nullptr);
	vkDestroyDescriptorSetLayout(
		Context::GetInstance().LogicalDevice, DescriptorSetLayout, nullptr);
	vkDestroyRenderPass(
		Context::GetInstance().LogicalDevice, RenderPass, nullptr);
}

void RenderProcess::createRenderPass() {
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format =
		Context::GetInstance().SwapChainContext->SwapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(Context::GetInstance().LogicalDevice,
			&renderPassInfo, nullptr, &RenderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
	VkResult result = vkCreateRenderPass(Context::GetInstance().LogicalDevice,
		&renderPassInfo, nullptr, &RenderPass);
	if (result == VK_SUCCESS) {
		std::cout << "[Vk render pass created]" << std::endl;
	} else {
		std::cout << "[Vk render pass creation failed]: " << result
				  << std::endl;
		throw std::runtime_error("failed to create render pass!");
	}
}

void RenderProcess::createDescriptorSetLayout() {
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType =
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding ySamplerLayoutBinding{};
	ySamplerLayoutBinding.binding = 2;
	ySamplerLayoutBinding.descriptorCount = 1;
	ySamplerLayoutBinding.descriptorType =
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	ySamplerLayoutBinding.pImmutableSamplers = nullptr;
	ySamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding uSamplerLayoutBinding{};
	uSamplerLayoutBinding.binding = 3;
	uSamplerLayoutBinding.descriptorCount = 1;
	uSamplerLayoutBinding.descriptorType =
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	uSamplerLayoutBinding.pImmutableSamplers = nullptr;
	uSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding vSamplerLayoutBinding{};
	vSamplerLayoutBinding.binding = 4;
	vSamplerLayoutBinding.descriptorCount = 1;
	vSamplerLayoutBinding.descriptorType =
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	vSamplerLayoutBinding.pImmutableSamplers = nullptr;
	vSamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 5> bindings = {uboLayoutBinding,
		samplerLayoutBinding, ySamplerLayoutBinding, uSamplerLayoutBinding,
		vSamplerLayoutBinding};
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	VkResult result =
		vkCreateDescriptorSetLayout(Context::GetInstance().LogicalDevice,
			&layoutInfo, nullptr, &DescriptorSetLayout);
	if (result == VK_SUCCESS) {
		std::cout << "[Vk descriptor set layout created]: "
				  << DescriptorSetLayout << std::endl;
	} else {
		std::cout << "[Vk descriptor set layout creation failed]: " << result
				  << std::endl;
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void RenderProcess::createGraphicsPipeline() {
	Shader::GetInstance().initShader();

	VkShaderModule vertShaderModule = Shader::GetInstance().VertShaderModule;
	VkShaderModule fragShaderModule = Shader::GetInstance().FragShaderModule;

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType =
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType =
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = {
		vertShaderStageInfo, fragShaderStageInfo};

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType =
		VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	auto bindingDescription = Vertex::GetBindingDescription();
	auto attributeDescriptions = Vertex::GetAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount =
		static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType =
		VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType =
		VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType =
		VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType =
		VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount =
		static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &DescriptorSetLayout;

	VkResult result =
		vkCreatePipelineLayout(Context::GetInstance().LogicalDevice,
			&pipelineLayoutInfo, nullptr, &PipelineLayout);
	if (result == VK_SUCCESS) {
		std::cout << "[Vk pipeline layout created]: " << PipelineLayout
				  << std::endl;
	} else {
		std::cout << "[Vk pipeline layout creation failed]: " << result
				  << std::endl;
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = PipelineLayout;
	pipelineInfo.renderPass = RenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	result = vkCreateGraphicsPipelines(Context::GetInstance().LogicalDevice,
		VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &GraphicsPipeline);
	if (result == VK_SUCCESS) {
		std::cout << "[Vk graphics pipeline created]: " << GraphicsPipeline
				  << std::endl;
	} else {
		std::cout << "[Vk graphics pipeline creation failed]: " << result
				  << std::endl;
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	Shader::GetInstance().destroyShaderModule();
}

void RenderProcess::createDescriptorPool() {
	std::array<VkDescriptorPoolSize, 5> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount =
		static_cast<uint32_t>(Context::GetInstance().MaxFramesInFlight);
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount =
		static_cast<uint32_t>(Context::GetInstance().MaxFramesInFlight);
	poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[2].descriptorCount =
		static_cast<uint32_t>(Context::GetInstance().MaxFramesInFlight);
	poolSizes[3].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[3].descriptorCount =
		static_cast<uint32_t>(Context::GetInstance().MaxFramesInFlight);
	poolSizes[4].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[4].descriptorCount =
		static_cast<uint32_t>(Context::GetInstance().MaxFramesInFlight);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets =
		static_cast<uint32_t>(Context::GetInstance().MaxFramesInFlight);

	VkResult result =
		vkCreateDescriptorPool(Context::GetInstance().LogicalDevice, &poolInfo,
			nullptr, &DescriptorPool);
	if (result == VK_SUCCESS) {
		std::cout << "[Vk descriptor pool created]" << std::endl;
	} else {
		std::cout << "[Vk descriptor pool creation failed]: " << result
				  << std::endl;
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void RenderProcess::createDescriptorSets() {
	std::vector<VkDescriptorSetLayout> layouts(
		Context::GetInstance().MaxFramesInFlight, DescriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = DescriptorPool;
	allocInfo.descriptorSetCount =
		static_cast<uint32_t>(Context::GetInstance().MaxFramesInFlight);
	allocInfo.pSetLayouts = layouts.data();

	DescriptorSets.resize(Context::GetInstance().MaxFramesInFlight);
	VkResult result =
		vkAllocateDescriptorSets(Context::GetInstance().LogicalDevice,
			&allocInfo, DescriptorSets.data());
	if (result == VK_SUCCESS) {
		std::cout << "[Vk descriptor sets allocated]" << std::endl;
	} else {
		std::cout << "[Vk descriptor sets allocation failed]: " << result
				  << std::endl;
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < Context::GetInstance().MaxFramesInFlight; i++) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer =
			Context::GetInstance().BufferContext->UniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView =
			Context::GetInstance().BufferContext->TextureImageView;
		imageInfo.sampler =
			Context::GetInstance().BufferContext->TextureSampler;

		VkDescriptorImageInfo yImageInfo{};
		yImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		yImageInfo.imageView = Context::GetInstance().BufferContext->YImageView;
		yImageInfo.sampler =
			Context::GetInstance().BufferContext->YSampler;

		VkDescriptorImageInfo uImageInfo{};
		uImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		uImageInfo.imageView = Context::GetInstance().BufferContext->UImageView;
		uImageInfo.sampler =
			Context::GetInstance().BufferContext->USampler;

		VkDescriptorImageInfo vImageInfo{};
		vImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		vImageInfo.imageView = Context::GetInstance().BufferContext->VImageView;
		vImageInfo.sampler =
			Context::GetInstance().BufferContext->VSampler;

		std::array<VkWriteDescriptorSet, 5> descriptorWrites{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = DescriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = DescriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType =
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[2].dstSet = DescriptorSets[i];
		descriptorWrites[2].dstBinding = 2;
		descriptorWrites[2].dstArrayElement = 0;
		descriptorWrites[2].descriptorType =
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pImageInfo = &yImageInfo;

		descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[3].dstSet = DescriptorSets[i];
		descriptorWrites[3].dstBinding = 3;
		descriptorWrites[3].dstArrayElement = 0;
		descriptorWrites[3].descriptorType =
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[3].descriptorCount = 1;
		descriptorWrites[3].pImageInfo = &uImageInfo;

		descriptorWrites[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[4].dstSet = DescriptorSets[i];
		descriptorWrites[4].dstBinding = 4;
		descriptorWrites[4].dstArrayElement = 0;
		descriptorWrites[4].descriptorType =
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[4].descriptorCount = 1;
		descriptorWrites[4].pImageInfo = &vImageInfo;

		vkUpdateDescriptorSets(Context::GetInstance().LogicalDevice,
			static_cast<uint32_t>(descriptorWrites.size()),
			descriptorWrites.data(), 0, nullptr);
	}
}
} // namespace vw
