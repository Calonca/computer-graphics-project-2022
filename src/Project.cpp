#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <optional>
#include <set>
#include <cstdint>
#include <algorithm>
#include <array>

#include "gameObjects/Object.hpp"

#include "utils/definitions.hpp"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include <chrono>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static float rot = 0;
static float change_time = 0;
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::string MODEL_PATH = "resources/models/";
const std::string TEXTURE_PATH = "resources/textures/";
const std::string SHADER_PATH = "shaders/";

const int MAX_FRAMES_IN_FLIGHT = 2;

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
	const bool Verbose = false;
#else
	const bool enableValidationLayers = true;
	const bool Verbose = true;
#endif
    Object sceneToLoad = Object("root",mat4(1));

struct SkyBoxModel {
	const char *ObjFile;
	const char *TextureFile[6];
};

const SkyBoxModel SkyBoxToLoad = {"SkyBoxCube.obj", {"sky/sky_sides.png", "sky/sky_sides.png", "sky/sky_top.png", "sky/sky_sides.png", "sky/sky_bottom.png", "sky/sky_sides.png"}};

struct SingleText {
	int usedLines;
	const char *l[4];
	int start;
	int len;
};

std::vector<SingleText> SceneText = {
        {1, {"Third Person View", "", "", ""}, 0, 0},
        {1, {"First Person View", "", "", ""}, 0, 0},
};

std::vector<float> M1_vertices;
std::vector<uint32_t> M1_indices;

#include "worldgen/models.cpp"
#include "utils/MatrixUtils.hpp"
#include "gameObjects/Truck.hpp"
#include "physics/PhysicsEngine.hpp"
#include "Project.hpp"


namespace std {
	template<> struct hash<std::vector<float>> {
		size_t operator()(std::vector<float> const& vertex) const {
			size_t h = 0;
			for(int i = 0; i < vertex.size(); i++) {
				h ^= static_cast<size_t>(vertex[i]);
			}
			return h;
		}
	};
}

//Contains variables that are transfered to the shaders
struct UniformBufferObject {
	vec4 ti;//time
	mat4 mvpMat;
	mat4 mMat;
	mat4 nMat;
};

struct TerrainUniformBufferObject {
    vec2 translation;//Terrain translation
};


struct GlobalUniformBufferObject {
	alignas(16) glm::vec3 lightDir;
	alignas(16) glm::vec4 lightColor;
	alignas(16) glm::vec3 eyePos;
    alignas(16) glm::vec3 leftSpotlightDir;
    alignas(16) glm::vec3 leftSpotlightPos;
    alignas(16) glm::vec3 leftSpotlightColor;
    alignas(16) glm::vec4 leftSpotlightParams;
    alignas(16) glm::vec3 rightSpotlightDir;
    alignas(16) glm::vec3 rightSpotlightPos;
    alignas(16) glm::vec3 rightSpotlightColor;
    alignas(16) glm::vec4 rightSpotlightParams;
};

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
				vkGetInstanceProcAddr(instance,
					"vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
				vkGetInstanceProcAddr(instance,
					"vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() &&
			   presentFamily.has_value();
	}
};
    
struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};


struct ErrorCode {
	VkResult resultCode;
	std::string meaning;
}
errorCodes[ ] = {
	{ VK_NOT_READY, "Not Ready" },
	{ VK_TIMEOUT, "Timeout" },
	{ VK_EVENT_SET, "Event Set" },
	{ VK_EVENT_RESET, "Event Reset" },
	{ VK_INCOMPLETE, "Incomplete" },
	{ VK_ERROR_OUT_OF_HOST_MEMORY, "Out of Host Memory" },
	{ VK_ERROR_OUT_OF_DEVICE_MEMORY, "Out of Device Memory" },
	{ VK_ERROR_INITIALIZATION_FAILED, "Initialization Failed" },
	{ VK_ERROR_DEVICE_LOST, "Device Lost" },
	{ VK_ERROR_MEMORY_MAP_FAILED, "Memory Map Failed" },
	{ VK_ERROR_LAYER_NOT_PRESENT, "Layer Not Present" },
	{ VK_ERROR_EXTENSION_NOT_PRESENT, "Extension Not Present" },
	{ VK_ERROR_FEATURE_NOT_PRESENT, "Feature Not Present" },
	{ VK_ERROR_INCOMPATIBLE_DRIVER, "Incompatible Driver" },
	{ VK_ERROR_TOO_MANY_OBJECTS, "Too Many Objects" },
	{ VK_ERROR_FORMAT_NOT_SUPPORTED, "Format Not Supported" },
	{ VK_ERROR_FRAGMENTED_POOL, "Fragmented Pool" },
	{ VK_ERROR_SURFACE_LOST_KHR, "Surface Lost" },
	{ VK_ERROR_NATIVE_WINDOW_IN_USE_KHR, "Native Window in Use" },
	{ VK_SUBOPTIMAL_KHR, "Suboptimal" },
	{ VK_ERROR_OUT_OF_DATE_KHR, "Error Out of Date" },
	{ VK_ERROR_INCOMPATIBLE_DISPLAY_KHR, "Incompatible Display" },
	{ VK_ERROR_VALIDATION_FAILED_EXT, "Valuidation Failed" },
	{ VK_ERROR_INVALID_SHADER_NV, "Invalid Shader" },
	{ VK_ERROR_OUT_OF_POOL_MEMORY_KHR, "Out of Pool Memory" },
	{ VK_ERROR_INVALID_EXTERNAL_HANDLE, "Invalid External Handle" },

};

void PrintVkError( VkResult result ) {
	const int numErrorCodes = sizeof( errorCodes ) / sizeof( struct ErrorCode );
	std::string meaning = "";
	for(ErrorCode &errorCode : errorCodes) {
		if( result == errorCode.resultCode ) {
			meaning = errorCode.meaning;
			break;
		}
	}
	std::cout << "Error: " << result << ", " << meaning << "\n";
}

class CGProject {
public:
    void run() {
        initWindow();
        initVulkan();
		initClasses();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;
    VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	VkRenderPass renderPass;
	
	VkDescriptorPool descriptorPool;

	VertexDescriptor 	phongAndSkyBoxVertices = VertexDescriptor(true, true, true, false, false);
	VertexDescriptor 	modelsVD = VertexDescriptor(true, false, false, false, false);
	VertexDescriptor 	textVertices = VertexDescriptor(true, false, true, false, false);

	// Phong pipeline
 	VkDescriptorSetLayout PhongDescriptorSetLayout;
  	VkPipelineLayout PhongPipelineLayout;
  	VkPipelineLayout TerrainPipelineLayout;
	VkPipeline PhongPipeline;
	VkPipeline TerrainPipeline;
	//// For the first uniform (per object)
	//We should have multiple buffers, because multiple frames may be in flight at the same time
    // and we don't want to update the buffer in preparation of the next frame while a previous one is still reading from it
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;

    std::vector<VkBuffer> terrainUniformBuffers;
    std::vector<VkDeviceMemory> terrainUniformBuffersMemory;
	//// For the second uniform (per scene)
	std::vector<VkBuffer> globalUniformBuffers;
	std::vector<VkDeviceMemory> globalUniformBuffersMemory;	
	// to access uniforms in the pipeline
	std::vector<VkDescriptorSet> PhongDescriptorSets;
	// scene graph using the Phong pipeline
	std::vector<SceneModel> scene;
	
	//  Skybox pipeline
 	VkDescriptorSetLayout SkyBoxDescriptorSetLayout; // for skybox
  	VkPipelineLayout SkyBoxPipelineLayout;	// for skybox
	VkPipeline SkyBoxPipeline;		// for skybox
	std::vector<VkBuffer> SkyBoxUniformBuffers;
	std::vector<VkDeviceMemory> SkyBoxUniformBuffersMemory;
	// to access uniforms in the pipeline
	std::vector<VkDescriptorSet> SkyBoxDescriptorSets;
	// Skybox definition, using the Skybox pipeline
	SceneModel SkyBox;

	//  Text pipeline
 	VkDescriptorSetLayout TextDescriptorSetLayout; 
  	VkPipelineLayout TextPipelineLayout;
	VkPipeline TextPipeline;	
	std::vector<VkBuffer> TextUniformBuffers;
	std::vector<VkDeviceMemory> TextUniformBuffersMemory;
	// to access uniforms in the pipeline
	std::vector<VkDescriptorSet> TextDescriptorSets;
	SceneModel SText;


	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	size_t currentFrame = 0;
	bool framebufferResized = false;

	// Questi dovrebbero essere i render target
	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;
	
	PhysicsEngine physicsEngine;
	Truck truck;
	
	// The current view, first or third person
	int currentView = 0;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<CGProject*>
						(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}    

    void initVulkan() {
		createInstance();
		setupDebugMessenger();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createImageViews();
		createRenderPass();
		createDescriptorSetLayouts();
		createPipelines();
		createCommandPool();
		createColorResources();
		createDepthResources();
		createFramebuffers();
		loadModels();
		createUniformBuffers();
		createDescriptorPool();
		createDescriptorSets();
		createCommandBuffers();
		createSyncObjects();
    }

    void createInstance() {
    	VkApplicationInfo appInfo{};
       	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    	appInfo.pApplicationName = "Computer graphics project";
    	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    	appInfo.pEngineName = "No Engine";
    	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;
		
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
				
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;

		createInfo.enabledLayerCount = 0;
		
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		 
		std::vector<VkExtensionProperties> extensionsPr(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, 
		 										extensionsPr.data());
		 
		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount =
			static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();		
		
		if (enableValidationLayers && !checkValidationLayerSupport()) {
			throw std::runtime_error("validation layers requested, but not available!");
		}

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		if (enableValidationLayers) {
			createInfo.enabledLayerCount =
				static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
			
			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)
									&debugCreateInfo;
		} else {
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}
		
		VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
		
		if(result != VK_SUCCESS) {
		 	PrintVkError(result);
			throw std::runtime_error("failed to create instance!");
		}
    }
    
