#include "vulkan_app.hpp"

#include <iostream>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// Add functions from extensions
#include "vk_ext/all.hpp"

vulkan_app::vulkan_app()
{
	// When the app starts, we initialize the window
	init_window();
	// We also want to initialise Vulkan
	init_vulkan();
}
vulkan_app::~vulkan_app()
{
	// Clean vulkan stuff
	clean_vulkan();
	// When the app emds we clean the window related stuff 
	clean_window();
}

void vulkan_app::init_window()
{
	// Initialise glfw
    glfwInit();

	// Window doesn't use opengl
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // Window isn't resizable
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// create a 720p window
    window = glfwCreateWindow(1280, 720, "Learning Vulkan", nullptr, nullptr);
}
void vulkan_app::clean_window()
{
	glfwDestroyWindow(window);

    glfwTerminate();
}

void vulkan_app::init_vulkan()
{
	create_instance();

	setup_debug_messenger();

	pick_physical_device();
}
void vulkan_app::clean_vulkan()
{
	if(enableValidationLayers) {
		vk_ext::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

    vkDestroyInstance(instance, nullptr);
}

// Fetches the extensions required by glfw, and adds another, special one to them, if debugging is enabled 
std::vector<const char*> vulkan_app::getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void vulkan_app::create_instance()
{
	if(enableValidationLayers && !check_validation_layer_support()) {
		std::cerr << "Requested validation layers, but one or more of them hasn't been found.\n";
    	_ASSERT(false);
    }

	// Get the number of extensions
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	
	std::vector<VkExtensionProperties> extensions(extensionCount);
	
	// Write the data to the array
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	// Print the extensions in the terminal
	std::cout << "Available extensions:\n";
	for(const auto& extension : extensions) 
	{
		std::cout << '\t' << extension.extensionName << '\n';
	}


	// Define information about the app itself
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Learning Vulkan";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// Define CREATION INFORMATION for the VkInstance
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// Basically just take all the extensions we can
	auto glfwExtensions = getRequiredExtensions();
	// And enable them
	createInfo.enabledExtensionCount = static_cast<uint32_t>(glfwExtensions.size());
	createInfo.ppEnabledExtensionNames = glfwExtensions.data();

	// If we want the debug - validation layers
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if(enableValidationLayers) {
		// We tell the instance's creation information know how many validation layers we want
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		// And which ones to be exact
		createInfo.ppEnabledLayerNames = validationLayers.data();
		
		// We tell our instance's create info to also create a Debug Util Messenger (it's an additional one, it will be used when the first one's not available).
		fill_debug_utils_messenger_create_info(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
	}
	else {
		// If we don't want em, we just tell it to not bother
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	// Creates the instance using the creation info
	if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		std::cerr << "Could not create a VkInstance based on the given creation information.\n";
		_ASSERT(false);
	}
	else {
		std::cout << "VkInstance created succesfully!\n";
	}
}

// -- DEBUG - VULKAN VALIDATION LAYERS
bool vulkan_app::check_validation_layer_support() 
{
	// Get the number of validation layers
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	// Allocate the validation layers
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	
	// Go through every requested validation layer 
	for(const char* layerName : validationLayers) {
		bool layerFound = false;

		// Go through every available validation layer
		for(const auto& layerProperties : availableLayers) {
			// check if any of the available layers check out with the one requested
			if(strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		// if any of the requested layers can't be found, the function returns as a failed one
		if(!layerFound) {
			return false;
		}
	}

	return true;
}

void vulkan_app::fill_debug_utils_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT; // Just as sometime earlier, this ensures the API it's dealing with the correct struct
	// We define which messages to accept (forward to our debug callback)
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	// Defines the callback we'd like to use
	createInfo.pfnUserCallback = debug_callback;
	createInfo.pUserData = nullptr; // Optional
}
void vulkan_app::setup_debug_messenger()
{
	if(!enableValidationLayers) return;

	// We're going to need creation information for our debug messenger
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	fill_debug_utils_messenger_create_info(createInfo);

	// Actually create the messenger, using the data
	if(vk_ext::CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		std::cerr << "Could not create a Debug Util Messenger.\n";
		_ASSERT(false);
	}
}
VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_app::debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
) 
{
	// Only inform about warnings and errors
	if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		std::cerr << "[VALIDATION LAYER]: " << pCallbackData->pMessage << std::endl;
	}

	return VK_FALSE;
}
// -- DEBUG - END

void vulkan_app::pick_physical_device()
{
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	
}