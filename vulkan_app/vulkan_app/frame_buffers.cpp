#include "../vulkan_app.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>


void vulkan_app::create_frame_buffers()
{
	// we want the same ammount of fb's as of image views
	swapChainFramebuffers.resize(swapChainImageViews.size());

	for(size_t i = 0; i < swapChainImageViews.size(); i++) {
		VkImageView attachments[] = {swapChainImageViews[i]};

		VkFramebufferCreateInfo framebufferCreationInfo{};
		framebufferCreationInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreationInfo.renderPass = renderPass;
		framebufferCreationInfo.attachmentCount = 1;
		framebufferCreationInfo.pAttachments = attachments;
		framebufferCreationInfo.width  = swapChainExtent.width;
		framebufferCreationInfo.height = swapChainExtent.height;
		framebufferCreationInfo.layers = 1;

		if(vkCreateFramebuffer(device, &framebufferCreationInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			std::cerr << "Could create a Framebuffer of ID: " << i << " !\n";
			_ASSERT(false);
		}
		else {
			std::cout << "Created a Framebuffer of ID: " << i << "\n";
		}
	}
}
void vulkan_app::clean_frame_buffers()
{
	for(VkFramebuffer fb : swapChainFramebuffers) {
		vkDestroyFramebuffer(device, fb, nullptr);
	}
}