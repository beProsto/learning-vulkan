#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

class vulkan_app
{
public:
	vulkan_app();
	~vulkan_app();

	void begin();

private:
	void init_window();
	void clean_window();

	void init_vulkan();
	void clean_vulkan();

	void create_instance();

	std::vector<const char*> getRequiredExtensions();

	// -- DEBUG - VULKAN VALIDATION LAYERS
	bool check_validation_layer_support();
	
	void setup_debug_messenger();
public:
	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	);
private:
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
	
	const bool enableValidationLayers = true;

	VkDebugUtilsMessengerEXT debugMessenger;
	// -- DEBUG - END

	GLFWwindow*	window;
	VkInstance	instance;
};