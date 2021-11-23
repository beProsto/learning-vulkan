#include "vulkan_app.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint> // UINT32_MAX
#include <algorithm> // std::clamp

// INITIALISE THE SWAP CHAIN
void vulkan_app::init_swap_chain()
{
	SwapChainSupportDetails details = query_swap_chain_support(physicalDevice);


}

// scans the list of available swap surface formats, and chooses the most suitable one
VkSurfaceFormatKHR vulkan_app::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	for(const VkSurfaceFormatKHR& format : availableFormats) {
		if(format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return format;
		}
	}

	// if couldn't find a great one, gets the first proposed
	return availableFormats[0];
}
// scans all the available present modes and if there is one that supports mailbox rendering, it chooses it
VkPresentModeKHR vulkan_app::choose_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for(const VkPresentModeKHR& presentMode : availablePresentModes) {
		if(presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return presentMode;
		}
	}
	
	// otherwise just settles on standard vsync
	return VK_PRESENT_MODE_FIFO_KHR;
}
// Chooses the output resolution
VkExtent2D vulkan_app::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities) {
	// if the resolution is the highest it can, keep it that way o.o
	if(capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		int w, h;
		glfwGetFramebufferSize(window, &w, &h);

		VkExtent2D extent = {(uint32_t)w, (uint32_t)h};

		extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return extent;
	}
}