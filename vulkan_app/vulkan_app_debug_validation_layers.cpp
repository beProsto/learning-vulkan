#include "vulkan_app.hpp"

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// Add functions from extensions
#include "vk_ext/all.hpp"

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
void vulkan_app::clean_debug_messenger()
{
	if(!enableValidationLayers) return;

	// destroy the debug utils messenger
	vk_ext::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
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