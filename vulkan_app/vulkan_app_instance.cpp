#include "vulkan_app.hpp"

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// Creates the VkInstance itself
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

// Fetches the extensions required by glfw, and adds another, special one to them, if debugging is enabled 
std::vector<const char*> vulkan_app::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}