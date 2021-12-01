#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include "macro/debug_unwrap.hpp"

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

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	bool check_device_extension_support(VkPhysicalDevice device);

	bool is_device_compatible(VkPhysicalDevice device);
	void pick_physical_device();

	void create_logical_device();
	void create_surface();

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
	SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device);

public:
	struct queue_family_indices
	{
		bool found_graphics_family = false;
		uint32_t graphics_family_index = 0;

		bool found_present_family = false;
		uint32_t present_family_index = 0;

		bool is_okay() {
			return found_graphics_family && found_present_family;
		}

		MAKE_DEBUG_UNWRAP(
			found_graphics_family, 
			graphics_family_index,
			found_present_family,
			present_family_index
		);
	};

private:
	queue_family_indices find_queue_families(VkPhysicalDevice device);

	/// SWAP CHAIN
	VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);

	void create_swap_chain();
	void clean_swap_chain();

	void create_image_views();
	void clean_image_views();

	void create_pipeline();
	void clean_pipeline();

	void create_render_pass();
	void clean_render_pass();

private:
	std::vector<const char*> getRequiredExtensions();

	// -- DEBUG - VULKAN VALIDATION LAYERS
	bool check_validation_layer_support();
	void fill_debug_utils_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void setup_debug_messenger();
	void clean_debug_messenger();
	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	);
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
	
	const bool enableValidationLayers = true;

	VkDebugUtilsMessengerEXT debugMessenger;
	// -- DEBUG - END

	// The picked GPU
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	// The logical device
	VkDevice device;
	// The graphics queue - regards graphics operations
	VkQueue graphicsQueue;
	// The present queue - regards displaying said graphics
	VkQueue presentQueue;
	// The surface - the canvas for us to draw upon
	VkSurfaceKHR surface;
	// The a 'SwapChain
	VkSwapchainKHR swapChain;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	// The images provided by the swap chain
	std::vector<VkImage> swapChainImages;
	// The images' views - these will mark the area of the images that shall be written to
	std::vector<VkImageView> swapChainImageViews;

	// The pipeline layout
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	
	// The window
	GLFWwindow*	window;
	// The Vulkan instance - it will be used to do anything regarding the Vulkan API
	VkInstance	instance;
};