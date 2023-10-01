#include "shader.h"
#include "context.h"
#include <vector>
#include <vulkan/vulkan_core.h>

namespace vw {
Shader::Shader() {}
Shader::~Shader() {}
Shader &Shader::GetInstance() {
	static Shader instance;
	return instance;
}

void Shader::initShader() {
	auto vertShaderCode = readFile("shaders/vert.spv");
	auto fragShaderCode = readFile("shaders/frag.spv");
	createShader(vertShaderCode, fragShaderCode);
}

void Shader::destroyShaderModule() {
	vkDestroyShaderModule(
		Context::GetInstance().LogicalDevice, VertShaderModule, nullptr);
	vkDestroyShaderModule(
		Context::GetInstance().LogicalDevice, FragShaderModule, nullptr);
}

void Shader::createShader(const std::vector<char> &vertShaderCode,
	const std::vector<char> &fragShaderCode) {
	VertShaderModule = createShaderModule(vertShaderCode);
	FragShaderModule = createShaderModule(fragShaderCode);
}

VkShaderModule Shader::createShaderModule(const std::vector<char> &code) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(Context::GetInstance().LogicalDevice, &createInfo,
			nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

std::vector<char> Shader::readFile(const std::string &filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
};
} // namespace vw