    std::vector<const char*> getRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions =
			glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions,
			glfwExtensions + glfwExtensionCount);
		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		
		return extensions;
	}
	
	bool checkValidationLayerSupport() {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount,
					availableLayers.data());

		for (const char* layerName : validationLayers) {
			bool layerFound = false;
			
			for (const auto& layerProperties : availableLayers) {
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

    void populateDebugMessengerCreateInfo(
    		VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    	createInfo = {};
		createInfo.sType =
			VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity =
//			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | // disabled in the tutorial
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr;
    }

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {

		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;		
		return VK_FALSE;
	}

	void setupDebugMessenger() {
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		populateDebugMessengerCreateInfo(createInfo);
		
		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr,
				&debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

    void createSurface() {
    	if (glfwCreateWindowSurface(instance, window, nullptr, &surface)
    			!= VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
    }

    void pickPhysicalDevice() {
    	uint32_t deviceCount = 0;
    	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    	 
    	if (deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}
		
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
		
		std::cout << "Physical devices found: " << deviceCount << "\n";
		
		for (const auto& device : devices) {
			if (isDeviceSuitable(device)) {
				physicalDevice = device;
				msaaSamples = getMaxUsableSampleCount();
				std::cout << "\n\nMaximum samples for anti-aliasing: " << msaaSamples << "\n\n\n";
				break;
			}
		}
		
		if (physicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}
    }

    bool isDeviceSuitable(VkPhysicalDevice device) {
 		QueueFamilyIndices indices = findQueueFamilies(device);

		bool extensionsSupported = checkDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionsSupported) {
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() &&
								!swapChainSupport.presentModes.empty();
		}
		
		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
		
		return indices.isComplete() && extensionsSupported && swapChainAdequate &&
						supportedFeatures.samplerAnisotropy;
	}
    
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
		QueueFamilyIndices indices;
		
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
						nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
								queueFamilies.data());
								
		int i=0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}
				
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface,
												 &presentSupport);
			if (presentSupport) {
			 	indices.presentFamily = i;
			}

			if (indices.isComplete()) {
				break;
			}			
			i++;
		}

		return indices;
	}

	bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr,
					&extensionCount, nullptr);
					
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr,
					&extensionCount, availableExtensions.data());
					
		std::set<std::string> requiredExtensions(deviceExtensions.begin(),
					deviceExtensions.end());
					
		for (const auto& extension : availableExtensions){
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails details;
		
		 vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
		 		&details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
				nullptr);
				
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface,
					&formatCount, details.formats.data());
		}
		
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
			&presentModeCount, nullptr);
		
		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
					&presentModeCount, details.presentModes.data());
		}
		 
		return details;
	}

	VkSampleCountFlagBits getMaxUsableSampleCount() {
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
		
		VkSampleCountFlags counts =
				physicalDeviceProperties.limits.framebufferColorSampleCounts &
				physicalDeviceProperties.limits.framebufferDepthSampleCounts;
		
		if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
		if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
		if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
		if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
		if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
		if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

		return VK_SAMPLE_COUNT_1_BIT;
	}	
	
	void createLogicalDevice() {
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies =
				{indices.graphicsFamily.value(), indices.presentFamily.value()};
		
		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}
		
		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		deviceFeatures.sampleRateShading = VK_TRUE;
		deviceFeatures.fillModeNonSolid = VK_TRUE;
		
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = 
			static_cast<uint32_t>(queueCreateInfos.size());
		
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount =
				static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = 
					static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		} else {
			createInfo.enabledLayerCount = 0;
		}
		
		VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
		
		if (result != VK_SUCCESS) {
		 	PrintVkError(result);
			throw std::runtime_error("failed to create logical device!");
		}
		
		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
	}
	
	void createSwapChain() {
		SwapChainSupportDetails swapChainSupport =
				querySwapChainSupport(physicalDevice);
		VkSurfaceFormatKHR surfaceFormat =
				chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode =
				chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
		
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		
		if (swapChainSupport.capabilities.maxImageCount > 0 &&
				imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}
		
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
										 indices.presentFamily.value()};
		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}
		
		 createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		 createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		 createInfo.presentMode = presentMode;
		 createInfo.clipped = VK_TRUE;
		 createInfo.oldSwapchain = VK_NULL_HANDLE;
		 
		 VkResult result = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain);
		 if (result != VK_SUCCESS) {
		 	PrintVkError(result);
			throw std::runtime_error("failed to create swap chain!");
		}
		
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount,
				swapChainImages.data());
				
		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(
				const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
				availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}
		
		return availableFormats[0];
	}

	VkPresentModeKHR chooseSwapPresentMode(
			const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}
	
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		} else {
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			
			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};
			actualExtent.width = std::max(capabilities.minImageExtent.width,
					std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height,
					std::min(capabilities.maxImageExtent.height, actualExtent.height));
			return actualExtent;
		}
	}

	void createImageViews() {
		swapChainImageViews.resize(swapChainImages.size());
		
		for (size_t i = 0; i < swapChainImages.size(); i++) {
			swapChainImageViews[i] = createImageView(swapChainImages[i],
													 swapChainImageFormat,
													 VK_IMAGE_ASPECT_COLOR_BIT, 1,
													 VK_IMAGE_VIEW_TYPE_2D, 1);
		}
	}
	
	VkImageView createImageView(VkImage image, VkFormat format,
								VkImageAspectFlags aspectFlags,
								uint32_t mipLevels, VkImageViewType type, int layerCount) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = type;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = layerCount;
		VkImageView imageView;

		VkResult result = vkCreateImageView(device, &viewInfo, nullptr,
				&imageView);
		if (result != VK_SUCCESS) {
		 	PrintVkError(result);
			throw std::runtime_error("failed to create image view!");
		}
		return imageView;
	}

    void createRenderPass() {
		VkAttachmentDescription colorAttachmentResolve{};
		colorAttachmentResolve.format = swapChainImageFormat;
		colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentResolveRef{};
		colorAttachmentResolveRef.attachment = 2;
		colorAttachmentResolveRef.layout =
						VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		
		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = findDepthFormat();
		depthAttachment.samples = msaaSamples;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout =
						VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = 
						VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    	VkAttachmentDescription colorAttachment{};
		colorAttachment.format = swapChainImageFormat;
		colorAttachment.samples = msaaSamples;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout =
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		subpass.pResolveAttachments = &colorAttachmentResolveRef;
		
		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 3> attachments =
								{colorAttachment, depthAttachment,
								 colorAttachmentResolve};

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VkResult result = vkCreateRenderPass(device, &renderPassInfo, nullptr,
					&renderPass);
		if (result != VK_SUCCESS) {
		 	PrintVkError(result);
			throw std::runtime_error("failed to create render pass!");
		}		
	}

	void createDescriptorSetLayouts() {
		createPhongDescriptorSetLayout();
		createSkyBoxDescriptorSetLayout();
		createTextDescriptorSetLayout();
	}
	
	void createPhongDescriptorSetLayout() {
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		
		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		
		VkDescriptorSetLayoutBinding globalUboLayoutBinding{};
		globalUboLayoutBinding.binding = 2;
		globalUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		globalUboLayoutBinding.descriptorCount = 1;
		globalUboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
		globalUboLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding terrainUboLayoutBinding{};
        terrainUboLayoutBinding.binding = 3;
        terrainUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        terrainUboLayoutBinding.descriptorCount = 1;
        terrainUboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
        uboLayoutBinding.pImmutableSamplers = nullptr;

        std::array<VkDescriptorSetLayoutBinding, 4> bindings = {uboLayoutBinding, samplerLayoutBinding, globalUboLayoutBinding,terrainUboLayoutBinding};
		
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();
		
		VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &PhongDescriptorSetLayout);
		if (result != VK_SUCCESS) {
			PrintVkError(result);
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	void createSkyBoxDescriptorSetLayout() {
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		
		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorType =
						VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		
		std::array<VkDescriptorSetLayoutBinding, 2> bindings =
							{uboLayoutBinding, samplerLayoutBinding};
		
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();
		
		VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo,
									nullptr, &SkyBoxDescriptorSetLayout);
		if (result != VK_SUCCESS) {
			PrintVkError(result);
			throw std::runtime_error("failed to create SkyBox descriptor set layout!");
		}
	}

	void createTextDescriptorSetLayout() {
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		
		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorType =
						VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		
		std::array<VkDescriptorSetLayoutBinding, 2> bindings =
							{uboLayoutBinding, samplerLayoutBinding};
		
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();
		
		VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo,
									nullptr, &TextDescriptorSetLayout);
		if (result != VK_SUCCESS) {
			PrintVkError(result);
			throw std::runtime_error("failed to create Text descriptor set layout!");
		}
	}

 	void createPipelines() {
 		createPhongPipeline();
        createTerrainPipeline();
 		createSkyBoxPipeline();
 		createTextPipeline();
 	}
 	
 	void createPhongPipeline() {
 		createPipeline("PhongVert.spv", "PhongFrag.spv",
 					    PhongPipelineLayout, PhongPipeline,
 					    PhongDescriptorSetLayout, VK_COMPARE_OP_LESS, VK_POLYGON_MODE_FILL,
 					    1.0, VK_CULL_MODE_BACK_BIT, false, phongAndSkyBoxVertices);
 	}
    void createTerrainPipeline(){
        createPipeline("TerrainVert.spv", "TerrainFrag.spv",
                       TerrainPipelineLayout, TerrainPipeline,
                       PhongDescriptorSetLayout, VK_COMPARE_OP_LESS, VK_POLYGON_MODE_FILL,
                       1.0, VK_CULL_MODE_BACK_BIT, false, phongAndSkyBoxVertices);
    }

 	void createSkyBoxPipeline() {
 		createPipeline("SkyBoxVert.spv", "SkyBoxFrag.spv",
 					    SkyBoxPipelineLayout, SkyBoxPipeline,
 					    SkyBoxDescriptorSetLayout, VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
 					    1.0, VK_CULL_MODE_BACK_BIT, false, phongAndSkyBoxVertices);
 	}

 	void createTextPipeline() {
 		createPipeline("TextVert.spv", "TextFrag.spv",
 					    TextPipelineLayout, TextPipeline,
 					    TextDescriptorSetLayout, VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL,
 					    1.0, VK_CULL_MODE_NONE, true, textVertices);
 	}

 	void createPipeline(const char* VertexShaderName, const char* FragShaderName,
 						VkPipelineLayout& PipelineLayout,
 						VkPipeline& Pipeline,
 						VkDescriptorSetLayout &descriptorSetLayout,
 						VkCompareOp compareOp, VkPolygonMode polyModel, float LW,
 						VkCullModeFlagBits CM, bool transp,
 						VertexDescriptor &VD) {
		auto vertShaderCode = readFile((SHADER_PATH + VertexShaderName).c_str());
		auto fragShaderCode = readFile((SHADER_PATH + FragShaderName).c_str());
		
		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		auto bindingDescription = VD.getBindingDescription();
		auto attributeDescriptions = VD.getAttributeDescriptions();
				
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float) swapChainExtent.width;
		viewport.height = (float) swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		
		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = swapChainExtent;
		
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;
		
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = polyModel;
		rasterizer.lineWidth = LW;
		rasterizer.cullMode = CM;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
		
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_TRUE;
		multisampling.minSampleShading = .2f;
		multisampling.rasterizationSamples = msaaSamples;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional
		
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask =
				VK_COLOR_COMPONENT_R_BIT |
				VK_COLOR_COMPONENT_G_BIT |
				VK_COLOR_COMPONENT_B_BIT |
				VK_COLOR_COMPONENT_A_BIT;

		colorBlendAttachment.blendEnable = transp ? VK_TRUE : VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor =
				transp ? VK_BLEND_FACTOR_SRC_ALPHA : VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstColorBlendFactor =
				transp ? VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA : VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.colorBlendOp =
				VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment.srcAlphaBlendFactor =
				VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstAlphaBlendFactor =
				VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.alphaBlendOp =
				VK_BLEND_OP_ADD; // Optional

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType =
				VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional
		
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
		
		VkResult result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr,
					&PipelineLayout);
		if (result != VK_SUCCESS) {
		 	PrintVkError(result);
			throw std::runtime_error("failed to create pipeline layout!");
		}
		
		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = compareOp;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f; // Optional
		depthStencil.maxDepthBounds = 1.0f; // Optional
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {}; // Optional
		depthStencil.back = {}; // Optional

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr; // Optional
		pipelineInfo.layout = PipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional
		
		result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1,
				&pipelineInfo, nullptr, &Pipeline);
		if (result != VK_SUCCESS) {
		 	PrintVkError(result);
			throw std::runtime_error("failed to create graphics pipeline!");
		}
		
		vkDestroyShaderModule(device, fragShaderModule, nullptr);
		vkDestroyShaderModule(device, vertShaderModule, nullptr);
	}

	static std::vector<char> readFile(const std::string& filename) {
        std::string s = "failed to open file!"+filename;
			std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
			throw std::runtime_error(s);
		}
		
		size_t fileSize = (size_t) file.tellg();
		std::vector<char> buffer(fileSize);
		 
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		 
		file.close();
		 
		return buffer;
	}

	VkShaderModule createShaderModule(const std::vector<char>& code) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
		
		VkShaderModule shaderModule;

		VkResult result = vkCreateShaderModule(device, &createInfo, nullptr,
						&shaderModule);
		if (result != VK_SUCCESS) {
		 	PrintVkError(result);
			throw std::runtime_error("failed to create shader module!");
		}
		
		return shaderModule;
	}

    void createFramebuffers() {
		swapChainFramebuffers.resize(swapChainImageViews.size());
		for (size_t i = 0; i < swapChainImageViews.size(); i++) {
			std::array<VkImageView, 3> attachments = {
				colorImageView,
				depthImageView,
				swapChainImageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType =
				VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount =
							static_cast<uint32_t>(attachments.size());;
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = swapChainExtent.width; 
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;
			
			VkResult result = vkCreateFramebuffer(device, &framebufferInfo, nullptr,
						&swapChainFramebuffers[i]);
			if (result != VK_SUCCESS) {
			 	PrintVkError(result);
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}

    void createCommandPool() {
    	QueueFamilyIndices queueFamilyIndices = 
    			findQueueFamilies(physicalDevice);
    			
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
		poolInfo.flags = 0; // Optional
		
		VkResult result = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
		if (result != VK_SUCCESS) {
		 	PrintVkError(result);
			throw std::runtime_error("failed to create command pool!");
		}
	}

	void createColorResources() {
		VkFormat colorFormat = swapChainImageFormat;
		createImage(swapChainExtent.width, swapChainExtent.height, 1,
					msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
					VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					colorImage, colorImageMemory);
		colorImageView = createImageView(colorImage, colorFormat,
									VK_IMAGE_ASPECT_COLOR_BIT, 1,
									VK_IMAGE_VIEW_TYPE_2D, 1);
	}

	void createDepthResources() {
		VkFormat depthFormat = findDepthFormat();
		
		createImage(swapChainExtent.width, swapChainExtent.height, 1, 
					msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					depthImage, depthImageMemory);
		depthImageView = createImageView(depthImage, depthFormat,
										 VK_IMAGE_ASPECT_DEPTH_BIT, 1,
										 VK_IMAGE_VIEW_TYPE_2D, 1);

		transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED,
							  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, 1);
	}

	VkFormat findDepthFormat() {
		return findSupportedFormat({VK_FORMAT_D32_SFLOAT,
									VK_FORMAT_D32_SFLOAT_S8_UINT,
									VK_FORMAT_D24_UNORM_S8_UINT},
									VK_IMAGE_TILING_OPTIMAL, 
								VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
	}
	
	VkFormat findSupportedFormat(const std::vector<VkFormat> candidates,
						VkImageTiling tiling, VkFormatFeatureFlags features) {
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
			if (tiling == VK_IMAGE_TILING_LINEAR &&
						(props.linearTilingFeatures & features) == features) {
				return format;
			} else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
						(props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}
		
		throw std::runtime_error("failed to find supported format!");
	}
	
	bool hasStencilComponent(VkFormat format) {
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
			   format == VK_FORMAT_D24_UNORM_S8_UINT;
	}
		
	void createTextureImage(const char* FName, TextureData& TD) {
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load((TEXTURE_PATH + FName).c_str(), &texWidth, &texHeight,
							&texChannels, STBI_rgb_alpha);
		if (!pixels) {
			std::cout << (TEXTURE_PATH + FName).c_str() << "\n";
			throw std::runtime_error("failed to load texture image!");
		}
		std::cout << FName << " -> size: " << texWidth
				  << "x" << texHeight << ", ch: " << texChannels <<"\n";

		VkDeviceSize imageSize = texWidth * texHeight * 4;
		TD.mipLevels = static_cast<uint32_t>(std::floor(
						std::log2(std::max(texWidth, texHeight)))) + 1;
		
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		 
		createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		  						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		  						VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		  						stagingBuffer, stagingBufferMemory);
		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		vkUnmapMemory(device, stagingBufferMemory);
		
		stbi_image_free(pixels);
		
		createImage(texWidth, texHeight, TD.mipLevels,
					VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB,
					VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
					VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, TD.textureImage,
					TD.textureImageMemory);
					
		transitionImageLayout(TD.textureImage, VK_FORMAT_R8G8B8A8_SRGB,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, TD.mipLevels, 1);
		copyBufferToImage(stagingBuffer, TD.textureImage,
				static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1);

		generateMipmaps(TD.textureImage, VK_FORMAT_R8G8B8A8_SRGB,
						texWidth, texHeight, TD.mipLevels, 1);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}
	
	
	void createCubicTextureImage(const char *const FName[6], TextureData& TD) {
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels[6];

		for(int i = 0; i < 6; i++) {		
			pixels[i] = stbi_load((TEXTURE_PATH + FName[i]).c_str(), &texWidth, &texHeight,
								&texChannels, STBI_rgb_alpha);
			if (!pixels[i]) {
				std::cout << (TEXTURE_PATH + FName[i]).c_str() << "\n";
				throw std::runtime_error("failed to load texture image!");
			}
			std::cout << FName[i] << " -> size: " << texWidth
					  << "x" << texHeight << ", ch: " << texChannels <<"\n";
		}	

		VkDeviceSize imageSize = texWidth * texHeight * 4;
		VkDeviceSize totalImageSize = texWidth * texHeight * 4 * 6;
		TD.mipLevels = static_cast<uint32_t>(std::floor(
						std::log2(std::max(texWidth, texHeight)))) + 1;
		
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		 
		createBuffer(totalImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		  						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		  						VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		  						stagingBuffer, stagingBufferMemory);
		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, totalImageSize, 0, &data);
		for(int i = 0; i < 6; i++) {
			memcpy(static_cast<char *>(data) + imageSize * i, pixels[i], static_cast<size_t>(imageSize));
		}
		vkUnmapMemory(device, stagingBufferMemory);
			
		for(int i = 0; i < 6; i++) {
			stbi_image_free(pixels[i]);
		}			
		createSkyBoxImage(texWidth, texHeight, TD.mipLevels, TD.textureImage,
					TD.textureImageMemory);
					
		transitionImageLayout(TD.textureImage, VK_FORMAT_R8G8B8A8_SRGB,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, TD.mipLevels, 6);
		copyBufferToImage(stagingBuffer, TD.textureImage,
				static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 6);

		generateMipmaps(TD.textureImage, VK_FORMAT_R8G8B8A8_SRGB,
						texWidth, texHeight, TD.mipLevels, 6);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

	void generateMipmaps(VkImage image, VkFormat imageFormat,
						 int32_t texWidth, int32_t texHeight,
						 uint32_t mipLevels, int layerCount) {
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat,
							&formatProperties);

		if (!(formatProperties.optimalTilingFeatures &
					VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
			throw std::runtime_error("texture image format does not support linear blitting!");
		}

		VkCommandBuffer commandBuffer = beginSingleTimeCommands();
		
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = layerCount;
		barrier.subresourceRange.levelCount = 1;

		int32_t mipWidth = texWidth;
		int32_t mipHeight = texHeight;

		for (uint32_t i = 1; i < mipLevels; i++) { 
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			
			vkCmdPipelineBarrier(commandBuffer,
								 VK_PIPELINE_STAGE_TRANSFER_BIT,
								 VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
								 0, nullptr, 0, nullptr,
								 1, &barrier);

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = layerCount;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1,
								   mipHeight > 1 ? mipHeight/2:1, 1};
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = layerCount;
			
			vkCmdBlitImage(commandBuffer, image,
						   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						   image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
						   &blit, VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			
			vkCmdPipelineBarrier(commandBuffer,
								 VK_PIPELINE_STAGE_TRANSFER_BIT,
								 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
								 0, nullptr, 0, nullptr,
								 1, &barrier);
			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;
		}

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		vkCmdPipelineBarrier(commandBuffer,
							 VK_PIPELINE_STAGE_TRANSFER_BIT,
							 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
							 0, nullptr, 0, nullptr,
							 1, &barrier);

		endSingleTimeCommands(commandBuffer);
	}


	void createSkyBoxImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkImage& image,
				 	 VkDeviceMemory& imageMemory) {		
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = 6;
		imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		
		VkResult result = vkCreateImage(device, &imageInfo, nullptr, &image);
		if (result != VK_SUCCESS) {
		 	PrintVkError(result);
		 	throw std::runtime_error("failed to create image!");
		}
		
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
											VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) !=
								VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(device, image, imageMemory, 0);
	}

							 
	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
					 VkSampleCountFlagBits numSamples, VkFormat format, 
					 VkImageTiling tiling, VkImageUsageFlags usage,
				 	 VkMemoryPropertyFlags properties, VkImage& image,
				 	 VkDeviceMemory& imageMemory) {		
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = numSamples;
		imageInfo.flags = 0; // Optional
		
		VkResult result = vkCreateImage(device, &imageInfo, nullptr, &image);
		if (result != VK_SUCCESS) {
		 	PrintVkError(result);
		 	throw std::runtime_error("failed to create image!");
		}
		
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
											properties);
		if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) !=
								VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(device, image, imageMemory, 0);
	}

	void transitionImageLayout(VkImage image, VkFormat format,
					VkImageLayout oldLayout, VkImageLayout newLayout,
					uint32_t mipLevels, int layersCount) {
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		
		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (hasStencilComponent(format)) {
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}else{
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = layersCount;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
					newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
				   newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		} else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && 
				   newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
									VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		} else {
			throw std::invalid_argument("unsupported layout transition!");
		}
		vkCmdPipelineBarrier(commandBuffer,
								sourceStage, destinationStage, 0,
								0, nullptr, 0, nullptr, 1, &barrier);

		endSingleTimeCommands(commandBuffer);
	}
	
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t
						   width, uint32_t height, int layerCount) {
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();
		
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = layerCount;
		region.imageOffset = {0, 0, 0};
		region.imageExtent = {width, height, 1};
		
		vkCmdCopyBufferToImage(commandBuffer, buffer, image,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		endSingleTimeCommands(commandBuffer);
	}
	
	void createTextureImageView(TextureData& TD) {
		TD.textureImageView = createImageView(TD.textureImage,
										   VK_FORMAT_R8G8B8A8_SRGB,
										   VK_IMAGE_ASPECT_COLOR_BIT,
										   TD.mipLevels,
										   VK_IMAGE_VIEW_TYPE_2D, 1);
	}
	
	void createSkyBoxImageView(TextureData& TD) {
		TD.textureImageView = createImageView(TD.textureImage,
										   VK_FORMAT_R8G8B8A8_SRGB,
										   VK_IMAGE_ASPECT_COLOR_BIT,
										   TD.mipLevels,
										   VK_IMAGE_VIEW_TYPE_CUBE, 6);
	}
	
	void createTextureSampler(TextureData& TD) {
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 16;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = static_cast<float>(TD.mipLevels);
		
		VkResult result = vkCreateSampler(device, &samplerInfo, nullptr,
										  &TD.textureSampler);
		if (result != VK_SUCCESS) {
		 	PrintVkError(result);
		 	throw std::runtime_error("failed to create texture sampler!");
		}
	}
	Object* treeContainer1 = new Object("treeContainer", mat4(1));
	Object* treeContainer2 = new Object("treeContainer", mat4(1));
	Object* treeContainer3 = new Object("treeContainer", mat4(1));
	Object* treeContainer4 = new Object("treeContainer", mat4(1));
	Object* treeContainer5 = new Object("treeContainer", mat4(1));
	Object* treeContainer6 = new Object("treeContainer", mat4(1));
	Object* treeContainer7 = new Object("treeContainer", mat4(1));
	Object* treeContainer8 = new Object("treeContainer", mat4(1));
	Object* treeContainer9 = new Object("treeContainer", mat4(1));
	std::vector<Object*> treeContainer;
	
	

	int treeGridLength = 40;
	void loadModels() {
        sceneToLoad.addObject("terrain", {"floor.obj", "grass6.jpg", 1, Terrain}, mat4(1));
		sceneToLoad.addObject(*treeContainer1);
		sceneToLoad.addObject(*treeContainer2);
		sceneToLoad.addObject(*treeContainer3);
		sceneToLoad.addObject(*treeContainer4);
		sceneToLoad.addObject(*treeContainer5);
		sceneToLoad.addObject(*treeContainer6);
		sceneToLoad.addObject(*treeContainer7);
		sceneToLoad.addObject(*treeContainer8);
		sceneToLoad.addObject(*treeContainer9);

        ///Test objects
        //Object* pyramid = sceneToLoad.addObject("pyr", {"pyramid.obj", "Colors.png", 1, Flat}, translate(mat4(1), vec3(0, 0, 0)));
        //Object* walls = pyramid->addObject("walls", {"Walls.obj", "grass2.jpg", 1, Flat}, translate(mat4(1), vec3(10, 0, 0)));
        //Object* walls2 = walls->addObject("walls2", {"Walls.obj", "Colors.png", 1, Flat}, translate(mat4(1), vec3(11, 10, 0)));
        ///
		//add 10 tree objects inside treeContainer
		for (int i = -6; i < -2; i++) {
			for (int j = -6; j < -2; j++) {
				int x = treeGridLength * i;

				int z = treeGridLength * j;

				treeContainer1->addObject("tre"//+std::to_string(x)+ std::to_string(z),
					, { "tree.obj", "Colors.png", 1, Flat }, translate(mat4(1), vec3(x, getHeight(x, z), z)));
			}
		}

		for (int i = -2; i < 2; i++) {
			for (int j = -6; j < -2; j++) {
				int x = treeGridLength * i;

				int z = treeGridLength * j;

				treeContainer2->addObject("tre"//+std::to_string(x)+ std::to_string(z),
					, { "tree.obj", "Colors.png", 1, Flat }, translate(mat4(1), vec3(x, getHeight(x, z), z)));
			}
		}
		
		for (int i = 2; i < 6; i++) {
			for (int j = -6; j < -2; j++) {
				int x = treeGridLength * i;

				int z = treeGridLength * j;

				treeContainer3->addObject("tre"//+std::to_string(x)+ std::to_string(z),
					, { "tree.obj", "Colors.png", 1, Flat }, translate(mat4(1), vec3(x, getHeight(x, z), z)));
			}
		}


		for (int i = -6; i < -2; i++) {
			for (int j = -2; j < 2; j++) {
				int x = treeGridLength * i;

				int z = treeGridLength * j;

				treeContainer4->addObject("tre"//+std::to_string(x)+ std::to_string(z),
					, { "tree.obj", "Colors.png", 1, Flat }, translate(mat4(1), vec3(x, getHeight(x, z), z)));
			}
		}

		for (int i = -2; i < 2; i++) {
			for (int j = -2; j < 2; j++) {
				int x = treeGridLength * i;

				int z = treeGridLength * j;
				
				treeContainer5->addObject("tre"//+std::to_string(x)+ std::to_string(z),
					,{"tree.obj", "Colors.png", 1, Flat}, translate(mat4(1), vec3(x, getHeight(x, z), z)));
			}
		}
		
		for (int i = 2; i < 6; i++) {
			for (int j = -2; j < 2; j++) {
				int x = treeGridLength * i;

				int z = treeGridLength * j;

				treeContainer6->addObject("tre"//+std::to_string(x)+ std::to_string(z),
					, { "tree.obj", "Colors.png", 1, Flat }, translate(mat4(1), vec3(x, getHeight(x, z), z)));
			}
		}
		
		for (int i = -6; i < -2; i++) {
			for (int j = 2; j < 6; j++) {
				int x = treeGridLength * i;

				int z = treeGridLength * j;

				treeContainer7->addObject("tre"//+std::to_string(x)+ std::to_string(z),
					, { "tree.obj", "Colors.png", 1, Flat }, translate(mat4(1), vec3(x, getHeight(x, z), z)));
			}
		}
		for (int i = -2; i < 2; i++) {
			for (int j = 2; j < 6; j++) {
				int x = treeGridLength * i;

				int z = treeGridLength * j;

				treeContainer8->addObject("tre"//+std::to_string(x)+ std::to_string(z),
					, { "tree.obj", "Colors.png", 1, Flat }, translate(mat4(1), vec3(x, getHeight(x, z), z)));
			}
		}

		for (int i = 2; i < 6; i++) {
			for (int j = 2; j < 6; j++) {
				int x = treeGridLength * i;

				int z = treeGridLength * j;

				treeContainer9->addObject("tre"//+std::to_string(x)+ std::to_string(z),
					, { "tree.obj", "Colors.png", 1, Flat }, translate(mat4(1), vec3(x, getHeight(x, z), z)));
			}
		}

		
		//pushback all treeContainer objects into vector
		treeContainer.push_back(treeContainer1);
		treeContainer.push_back(treeContainer2);
		treeContainer.push_back(treeContainer3);
		treeContainer.push_back(treeContainer4);
		treeContainer.push_back(treeContainer5);
		treeContainer.push_back(treeContainer6);
		treeContainer.push_back(treeContainer7);
		treeContainer.push_back(treeContainer8);
		treeContainer.push_back(treeContainer9);
		
		

        sceneToLoad.addObject(truck);
        Object* leftLight = new Object("leftLight",translate(mat4(1),vec3(-0.4,1,-0.5)));
        Object* rightLight = new Object("rightLight",translate(mat4(1),vec3(0.4,1,-0.5)));
        truck.addObject(*leftLight);
        truck.addObject(*rightLight);


        //std::cout<<"Scene size is :"<<sceneToLoad.countChildrenWithModels()<<std::endl;
		scene.resize(Object::objs.size());
        std::vector<Object*> sct = Object::objs;
		for (int i = 0;i<sct.size();i++ ) {
			loadModelWithTexture(*sct[i], i);
		}


		loadSkyBox();
		createTexts();

	}
	
	void loadModelWithTexture(const Object &M, int i) {
        loadMesh(M.model.ObjFile, scene[i].MD, phongAndSkyBoxVertices, vec3(0,0,0));
		createVertexBuffer(scene[i].MD);
		createIndexBuffer(scene[i].MD);
		
		createTextureImage(M.model.TextureFile, scene[i].TD);
		createTextureImageView(scene[i].TD);
		createTextureSampler(scene[i].TD);
	}
	
	void loadSkyBox() {
        loadMesh(SkyBoxToLoad.ObjFile, SkyBox.MD, phongAndSkyBoxVertices, vec3(0, 0, 0));
		createVertexBuffer(SkyBox.MD);
		createIndexBuffer(SkyBox.MD);

		createCubicTextureImage(SkyBoxToLoad.TextureFile, SkyBox.TD);
		createSkyBoxImageView(SkyBox.TD);
		createTextureSampler(SkyBox.TD);
	}
	
	void createTexts() {
		createTextMesh(SText.MD, textVertices);
		createVertexBuffer(SText.MD);
		createIndexBuffer(SText.MD);

		createTextureImage("Fonts.png", SText.TD);
		createTextureImageView(SText.TD);
		createTextureSampler(SText.TD);
	}
	
	void loadMesh(const char *FName, ModelData &MD, VertexDescriptor &VD, vec3 translation) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                              (MODEL_PATH + FName).c_str())) {
            throw std::runtime_error(warn + err);
        }

        MD.vertDesc = &VD;

        std::cout << FName << "\n";

        //Used to temporarily store vertices when copying them
        std::vector<float> vertexToCopy{};
        vertexToCopy.resize(VD.size);


        if (FName == "floor.obj") {

            makeModels();
            for (unsigned int M1_indice : M1_indices) {
                //Assigns vertexToCopy and indices to MD
                //In case of vetex with normal and
                //DeltaPos is 0
                //DeltaNormal is 3
                //deltaTextCoords is 6
                //printf("The index is %d\n",M1_indice);

                vertexToCopy[VD.deltaPos + 0] = M1_vertices[3 * M1_indice + 0];
                //std::cout << " vtx x " << vertexToCopy[VD.deltaPos + 0];

                vertexToCopy[VD.deltaPos + 1] = float(M1_indice);
                //std::cout << " vtx y " << vertexToCopy[VD.deltaPos + 1];

                vertexToCopy[VD.deltaPos + 2] = M1_vertices[3 * M1_indice + 2];
                //std::cout << " vtx z " << vertexToCopy[VD.deltaPos + 2];

                vertexToCopy[VD.deltaNormal + 0] = 0;
                vertexToCopy[VD.deltaNormal + 1] = 1;
                vertexToCopy[VD.deltaNormal + 2] = 0;

                vertexToCopy[VD.deltaTexCoord + 0] = 0;
                vertexToCopy[VD.deltaTexCoord + 1] = 0;



                int j = MD.vertices.size() / VD.size;
                int s = MD.vertices.size();
                //std::cout << " J " << j << "  MD VERTICE SIZE " << s << " vd size " << VD.size;
                MD.vertices.resize(s + VD.size);
                for (int k = 0; k < VD.size; k++) {
                    MD.vertices[s + k] = vertexToCopy[k];
                }
                MD.indices.push_back(j);
            }
        } else {

            //		std::unordered_map<std::vector<float>, uint32_t> uniqueVertices{};
            for (const auto &shape: shapes) {
                for (const auto &index: shape.mesh.indices) {

                    vertexToCopy[VD.deltaPos + 0] = attrib.vertices[3 * index.vertex_index + 0]+translation.x;
                    vertexToCopy[VD.deltaPos + 1] = attrib.vertices[3 * index.vertex_index + 1]+translation.y;
                    vertexToCopy[VD.deltaPos + 2] = attrib.vertices[3 * index.vertex_index + 2]+translation.z;
                    vertexToCopy[VD.deltaNormal + 0] = attrib.normals[3 * index.normal_index + 0];
                    vertexToCopy[VD.deltaNormal + 1] = attrib.normals[3 * index.normal_index + 1];
                    vertexToCopy[VD.deltaNormal + 2] = attrib.normals[3 * index.normal_index + 2];
                   //if tex coord empty assign 0
					if (attrib.texcoords.size() == 0) {
						vertexToCopy[VD.deltaTexCoord + 0] = 0;
						vertexToCopy[VD.deltaTexCoord + 1] = 0;
					}
					else {
						vertexToCopy[VD.deltaTexCoord + 0] = attrib.texcoords[2 * index.texcoord_index + 0];
						vertexToCopy[VD.deltaTexCoord + 1] = 1 - attrib.texcoords[2 * index.texcoord_index + 1];
					}
						int j = MD.vertices.size() / VD.size;
						int s = MD.vertices.size();
						MD.vertices.resize(s + VD.size);
						for (int k = 0; k < VD.size; k++) {
							MD.vertices[s + k] = vertexToCopy[k];
						}
						MD.indices.push_back(j);
				}
            }

            std::cout << FName << " -> V: " << MD.vertices.size()
                      << ", I: " << MD.indices.size() << "\n";
			
		}
    }
        void createTextMesh(ModelData &MD, VertexDescriptor &VD) {
            MD.vertDesc = &VD;

            int totLen = 0;
            for (auto &Txt: SceneText) {
                for (int i = 0; i < Txt.usedLines; i++) {
                    totLen += strlen(Txt.l[i]);
                }
            }
            std::cout << "Total characters: " << totLen << "\n";

            MD.vertices.resize(4 * VD.size * totLen);
            MD.indices.resize(6 * totLen);

            int FontId = 1;

            float PtoTdx = -0.95;
            float PtoTdy = -0.95;
            float PtoTsx = 2.0 / 800.0;
            float PtoTsy = 2.0 / 600.0;

            int minChar = 32;
            int maxChar = 127;
            int texW = 1024;
            int texH = 512;

            int tpx = 0;
            int tpy = 0;

            int vb = 0, ib = 0, k = 0;
            for (auto &Txt: SceneText) {
                Txt.start = ib;
                for (int i = 0; i < Txt.usedLines; i++) {
                    for (int j = 0; j < strlen(Txt.l[i]); j++) {
                        int c = ((int) Txt.l[i][j]) - minChar;
                        if ((c >= 0) && (c <= maxChar)) {
//std::cout << k << " " << j << " " << i << " " << vb << " " << ib << " " << c << "\n";
                            CharData d = Fonts[FontId].P[c];

                            MD.vertices[vb + 0] = (float) (tpx + d.xoffset) * PtoTsx + PtoTdx;
                            MD.vertices[vb + 1] = (float) (tpy + d.yoffset) * PtoTsy + PtoTdy;
                            MD.vertices[vb + 2] = 0.0;
                            MD.vertices[vb + 3] = (float) d.x / texW;
                            MD.vertices[vb + 4] = (float) d.y / texH;

                            MD.vertices[vb + 5] = (float) (tpx + d.xoffset + d.width) * PtoTsx + PtoTdx;
                            MD.vertices[vb + 6] = (float) (tpy + d.yoffset) * PtoTsy + PtoTdy;
                            MD.vertices[vb + 7] = 0.0;
                            MD.vertices[vb + 8] = (float) (d.x + d.width) / texW;
                            MD.vertices[vb + 9] = (float) d.y / texH;

                            MD.vertices[vb + 10] = (float) (tpx + d.xoffset) * PtoTsx + PtoTdx;
                            MD.vertices[vb + 11] = (float) (tpy + d.yoffset + d.height) * PtoTsy + PtoTdy;
                            MD.vertices[vb + 12] = 0.0;
                            MD.vertices[vb + 13] = (float) d.x / texW;
                            MD.vertices[vb + 14] = (float) (d.y + d.height) / texH;

                            MD.vertices[vb + 15] = (float) (tpx + d.xoffset + d.width) * PtoTsx + PtoTdx;
                            MD.vertices[vb + 16] = (float) (tpy + d.yoffset + d.height) * PtoTsy + PtoTdy;
                            MD.vertices[vb + 17] = 0.0;
                            MD.vertices[vb + 18] = (float) (d.x + d.width) / texW;
                            MD.vertices[vb + 19] = (float) (d.y + d.height) / texH;

                            MD.indices[ib + 0] = 4 * k + 0;
                            MD.indices[ib + 1] = 4 * k + 1;
                            MD.indices[ib + 2] = 4 * k + 2;
                            MD.indices[ib + 3] = 4 * k + 1;
                            MD.indices[ib + 4] = 4 * k + 2;
                            MD.indices[ib + 5] = 4 * k + 3;

                            vb += 4 * VD.size;
                            ib += 6;
                            tpx += d.xadvance;
                            k++;
                        }
                    }
                    tpy += Fonts[FontId].lineHeight;
                    tpx = 0;
                }
                tpx = 0;
                tpy = 0;
                Txt.len = ib - Txt.start;
            }
            std::cout << "Text: " << MD.vertices.size()
                      << ", I: " << MD.indices.size() << "\n";
        }

        void createVertexBuffer(ModelData &Md) {
            VkDeviceSize bufferSize = sizeof(Md.vertices[0]) * Md.vertices.size();

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         stagingBuffer, stagingBufferMemory);

            void *data;
            vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, Md.vertices.data(), (size_t) bufferSize);
            vkUnmapMemory(device, stagingBufferMemory);

            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                     VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                         Md.vertexBuffer, Md.vertexBufferMemory);

            copyBuffer(stagingBuffer, Md.vertexBuffer, bufferSize);

            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);
        }

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                          VkMemoryPropertyFlags properties,
                          VkBuffer &buffer, VkDeviceMemory &bufferMemory) {
            VkBufferCreateInfo bufferInfo{};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = size;
            bufferInfo.usage = usage;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            VkResult result =
                    vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);
            if (result != VK_SUCCESS) {
                PrintVkError(result);
                throw std::runtime_error("failed to create vertex buffer!");
            }

            VkMemoryRequirements memRequirements;
            vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex =
                    findMemoryType(memRequirements.memoryTypeBits, properties);

            result = vkAllocateMemory(device, &allocInfo, nullptr,
                                      &bufferMemory);
            if (result != VK_SUCCESS) {
                PrintVkError(result);
                throw std::runtime_error("failed to allocate vertex buffer memory!");
            }

            vkBindBufferMemory(device, buffer, bufferMemory, 0);
        }

        uint32_t findMemoryType(uint32_t typeFilter,
                                VkMemoryPropertyFlags properties) {
            VkPhysicalDeviceMemoryProperties memProperties;
            vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

            for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
                if ((typeFilter & (1 << i)) &&
                    (memProperties.memoryTypes[i].propertyFlags & properties) ==
                    properties) {
                    return i;
                }
            }

            throw std::runtime_error("failed to find suitable memory type!");
        }

        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
            VkCommandBuffer commandBuffer = beginSingleTimeCommands();

            VkBufferCopy copyRegion{};
            copyRegion.srcOffset = 0; // Optional copyRegion.dstOffset = 0; // Optional
            copyRegion.size = size;
            vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

            endSingleTimeCommands(commandBuffer);
        }

        VkCommandBuffer beginSingleTimeCommands() {
            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandPool = commandPool;
            allocInfo.commandBufferCount = 1;

            VkCommandBuffer commandBuffer;
            vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            vkBeginCommandBuffer(commandBuffer, &beginInfo);

            return commandBuffer;
        }

        void endSingleTimeCommands(VkCommandBuffer commandBuffer) {
            vkEndCommandBuffer(commandBuffer);

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;
            vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(graphicsQueue);

            vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
        }

        void createIndexBuffer(ModelData &Md) {
            VkDeviceSize bufferSize = sizeof(Md.indices[0]) * Md.indices.size();

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         stagingBuffer, stagingBufferMemory);

            void *data;
            vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, Md.indices.data(), (size_t) bufferSize);
            vkUnmapMemory(device, stagingBufferMemory);

            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                     VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                         Md.indexBuffer, Md.indexBufferMemory);

            copyBuffer(stagingBuffer, Md.indexBuffer, bufferSize);
            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);
        }

        void createUniformBuffers() {
            //Phong uniform buffer
            VkDeviceSize bufferSize = sizeof(UniformBufferObject);

            uniformBuffers.resize(swapChainImages.size() * scene.size());
            uniformBuffersMemory.resize(swapChainImages.size() * scene.size());

            for (size_t i = 0; i < swapChainImages.size() * scene.size(); i++) {
                createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             uniformBuffers[i], uniformBuffersMemory[i]);
            }

            //Terrain uniform buffer
            bufferSize = sizeof(TerrainUniformBufferObject);

            terrainUniformBuffers.resize(swapChainImages.size() * scene.size());
            terrainUniformBuffersMemory.resize(swapChainImages.size() * scene.size());

            for (size_t i = 0; i < swapChainImages.size() * scene.size(); i++) {
                createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             terrainUniformBuffers[i], terrainUniformBuffersMemory[i]);
            }

            //Global uniform buffer
            bufferSize = sizeof(GlobalUniformBufferObject);

            globalUniformBuffers.resize(swapChainImages.size());
            globalUniformBuffersMemory.resize(swapChainImages.size());

            for (size_t i = 0; i < swapChainImages.size(); i++) {
                createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             globalUniformBuffers[i], globalUniformBuffersMemory[i]);
            }


            //Skybox uniform buffer
            bufferSize = sizeof(UniformBufferObject);

            SkyBoxUniformBuffers.resize(swapChainImages.size());
            SkyBoxUniformBuffersMemory.resize(swapChainImages.size());

            for (size_t i = 0; i < swapChainImages.size(); i++) {
                createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             SkyBoxUniformBuffers[i], SkyBoxUniformBuffersMemory[i]);
            }

            //Text uniform buffer
            bufferSize = sizeof(UniformBufferObject);

            TextUniformBuffers.resize(swapChainImages.size());
            TextUniformBuffersMemory.resize(swapChainImages.size());

            for (size_t i = 0; i < swapChainImages.size(); i++) {
                createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             TextUniformBuffers[i], TextUniformBuffersMemory[i]);
            }
        }

        void createDescriptorPool() {
            std::array<VkDescriptorPoolSize, 10> poolSizes{};
            poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImages.size() * scene.size());
            poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChainImages.size() * scene.size());
            poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[2].descriptorCount = static_cast<uint32_t>(swapChainImages.size() * scene.size());
            poolSizes[3].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[3].descriptorCount = static_cast<uint32_t>(swapChainImages.size());
            poolSizes[4].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[4].descriptorCount = static_cast<uint32_t>(swapChainImages.size());
            poolSizes[5].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[5].descriptorCount = static_cast<uint32_t>(swapChainImages.size());
            poolSizes[6].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[6].descriptorCount = static_cast<uint32_t>(swapChainImages.size());
            poolSizes[7].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[7].descriptorCount = static_cast<uint32_t>(swapChainImages.size());
            poolSizes[8].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[8].descriptorCount = static_cast<uint32_t>(swapChainImages.size());
            poolSizes[9].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[9].descriptorCount = static_cast<uint32_t>(swapChainImages.size() * scene.size());

            VkDescriptorPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
            poolInfo.pPoolSizes = poolSizes.data();
            poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size() * (scene.size() + 2));

            VkResult result = vkCreateDescriptorPool(device, &poolInfo, nullptr,
                                                     &descriptorPool);
            if (result != VK_SUCCESS) {
                PrintVkError(result);
                throw std::runtime_error("failed to create descriptor pool!");
            }
        }

	void createDescriptorSets() {
		createPhongDescriptorSets();
		createSkyBoxDescriptorSets();
		createTextDescriptorSets();
	}
	
	void createPhongDescriptorSets() {
		std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size() * scene.size(), PhongDescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size() * scene.size());
		allocInfo.pSetLayouts = layouts.data();

		PhongDescriptorSets.resize(swapChainImages.size() * scene.size());
		
		VkResult result = vkAllocateDescriptorSets(device, &allocInfo,
											PhongDescriptorSets.data());
		if (result != VK_SUCCESS) {
			PrintVkError(result);
			throw std::runtime_error("failed to allocate descriptor sets!");
		}
		
		for (size_t k = 0; k < swapChainImages.size(); k++) {
			for (size_t j = 0; j < scene.size(); j++) {
				size_t i = j * swapChainImages.size() + k;
				
				VkDescriptorBufferInfo bufferInfo{};
				bufferInfo.buffer = uniformBuffers[i];
				bufferInfo.offset = 0;
				bufferInfo.range = sizeof(UniformBufferObject);
				
				VkDescriptorImageInfo imageInfo{};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = scene[j].TD.textureImageView;
				imageInfo.sampler = scene[j].TD.textureSampler;
				
				VkDescriptorBufferInfo globalBufferInfo{};
				globalBufferInfo.buffer = globalUniformBuffers[k];
				globalBufferInfo.offset = 0;
				globalBufferInfo.range = sizeof(GlobalUniformBufferObject);

                VkDescriptorBufferInfo terrainBufferInfo{};
                terrainBufferInfo.buffer = terrainUniformBuffers[i];
                terrainBufferInfo.offset = 0;
                terrainBufferInfo.range = sizeof(TerrainUniformBufferObject);
				
				std::array<VkWriteDescriptorSet, 4> descriptorWrites{};
				descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[0].dstSet = PhongDescriptorSets[i];
				descriptorWrites[0].dstBinding = 0;
				descriptorWrites[0].dstArrayElement = 0;
				descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrites[0].descriptorCount =  1;
				descriptorWrites[0].pBufferInfo = &bufferInfo;
	
				descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[1].dstSet = PhongDescriptorSets[i];
				descriptorWrites[1].dstBinding = 1;
				descriptorWrites[1].dstArrayElement = 0;
				descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrites[1].descriptorCount = 1;
				descriptorWrites[1].pImageInfo = &imageInfo;
				
				descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[2].dstSet = PhongDescriptorSets[i];
				descriptorWrites[2].dstBinding = 2;
				descriptorWrites[2].dstArrayElement = 0;
				descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrites[2].descriptorCount =  1;
				descriptorWrites[2].pBufferInfo = &globalBufferInfo;

                descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[3].dstSet = PhongDescriptorSets[i];
                descriptorWrites[3].dstBinding = 3;
                descriptorWrites[3].dstArrayElement = 0;
                descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[3].descriptorCount =  1;
                descriptorWrites[3].pBufferInfo = &terrainBufferInfo;

				vkUpdateDescriptorSets(device,
							static_cast<uint32_t>(descriptorWrites.size()),
							descriptorWrites.data(), 0, nullptr);
			}
		}
	}

	void createSkyBoxDescriptorSets() {
		std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(),
												   SkyBoxDescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
		allocInfo.pSetLayouts = layouts.data();

		SkyBoxDescriptorSets.resize(swapChainImages.size());
		
		VkResult result = vkAllocateDescriptorSets(device, &allocInfo,
											SkyBoxDescriptorSets.data());
		if (result != VK_SUCCESS) {
			PrintVkError(result);
			throw std::runtime_error("failed to allocate Skybox descriptor sets!");
		}
		
		for (size_t k = 0; k < swapChainImages.size(); k++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = SkyBoxUniformBuffers[k];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);
				
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = SkyBox.TD.textureImageView;
			imageInfo.sampler = SkyBox.TD.textureSampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = SkyBoxDescriptorSets[k];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount =  1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = SkyBoxDescriptorSets[k];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType =
										VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(device,
						static_cast<uint32_t>(descriptorWrites.size()),
						descriptorWrites.data(), 0, nullptr);
		}
	}

	void createTextDescriptorSets() {
		std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(),
												   TextDescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
		allocInfo.pSetLayouts = layouts.data();

		TextDescriptorSets.resize(swapChainImages.size());
		
		VkResult result = vkAllocateDescriptorSets(device, &allocInfo,
											TextDescriptorSets.data());
		if (result != VK_SUCCESS) {
			PrintVkError(result);
			throw std::runtime_error("failed to allocate Text descriptor sets!");
		}
		
		for (size_t k = 0; k < swapChainImages.size(); k++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = TextUniformBuffers[k];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);
				
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = SText.TD.textureImageView;
			imageInfo.sampler = SText.TD.textureSampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = TextDescriptorSets[k];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount =  1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = TextDescriptorSets[k];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType =
										VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(device,
						static_cast<uint32_t>(descriptorWrites.size()),
						descriptorWrites.data(), 0, nullptr);
		}
	}

    void createCommandBuffers() {
    	commandBuffers.resize(swapChainFramebuffers.size());
    	
    	VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();
		
		VkResult result = vkAllocateCommandBuffers(device, &allocInfo,
				commandBuffers.data());
		if (result != VK_SUCCESS) {
		 	PrintVkError(result);
			throw std::runtime_error("failed to allocate command buffers!");
		}
		
		for (size_t i = 0; i < commandBuffers.size(); i++) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0; // Optional
			beginInfo.pInheritanceInfo = nullptr; // Optional

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) !=
						VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer!");
			}
			
			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = renderPass; 
			renderPassInfo.framebuffer = swapChainFramebuffers[i];
			renderPassInfo.renderArea.offset = {0, 0};
			renderPassInfo.renderArea.extent = swapChainExtent;

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
			clearValues[1].depthStencil = {1.0f, 0};

			renderPassInfo.clearValueCount =
							static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();
			
			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo,
					VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
					PhongPipeline);

            std::vector<Object*> sct = Object::objs;
            for (int j = 0;j<sct.size();j++ ) {
				if(sct[j]->model.pt == Flat) {
					VkBuffer vertexBuffers[] = {scene[j].MD.vertexBuffer};
					VkDeviceSize offsets[] = {0};
					vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
					vkCmdBindIndexBuffer(commandBuffers[i], scene[j].MD.indexBuffer, 0,
                                         VK_INDEX_TYPE_UINT32);
					vkCmdBindDescriptorSets(commandBuffers[i],
									VK_PIPELINE_BIND_POINT_GRAPHICS,
									PhongPipelineLayout, 0, 1,
									&PhongDescriptorSets[i + j * swapChainImages.size()],
									0, nullptr);
									
					vkCmdDrawIndexed(commandBuffers[i],
                                     static_cast<uint32_t>(scene[j].MD.indices.size()), 1, 0, 0, 0);
				}
			}

			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                              TerrainPipeline);
            for (int j = 0;j<sct.size();j++ ) {
                if(sct[j]->model.pt == Terrain) {
					VkBuffer vertexBuffers[] = {scene[j].MD.vertexBuffer};
					VkDeviceSize offsets[] = {0};
					vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
					vkCmdBindIndexBuffer(commandBuffers[i], scene[j].MD.indexBuffer, 0,
                                         VK_INDEX_TYPE_UINT32);
					vkCmdBindDescriptorSets(commandBuffers[i],
									VK_PIPELINE_BIND_POINT_GRAPHICS,
									PhongPipelineLayout, 0, 1,
									&PhongDescriptorSets[i + j * swapChainImages.size()],
									0, nullptr);
									
					vkCmdDrawIndexed(commandBuffers[i],
                                     static_cast<uint32_t>(scene[j].MD.indices.size()), 1, 0, 0, 0);
				}
			}

			// Draws the Skybox
			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
					SkyBoxPipeline);
			VkBuffer vertexBuffers[] = {SkyBox.MD.vertexBuffer};
			VkDeviceSize offsets[] = {0};
			vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commandBuffers[i], SkyBox.MD.indexBuffer, 0,
									VK_INDEX_TYPE_UINT32);
			vkCmdBindDescriptorSets(commandBuffers[i],
							VK_PIPELINE_BIND_POINT_GRAPHICS,
							SkyBoxPipelineLayout, 0, 1,
							&SkyBoxDescriptorSets[i],
							0, nullptr);							
			vkCmdDrawIndexed(commandBuffers[i],
						static_cast<uint32_t>(SkyBox.MD.indices.size()), 1, 0, 0, 0);
			
			
			// Draws the text
			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
					TextPipeline);
			VkBuffer TvertexBuffers[] = {SText.MD.vertexBuffer};
			VkDeviceSize Toffsets[] = {0};
			vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, TvertexBuffers, Toffsets);
			vkCmdBindIndexBuffer(commandBuffers[i], SText.MD.indexBuffer, 0,
									VK_INDEX_TYPE_UINT32);
			vkCmdBindDescriptorSets(commandBuffers[i],
							VK_PIPELINE_BIND_POINT_GRAPHICS,
							TextPipelineLayout, 0, 1,
							&TextDescriptorSets[i],
							0, nullptr);							
			vkCmdDrawIndexed(commandBuffers[i],
                             static_cast<uint32_t>(SceneText[currentView].len), 1, static_cast<uint32_t>(SceneText[currentView].start), 0, 0);
			
			vkCmdEndRenderPass(commandBuffers[i]);
			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}
		}
	}
    
    void createSyncObjects() {
    	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    	imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);
    	    	
    	VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			VkResult result1 = vkCreateSemaphore(device, &semaphoreInfo, nullptr,
								&imageAvailableSemaphores[i]);
			VkResult result2 = vkCreateSemaphore(device, &semaphoreInfo, nullptr,
								&renderFinishedSemaphores[i]);
			VkResult result3 = vkCreateFence(device, &fenceInfo, nullptr,
								&inFlightFences[i]);
			if (result1 != VK_SUCCESS ||
				result2 != VK_SUCCESS ||
				result3 != VK_SUCCESS) {
			 	PrintVkError(result1);
			 	PrintVkError(result2);
			 	PrintVkError(result3);
				throw std::runtime_error("failed to create synchronization objects for a frame!!");
			}
		}
	}

    TerrainCollider planeCollider = {};

	void initClasses() {
		RigidBody* rb = &truck.rb;
		physicsEngine.AddRigidBody(rb);

        //float planeY = 0.0f;
        physicsEngine.AddCollider(&planeCollider);
	}

    void mainLoop() {
        //std::thread t(&PhysicsEngine::StepC, PhysicsEngine(),&physicsEngine);
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            drawFrame();
            
            if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            	break;
            }
        }
        
        vkDeviceWaitIdle(device);
    }
    
    void drawFrame() {
		vkWaitForFences(device, 1, &inFlightFences[currentFrame],
						VK_TRUE, UINT64_MAX);
		
		uint32_t imageIndex;
		
		VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX,
				imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
			vkWaitForFences(device, 1, &imagesInFlight[imageIndex],
							VK_TRUE, UINT64_MAX);
		}
		imagesInFlight[imageIndex] = inFlightFences[currentFrame];
		
		updateUniformBuffer(imageIndex);
		
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
		VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;
		
		vkResetFences(device, 1, &inFlightFences[currentFrame]);

		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo,
				inFlightFences[currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}
		
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		
		VkSwapchainKHR swapChains[] = {swapChain};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional
		
		result = vkQueuePresentKHR(presentQueue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
			framebufferResized) {
            framebufferResized = false;
            recreateSwapChain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }
		
		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
	
	void updateUniformBuffer(uint32_t currentImage) {
		// Inputs
		static auto startTime = std::chrono::high_resolution_clock::now();
		static float lastTime = 0.0f;

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>
			(currentTime - startTime).count();
		float deltaT = time - lastTime;
		lastTime = time;

		static float debounce = time;

		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			if (time - debounce > 0.33) {
                currentView = (currentView + 1) % SceneText.size();
				debounce = time;
				framebufferResized = true;
				//std::cout << currentView << "\n";
			}
		}

		//mat4 oldTransform = truck.getTransform();

        truck.UpdatePos(window, deltaT);
		const float tilesize = 1.0f;
		int truckPosX = floor(truck.getTransform()[3].x / tilesize);//Should be divided TileSize
		int truckPosZ = floor(truck.getTransform()[3].z / tilesize);//Should be divided TileSize

		movetree(truckPosX, truckPosZ);
		//Physics
        physicsEngine.Step(deltaT, window);

		//std::cout << round(lookYaw * 180.f / 3.1416f) << "\t" << round(lookPitch * 180.f / 3.1416f) << "\t" <<  round(lookRoll * 180.f / 3.1416f) << "\n";

		glm::mat4 CamMat = glm::mat4(1.0);

		static int prevCt = -1;

		glm::mat4 Prj = glm::perspective(glm::radians(90.0f),
			swapChainExtent.width / (float)swapChainExtent.height,
			0.1f, 150.0f);
		Prj[1][1] *= -1;

		glm::vec3 EyePos;
		static glm::vec3 FollowerPos = truck.getTransform()[3];

		switch (currentView) {
            case 0:
                if (currentView != prevCt) {
                    prevCt = currentView;
                }
                {
                    vec3 truckZ = truck.getTransform()[2];

                    CamMat = MatrixUtils::LookAtMat(
                            vec3(truck.getTransform()[3])+truck.thirdPersonCamDelta.z*truckZ+vec3(0,truck.thirdPersonCamDelta.y,0),
                            vec3(truck.getTransform()[3])-truckZ+vec3(0,1,0),
                            0);
                }
                break;
		case 1:
			if (currentView != prevCt) {
				prevCt = currentView;
			}
			{
				//glm::vec3 RRCDP = truck.getTransform() * vec4(truck.RobotCamDeltaPos, 1.0f);
				//std::cout << RRCDP.x << " " << RRCDP.z << "\n";
				//CamMat = MatrixUtils::LookInDirMat(truck.getTransform() * vec4(truck.RobotCamDeltaPos, 1.0f),
                //                                   glm::vec3(0,0,0));
                vec3 xAxis = vec3(1,0,0);
                vec3 truckX = truck.getTransform()[1];
                float angle = acos( dot(normalize(xAxis),normalize(truckX)));
                //MatrixUtils::printVector(truckX);
                //std::cout<<angle<<std::endl;
                /*
                CamMat = inverse(
                        translate(mat4(1),vec3(truck.getTransform()[3]))*
                        truck.firstPersonCamDelta*
                        rotate(mat4(1),angle,vec3(0,1,0))
                        );*/
                //CamMat = inverse(truck.firstPersonCamDelta);
                CamMat = inverse(truck.getTransform()*
                        truck.firstPersonCamDelta
                );
			}
			break;
		}
		EyePos = -glm::vec3(CamMat * glm::vec4(0, 0, 0, 1));
		// Updates unifoms for the objects
        std::vector<Object*> sct = Object::objs;
        for (int j = 0;j<sct.size();j++) {
            Object* o=sct[j];
            UniformBufferObject ubo{};
            static TerrainUniformBufferObject tubo{};

			glm::vec3 delta;

			ubo.mMat = o->getTransform();

            if (o->id=="terrain"){
                const float tilesize = 1.0f;
                int truckPosX = floor(truck.getTransform()[3].x/ tilesize);//Should be divided TileSize
                int truckPosZ = floor(truck.getTransform()[3].z / tilesize);//Should be divided TileSize

                static float terrainUpdateTime;
                static bool firstFrame = true;
                const float updateRate = 0.5f;
                //Updates the terrain each updateRate;
                if (time - terrainUpdateTime > updateRate || firstFrame) {
                    tubo.translation = vec2(truckPosX,truckPosZ)-vec2(TILE_NUMBER/2, TILE_NUMBER/2);
                    terrainUpdateTime = time;

                    firstFrame=false;
                }
            }
	

			/*if (o->id == "tre")
			{
				
				const float tilesize = 1.0f;
				int truckPosX = floor(truck.getTransform()[3].x / tilesize);//Should be divided TileSize
				int truckPosZ = floor(truck.getTransform()[3].z / tilesize);//Should be divided TileSize
				
				mat4 tmp = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{truckPosX,0,0,1} };
			//	treeContainer->setTransform(tmp);


				//movetree(truckPosX,truckPosZ,o);
			
			}*/
			
			

			if (o->id=="truck") {
				ubo.mMat = ubo.mMat * glm::rotate(mat4(1), 1.5708f, glm::vec3(0, 1, 0)) ;
				/*FollowerTargetPos = TruckWM * glm::translate(glm::mat4(1), truck.thirdPersonCamDelta) *
                                    glm::rotate(glm::mat4(1), truck.lookPitch, glm::vec3(1, 0, 0)) *
                                    glm::vec4(0.0f, 0.0f, truck.followerDist, 1.0f);*/

				

			}

			ubo.mvpMat = Prj * CamMat * ubo.mMat;
			ubo.nMat = glm::inverse(glm::transpose(ubo.mMat));

			int i = j * swapChainImages.size() + currentImage;

			void* data;
			vkMapMemory(device, uniformBuffersMemory[i], 0, sizeof(ubo), 0, &data);
			memcpy(data, &ubo, sizeof(ubo));
			vkUnmapMemory(device, uniformBuffersMemory[i]);

            void* dataa;
            vkMapMemory(device, terrainUniformBuffersMemory[i], 0, sizeof(tubo), 0, &dataa);
            memcpy(dataa, &tubo, sizeof(tubo));
            vkUnmapMemory(device, terrainUniformBuffersMemory[i]);
		}

		GlobalUniformBufferObject gubo{};

		static auto startT = std::chrono::high_resolution_clock::now();
		static float lastT = 0.0f;

		auto currentT = std::chrono::high_resolution_clock::now();
		float t = std::chrono::duration<float, std::chrono::seconds::period>
			(currentT - startT).count();

		if (glfwGetKey(window, GLFW_KEY_P)) {
			change_time += 0.15;
			std::cout << "  chg time " << change_time << " rot time " << rot << std::endl;
		}
		rot = ( time+change_time) * 3.5;
		rot =  remainder(rot, 360);
		gubo.lightDir = rotate(glm::mat4(1), glm::radians(-rot), vec3(1, 0, 0))[2];

        float sinHeight = sin(radians(rot));
        gubo.lightColor = vec4(sinHeight, sinHeight, sinHeight, 1.0f);
		if(rot<0 || rot>180)
			gubo.lightColor = vec4(0.0f, 0.0f, 0.0f,1.0f);



		// updates global uniforms
		//gubo.lightDir = glm::vec3(cos(glm::radians(-135.0f)), sin(glm::radians(-135.0f)), 0.0f);
        gubo.leftSpotlightDir = truck.children[0]->getTransform()[2];//Gets the z axis of the light reference system
        gubo.leftSpotlightPos = truck.children[0]->getTransform()[3];
        gubo.leftSpotlightColor = glm::vec3(0.9f, 0.9f, 0.9f);
        gubo.leftSpotlightParams = glm::vec4(cos(glm::radians(22.5f)), cos(glm::radians(30.0f)), 1.0, 1.8f);

        gubo.rightSpotlightDir = truck.children[1]->getTransform()[2];//Gets the z axis of the light reference system
        gubo.rightSpotlightPos = truck.children[1]->getTransform()[3];
        gubo.rightSpotlightColor = glm::vec3(0.9f, 0.9f, 0.9f);
        gubo.rightSpotlightParams = glm::vec4(cos(glm::radians(22.5f)), cos(glm::radians(30.0f)), 1.0, 1.8f);
		gubo.eyePos = EyePos;

		

		// updates SkyBox uniforms
		UniformBufferObject ubo{};
		ubo.mMat = glm::mat4(1.0f);
		ubo.nMat = glm::mat4(1.0f);

		
		ubo.ti.x = time;
		ubo.mvpMat = Prj * glm::mat4(glm::mat3(CamMat));
		
		

		ubo.ti.y = rot;
		ubo.mvpMat = ubo.mvpMat * glm::rotate(glm::mat4(1), glm::radians(rot), glm::vec3(-1, 0, 0));


		void* data;
		vkMapMemory(device, globalUniformBuffersMemory[currentImage], 0, sizeof(gubo), 0, &data);
		memcpy(data, &gubo, sizeof(gubo));
		vkUnmapMemory(device, globalUniformBuffersMemory[currentImage]);



		vkMapMemory(device, SkyBoxUniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, SkyBoxUniformBuffersMemory[currentImage]);
	}


    void recreateSwapChain() {
    	int width = 0, height = 0;
		glfwGetFramebufferSize(window, &width, &height);
		
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(window, &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(device);
    	
    	cleanupSwapChain();

		createSwapChain();
		createImageViews();
		createRenderPass();
		createPipelines();
		createColorResources();
		createDepthResources();
		createFramebuffers();
		createUniformBuffers();
		createDescriptorPool();
		createDescriptorSets();
		createCommandBuffers();
	}

	void cleanupSwapChain() {
    	vkDestroyImageView(device, colorImageView, nullptr);
    	vkDestroyImage(device, colorImage, nullptr);
    	vkFreeMemory(device, colorImageMemory, nullptr);
    	
		vkDestroyImageView(device, depthImageView, nullptr);
		vkDestroyImage(device, depthImage, nullptr);
		vkFreeMemory(device, depthImageMemory, nullptr);

		for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
			vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
		}
		
		vkFreeCommandBuffers(device, commandPool,
				static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

		vkDestroyPipeline(device, PhongPipeline, nullptr);
		vkDestroyPipeline(device, TerrainPipeline, nullptr);
		vkDestroyPipelineLayout(device, PhongPipelineLayout, nullptr);
		vkDestroyPipelineLayout(device, TerrainPipelineLayout, nullptr);

		vkDestroyPipeline(device, SkyBoxPipeline, nullptr);
		vkDestroyPipelineLayout(device, SkyBoxPipelineLayout, nullptr);

		vkDestroyPipeline(device, TextPipeline, nullptr);
		vkDestroyPipelineLayout(device, TextPipelineLayout, nullptr);

		vkDestroyRenderPass(device, renderPass, nullptr);

		for (size_t i = 0; i < swapChainImageViews.size(); i++){
			vkDestroyImageView(device, swapChainImageViews[i], nullptr);
		}

		vkDestroySwapchainKHR(device, swapChain, nullptr);
		
		for (size_t i = 0; i < swapChainImages.size() * scene.size(); i++) {
			vkDestroyBuffer(device, uniformBuffers[i], nullptr);
			vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
		}

        for (size_t i = 0; i < swapChainImages.size() * scene.size(); i++) {
            vkDestroyBuffer(device, terrainUniformBuffers[i], nullptr);
            vkFreeMemory(device, terrainUniformBuffersMemory[i], nullptr);
        }

		for (size_t i = 0; i < swapChainImages.size(); i++) {
			vkDestroyBuffer(device, globalUniformBuffers[i], nullptr);
			vkFreeMemory(device, globalUniformBuffersMemory[i], nullptr);
		}		
		for (size_t i = 0; i < swapChainImages.size(); i++) {
			vkDestroyBuffer(device, SkyBoxUniformBuffers[i], nullptr);
			vkFreeMemory(device, SkyBoxUniformBuffersMemory[i], nullptr);
		}
		for (size_t i = 0; i < swapChainImages.size(); i++) {
			vkDestroyBuffer(device, TextUniformBuffers[i], nullptr);
			vkFreeMemory(device, TextUniformBuffersMemory[i], nullptr);
		}
		
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	}


    void cleanup() {
    	cleanupSwapChain();
    	
    	for (size_t i = 0; i < scene.size(); i++) {
	    	vkDestroySampler(device, scene[i].TD.textureSampler, nullptr);
	    	vkDestroyImageView(device, scene[i].TD.textureImageView, nullptr);
			vkDestroyImage(device, scene[i].TD.textureImage, nullptr);
			vkFreeMemory(device, scene[i].TD.textureImageMemory, nullptr);
	    	
	    	vkDestroyBuffer(device, scene[i].MD.indexBuffer, nullptr);
	    	vkFreeMemory(device, scene[i].MD.indexBufferMemory, nullptr);
	
			vkDestroyBuffer(device, scene[i].MD.vertexBuffer, nullptr);
	    	vkFreeMemory(device, scene[i].MD.vertexBufferMemory, nullptr);
    	}
    	vkDestroySampler(device, SkyBox.TD.textureSampler, nullptr);
    	vkDestroyImageView(device, SkyBox.TD.textureImageView, nullptr);
		vkDestroyImage(device, SkyBox.TD.textureImage, nullptr);
		vkFreeMemory(device, SkyBox.TD.textureImageMemory, nullptr);

    	vkDestroyBuffer(device, SkyBox.MD.indexBuffer, nullptr);
    	vkFreeMemory(device, SkyBox.MD.indexBufferMemory, nullptr);

		vkDestroyBuffer(device, SkyBox.MD.vertexBuffer, nullptr);
    	vkFreeMemory(device, SkyBox.MD.vertexBufferMemory, nullptr);
    	    	
    	vkDestroySampler(device, SText.TD.textureSampler, nullptr);
    	vkDestroyImageView(device, SText.TD.textureImageView, nullptr);
		vkDestroyImage(device, SText.TD.textureImage, nullptr);
		vkFreeMemory(device, SText.TD.textureImageMemory, nullptr);

    	vkDestroyBuffer(device, SText.MD.indexBuffer, nullptr);
    	vkFreeMemory(device, SText.MD.indexBufferMemory, nullptr);

		vkDestroyBuffer(device, SText.MD.vertexBuffer, nullptr);
    	vkFreeMemory(device, SText.MD.vertexBufferMemory, nullptr);
    	    	
    	vkDestroyDescriptorSetLayout(device, PhongDescriptorSetLayout, nullptr);
    	vkDestroyDescriptorSetLayout(device, SkyBoxDescriptorSetLayout, nullptr);
    	vkDestroyDescriptorSetLayout(device, TextDescriptorSetLayout, nullptr);
    	
    	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(device, inFlightFences[i], nullptr);
    	}
    	
    	vkDestroyCommandPool(device, commandPool, nullptr);
    	
 		vkDestroyDevice(device, nullptr);
		
	   	if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		
		vkDestroySurfaceKHR(instance, surface, nullptr);
    	vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }

	Object* p=NULL;
	void movetree(int truckPosX, int truckPosZ){
	/*
		int t = treeGridLength * 10;
		int a = (truckPosX % t);
		int b = (truckPosZ % t);
		int fxmin = truckPosX - a;//grid start x
		int fzmin = truckPosZ - b;//grid start z
		int fxmax = fxmin + t;
		int fzmax = fzmin + t;
		int closex, closez;
		if (a < (fxmax / 2) && b < (fzmax / 2)) {
			closex = fxmin; closez = fzmin;
		}
		else 		if (a > (fxmax / 2) && b < (fzmax / 2)) { closex = fxmax; closez = fzmin; }
		else 		if (a < (fxmax / 2) && b > (fzmax / 2)) { closex = fxmin; closez = fzmax; }
		else 		{ closex = fxmax; closez = fzmax; }

		

		int treePosX, treePosZ;
		treePosX = o->getTransform()[3].x;
		treePosZ= o->getTransform()[3].z;
		
		if (treePosX == -closex && treePosZ == -closez) {
			//print "closerrrrrrrr"
		//	std::cout << " x pos far" << treePosX << " y pos far" << treePosZ;
			//o->setTransform(mat4({ 1,0,0,0 }, { 0,1,0,0 }, { 0,0,1,0 }, { 0,0,0,1 }));
			 p = o;
		}

		
		if (closex==treePosX  && closez == treePosZ) {
			//print "closerrrrrrrr"
			//std::cout << " x pos" << treePosX << " y pos " << treePosZ;
			//o->setTransform(mat4({ 1,0,0,0 }, { 0,1,0,0 }, { 0,0,1,0 }, {0,0,0,1}));
			return;
		}
		else
			if(a>t||b>t)
		 {
			p->setTransform(mat4({ 1,0,0, 0 }, { 0,1,0, 0 }, { 0,0,1, 0 }, { closex,getHeight(closex,closez),closez, 1 }));
			return;
		}
		*/
		
		//make the current treeContiner as treeContainer5 if truck is present inside it
		int shrt=1;
		int t = treeGridLength * 2;
		float small = 10000;
		vec2 pos;
		for (int i = 0; i < 9;i++) {
			//calculate distance between truck and treeContainer
			float distance = sqrt(pow(truckPosX - treeContainer[i]->children[5]->getTransform()[3].x, 2) + pow(truckPosZ - treeContainer[i]->children[5]->getTransform()[3].z, 2));
			if (distance < small) {
				small = distance; shrt = i;
			}
		}
		
		std::cout << "grid no " << shrt << std::endl;

		if (shrt == 0) {
			treeContainer5 = treeContainer1;
			for (int i = 0; i < 9; i++) {
			
				for (Object* obs:treeContainer[i]->children)
				{
					pos.x = obs->getTransform()[3].x-t;
					pos.y = obs->getTransform()[3].z-t;

					obs->setTransform(mat4({ 1,0,0,0 }, { 0,1,0,0 }, { 0,0,1,0 }, { pos.x,getHeight(pos.x,pos.y),pos.y ,1}));
				}
			}
		}
		else
			if (shrt == 1) {
				treeContainer5 = treeContainer2;
				for (int i = 0; i < 9; i++) {
						for (Object* obs : treeContainer[i]->children)
						{
							pos.x = obs->getTransform()[3].x ;
							pos.y = obs->getTransform()[3].z - t;

							obs->setTransform(mat4({ 1,0,0,0 }, { 0,1,0,0 }, { 0,0,1,0 }, { pos.x,getHeight(pos.x,pos.y),pos.y ,1 }));
						}
				}
			}
    
			else
				if (shrt == 2) {
					treeContainer5 = treeContainer3;
					for (int i = 0; i < 9; i++) {
							for (Object* obs : treeContainer[i]->children)
							{
								pos.x = obs->getTransform()[3].x + t;
								pos.y = obs->getTransform()[3].z - t;

								obs->setTransform(mat4({ 1,0,0,0 }, { 0,1,0,0 }, { 0,0,1,0 }, { pos.x,getHeight(pos.x,pos.y),pos.y ,1 }));
							}
					}
				}
	
			else
					if (shrt == 3) {
						treeContainer5 = treeContainer4;
						for (int i = 0; i < 9; i++) {
								for (Object* obs : treeContainer[i]->children)
								{
									pos.x = obs->getTransform()[3].x - t;
									pos.y = obs->getTransform()[3].z ;

									obs->setTransform(mat4({ 1,0,0,0 }, { 0,1,0,0 }, { 0,0,1,0 }, { pos.x,getHeight(pos.x,pos.y),pos.y ,1 }));
								}
						}
					}

			else
						if (shrt == 5) {
							treeContainer5 = treeContainer5;
							for (int i = 0; i < 9; i++) {
									for (Object* obs : treeContainer[i]->children)
									{
										pos.x = obs->getTransform()[3].x + t;
										pos.y = obs->getTransform()[3].z ;

										obs->setTransform(mat4({ 1,0,0,0 }, { 0,1,0,0 }, { 0,0,1,0 }, { pos.x,getHeight(pos.x,pos.y),pos.y ,1 }));
									}
							}
						}
		else
			if (shrt == 6) {
				treeContainer5 = treeContainer6;
				for (int i = 0; i < 9; i++) {
					for (Object* obs : treeContainer[i]->children)
					{
						pos.x = obs->getTransform()[3].x - t;
						pos.y = obs->getTransform()[3].z + t;

						obs->setTransform(mat4({ 1,0,0,0 }, { 0,1,0,0 }, { 0,0,1,0 }, { pos.x,getHeight(pos.x,pos.y),pos.y ,1 }));
					}
			}
		}

			else
				if (shrt == 7) {
					treeContainer5 = treeContainer7;
					for (int i = 0; i < 9; i++) {
							for (Object* obs : treeContainer[i]->children)
							{
								pos.x = obs->getTransform()[3].x ;
								pos.y = obs->getTransform()[3].z + t;

								obs->setTransform(mat4({ 1,0,0,0 }, { 0,1,0,0 }, { 0,0,1,0 }, { pos.x,getHeight(pos.x,pos.y),pos.y ,1 }));
							}
					}
				}
		
				else
					if (shrt == 8) {
						treeContainer5 = treeContainer8;
						for (int i = 0; i < 9; i++) {
								for (Object* obs : treeContainer[i]->children)
								{
									pos.x = obs->getTransform()[3].x + t;
									pos.y = obs->getTransform()[3].z + t;

									obs->setTransform(mat4({ 1,0,0,0 }, { 0,1,0,0 }, { 0,0,1,0 }, { pos.x,getHeight(pos.x,pos.y),pos.y ,1 }));
								}
						}
					}

}

};
	

	//// To put in the right place
	


int main() {
    CGProject app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}