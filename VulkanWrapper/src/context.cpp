#include "context.h"
#include "buffer.h"
#include "render-process.h"
#include "render.h"
#include "swapchain.h"
#include <_types/_uint32_t.h>
#include <set>
#include <vector>
#include <vulkan/vulkan_core.h>

#ifdef NDEBUG
// --------------------Release--------------------
const bool isDebug = false;
#else
// --------------------Debug--------------------
const bool isDebug = true;
#endif

namespace vw {
Context::Context() { std::cout << "[Context create]----------" << std::endl; }
Context::~Context() {
	RenderProcessContext.reset();
	BufferContext.reset();
	SwapChainContext.reset();
	vkDestroySurfaceKHR(Instance, Surface, nullptr);
	vkDestroyDevice(LogicalDevice, nullptr);
	vkDestroyInstance(Instance, nullptr);
	std::cout << "[Context destroy]" << std::endl;
}
Context &Context::GetInstance() {
	static Context instance;
	return instance;
}
unsigned long long Context::getAddress() {
	return reinterpret_cast<unsigned long long>(this);
}

void Context::initVkContext(GLFWwindow *window) {
	this->Window = window;
	createInstance();
	createSurface();
	// 查询物理设备
	pickPhysicalDevices();
	// 找到支持图形操作的队列
	findQueueFamilies(PhysicalDevice);
	// 创建逻辑设备
	createLogicalDevice();
	SwapChainContext.reset(new SwapChain());
	RenderProcessContext.reset(new RenderProcess());
	SwapChainContext->createFramebuffers();
	BufferContext.reset(new Buffer());
	RenderContext.reset(new Render());
};

void Context::createInstance() {
	VkApplicationInfo applicationInfo{};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName = "Hello Triangle";
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.pEngineName = "No Engine";
	applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.apiVersion = VK_API_VERSION_1_0;
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &applicationInfo;

	std::vector<const char *> validationLayers = {};
	if (isDebug) {
		validationLayers.push_back("VK_LAYER_KHRONOS_validation");
		createInfo.enabledLayerCount =
			static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}

	uint32_t glfwExtensionCount = 0;
	const char **glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char *> extensions(
		glfwExtensions, glfwExtensions + glfwExtensionCount);
	extensions.push_back("VK_KHR_get_physical_device_properties2");
#ifdef __APPLE__
	// macos vkCreateInstance 返回 VK_ERROR_INCOMPATIBLE_DRIVER 的解决方式
	createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
	extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkResult result = vkCreateInstance(&createInfo, nullptr, &Instance);
	if (result == VK_SUCCESS) {
		std::cout << "[Vk instance created]" << std::endl;
	} else {
		std::cout << "[Vk instance creation failed]: " << result << std::endl;
		throw std::runtime_error("failed to create instance!");
	}
}

void Context::createSurface() {
	VkResult result =
		glfwCreateWindowSurface(Instance, Window, nullptr, &Surface);
	if (result == VK_SUCCESS) {
		std::cout << "[Vk surface created]" << std::endl;
	} else {
		std::cout << "[Vk surface creation failed]: " << result << std::endl;
		throw std::runtime_error("failed to create window surface!");
	}
}

void Context::pickPhysicalDevices() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(Instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(Instance, &deviceCount, devices.data());
	PhysicalDevice = devices[0];
	VkPhysicalDeviceProperties pProperties;
	vkGetPhysicalDeviceProperties(PhysicalDevice, &pProperties);
	std::cout << "Vk physical device name: " << pProperties.deviceName
			  << std::endl;
}

void Context::findQueueFamilies(VkPhysicalDevice physicalDevice) {
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(
		PhysicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(
		PhysicalDevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto &queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			QueueFamilyIndices.GraphicsFamily = i;
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(
			PhysicalDevice, i, Surface, &presentSupport);
		if (presentSupport) {
			QueueFamilyIndices.PresentFamily = i;
		}
		if (QueueFamilyIndices.isComplete()) {
			break;
		}
		i++;
	}
}

void Context::createLogicalDevice() {
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		QueueFamilyIndices.GraphicsFamily.value(),
		QueueFamilyIndices.PresentFamily.value()};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
		deviceQueueCreateInfo.sType =
			VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		deviceQueueCreateInfo.queueFamilyIndex = queueFamily;
		deviceQueueCreateInfo.queueCount = 1;
		deviceQueueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(deviceQueueCreateInfo);
	}

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.queueCreateInfoCount =
		static_cast<uint32_t>(queueCreateInfos.size());

	std::vector<const char *> validationLayers = {};
	if (isDebug) {
		validationLayers.push_back("VK_LAYER_KHRONOS_validation");
		deviceCreateInfo.enabledLayerCount =
			static_cast<uint32_t>(validationLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
	}

	std::vector<const char *> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
	extensions.push_back("VK_KHR_portability_subset");
	deviceCreateInfo.enabledExtensionCount =
		static_cast<uint32_t>(extensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = extensions.data();

	VkResult result = vkCreateDevice(
		PhysicalDevice, &deviceCreateInfo, nullptr, &LogicalDevice);
	if (result == VK_SUCCESS) {
		std::cout << "[Vk logical device created]" << std::endl;
	} else {
		std::cout << "[Vk logical device creation failed]: " << result
				  << std::endl;
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(LogicalDevice, QueueFamilyIndices.GraphicsFamily.value(),
		0, &GraphicsQueue);
	vkGetDeviceQueue(LogicalDevice, QueueFamilyIndices.PresentFamily.value(), 0,
		&PresentQueue);
}
} // namespace vw