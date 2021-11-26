#include "../vulkan_app.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint> // UINT32_MAX
#include <algorithm> // std::clamp
#include <iostream>

// INITIALISE THE SWAP CHAIN
void vulkan_app::create_swap_chain()
{
	SwapChainSupportDetails details = query_swap_chain_support(physicalDevice);

	// Set up most of the needed stuff
	VkSurfaceFormatKHR surfaceFormat = choose_swap_surface_format(details.formats);
    VkPresentModeKHR presentMode = choose_swap_present_mode(details.presentModes);
    VkExtent2D extent = choose_swap_extent(details.capabilities);

	// Specify an ammount of images to be used
	uint32_t imageCount = details.capabilities.minImageCount + 1;
	// if it exceeds the max, set it to be the max
	if(details.capabilities.maxImageCount > 0 && details.capabilities.maxImageCount < imageCount) {
    	imageCount = details.capabilities.maxImageCount;
	}

	// Specify the create info with intent of creation
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	// specify the surface ('canvas') to be used
	createInfo.surface = surface;
	// set the image count to be the one we specified
	createInfo.minImageCount = imageCount;

	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;

	createInfo.imageExtent = extent;
	
	createInfo.imageArrayLayers = 1; // we're doin 2d
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	queue_family_indices indices = find_queue_families(physicalDevice);
	uint32_t queueFamilyIndices[] = {indices.graphics_family_index, indices.present_family_index};

	if (indices.graphics_family_index != indices.present_family_index) { // this will almost never happen
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else { // this instead
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	// applies some needed transform to the image (horizontal flip, for instance)
	createInfo.preTransform = details.capabilities.currentTransform;
	// specifies what to do with the FINAL alpha channel
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	// better performance - discards fragments (pixels) that cannot be seen
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	
	// if we'll ever dynamically change the swap chain, we'll keep a handle to the old one here
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	// creates a swap chain based on our create info :D
	if(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		std::cerr << "Failed to create the SwapChain!\n";
		_ASSERT(false);
	}

	// Get all of the handles to the images the swap chain has created
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

	// Remember these funky variables for later:
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}
void vulkan_app::clean_swap_chain()
{
	vkDestroySwapchainKHR(device, swapChain, nullptr);
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