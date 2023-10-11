#include "context.h"
#include "buffer.h"
#include "render-process.h"
#include "render.h"
#include "swapchain.h"
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
	BufferContext.reset();
	RenderProcessContext.reset();
	SwapChainContext.reset();
	vkDestroySurfaceKHR(Instance, Surface, nullptr);
	vkDestroyDevice(LogicalDevice, nullptr);
	if (isDebug) {
		destroyDebugUtilsMessengerEXT(Instance, DebugMessenger, nullptr);
	}
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

void Context::initVkContext(GLFWwindow *window, int videoWidth, int videoHeight) {
	this->Window = window;
	createInstance();
	setupDebugMessenger();
	createSurface();
	// 查询物理设备
	pickPhysicalDevices();
	// 创建逻辑设备
	createLogicalDevice();

	SwapChainContext.reset(new SwapChain());
	RenderProcessContext.reset(new RenderProcess());
	RenderContext.reset(new Render());
	BufferContext.reset(new Buffer());

	SwapChainContext->createSwapChain();
	SwapChainContext->createImageViews();
	RenderProcessContext->createRenderPass();
	RenderProcessContext->createDescriptorSetLayout();
	RenderProcessContext->createGraphicsPipeline();
	SwapChainContext->createFramebuffers();
	RenderContext->createCommandPool();
	BufferContext->setupVideoSize(videoWidth, videoHeight);
	BufferContext->createYUV420pImage();
	BufferContext->createYUV420pImageView();
	BufferContext->createTextureImage();
	BufferContext->createTextureImageView();
	BufferContext->createTextureSampler();
	BufferContext->createVertexBuffer();
	BufferContext->createIndexBuffer();
	BufferContext->createUniformBuffers();
	RenderProcessContext->createDescriptorPool();
	RenderProcessContext->createDescriptorSets();
	RenderContext->createCommandBuffers();
	RenderContext->createSyncObjects();
};

void Context::createInstance() {
	if (isDebug && !checkValidationLayerSupport()) {
		throw std::runtime_error(
			"validation layers requested, but not available!");
	}

	VkApplicationInfo applicationInfo{};
	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pApplicationName = "Vulkan Wrapper";
	applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.pEngineName = "No Engine";
	applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	applicationInfo.apiVersion = VK_API_VERSION_1_0;
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &applicationInfo;

	auto extensions = getRequiredExtensions();
	extensions.push_back("VK_KHR_get_physical_device_properties2");
#ifdef __APPLE__
	// macos vkCreateInstance 返回 VK_ERROR_INCOMPATIBLE_DRIVER 的解决方式
	createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
	extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (isDebug) {
		createInfo.enabledLayerCount =
			static_cast<uint32_t>(ValidationLayers.size());
		createInfo.ppEnabledLayerNames = ValidationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext =
			(VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
	} else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

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
	for (const auto &device : devices) {
		if (isDeviceSuitable(device)) {
			PhysicalDevice = device;
			break;
		}
	}
	if (PhysicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}
	PhysicalDevice = devices[0];
	VkPhysicalDeviceProperties pProperties;
	vkGetPhysicalDeviceProperties(PhysicalDevice, &pProperties);
	std::cout << "[Vk physical device name]: " << pProperties.deviceName
			  << std::endl;
}

QueueFamilyIndices Context::findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(
		device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(
		device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto &queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.GraphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(
			device, i, Surface, &presentSupport);

		if (presentSupport) {
			indices.PresentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

void Context::createLogicalDevice() {
	QueueFamilyIndices indices = findQueueFamilies(PhysicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		indices.GraphicsFamily.value(), indices.PresentFamily.value()};

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

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount =
		static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

	deviceCreateInfo.enabledExtensionCount =
		static_cast<uint32_t>(DeviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();

	if (isDebug) {
		deviceCreateInfo.enabledLayerCount =
			static_cast<uint32_t>(ValidationLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = ValidationLayers.data();
	} else {
		deviceCreateInfo.enabledLayerCount = 0;
	}

	VkResult result = vkCreateDevice(
		PhysicalDevice, &deviceCreateInfo, nullptr, &LogicalDevice);
	if (result == VK_SUCCESS) {
		std::cout << "[Vk logical device created]" << std::endl;
	} else {
		std::cout << "[Vk logical device creation failed]: " << result
				  << std::endl;
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(
		LogicalDevice, indices.GraphicsFamily.value(), 0, &GraphicsQueue);
	vkGetDeviceQueue(
		LogicalDevice, indices.PresentFamily.value(), 0, &PresentQueue);
}

SwapChainSupportDetails Context::querySwapChainSupport(
	VkPhysicalDevice device) {
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
		device, Surface, &details.Capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(
		device, Surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.Formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(
			device, Surface, &formatCount, details.Formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(
		device, Surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.PresentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			device, Surface, &presentModeCount, details.PresentModes.data());
	}
	return details;
}

bool Context::isDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices indices = findQueueFamilies(device);
	bool extensionsSupported = checkDeviceExtensionSupport(device);
	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport =
			querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.Formats.empty() &&
			!swapChainSupport.PresentModes.empty();
	}
	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
	return indices.isComplete() && extensionsSupported && swapChainAdequate &&
		supportedFeatures.samplerAnisotropy;
}

bool Context::checkDeviceExtensionSupport(VkPhysicalDevice device) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(
		device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(
		device, nullptr, &extensionCount, availableExtensions.data());
	std::set<std::string> requiredExtensions(
		DeviceExtensions.begin(), DeviceExtensions.end());
	for (const auto &extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}
	return requiredExtensions.empty();
}

std::vector<const char *> Context::getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char **glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char *> extensions(
		glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (isDebug) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return extensions;
}

bool Context::checkValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char *layerName : ValidationLayers) {
		bool layerFound = false;

		for (const auto &layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}
	return true;
}

void Context::populateDebugMessengerCreateInfo(
	VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback =
		[](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
			void *pUserData) {
			std::cerr << "validation layer: " << pCallbackData->pMessage
					  << std::endl;
			return VK_FALSE;
		};
}

void Context::setupDebugMessenger() {
	if (!isDebug)
		return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	if (createDebugUtilsMessengerEXT(
			Instance, &createInfo, nullptr, &DebugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

VkResult Context::createDebugUtilsMessengerEXT(VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
	const VkAllocationCallbacks *pAllocator,
	VkDebugUtilsMessengerEXT *pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
		instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void Context::destroyDebugUtilsMessengerEXT(VkInstance instance,
	VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks *pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
		instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}
} // namespace vw
