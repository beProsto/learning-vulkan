#include "../vulkan_app.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

void vulkan_app::create_image_views()
{
	// we're going to have the same ammount of image views as of images
	swapChainImageViews.resize(swapChainImages.size());

	// We create one image view for every image
	for(size_t i = 0; i < swapChainImageViews.size(); i++) {
		VkImageViewCreateInfo createInfo{}; // once again lol
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i]; // we assign the correct image to this view

		// image format settings
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;

		// Colour settings
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		// Mipmapping and stuff
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
	
		// Actual creation
		if(vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			std::cout << "Couldn't create the image view! ID: " << i << '\n';
			_ASSERT(false);
		}
		else {
			std::cout << "created an image view of ID: " << i << '\n';
		}
	}

}
void vulkan_app::clean_image_views()
{
	for(VkImageView view : swapChainImageViews) {
		vkDestroyImageView(device, view, nullptr);
	}
}