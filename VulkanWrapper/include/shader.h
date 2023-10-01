#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>

namespace vw {
class Shader {
  public:
	static Shader &GetInstance();
	void initShader();
	void destroyShaderModule();
	VkShaderModule VertShaderModule;
	VkShaderModule FragShaderModule;

  private:
	Shader();
	~Shader();
	Shader(const Shader &) = delete;
	Shader &operator=(const Shader &) = delete;

	void createShader(const std::vector<char> &vertShaderCode,
		const std::vector<char> &fragShaderCode);
	VkShaderModule createShaderModule(const std::vector<char> &code);
	std::vector<char> readFile(const std::string &filename);
};
} // namespace vw