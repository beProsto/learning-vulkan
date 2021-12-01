#include "../vulkan_app.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

void vulkan_app::create_command_pool()
{
	queue_family_indices queueFamilyIndices = find_queue_families(physicalDevice);

	VkCommandPoolCreateInfo cmdPoolCreationInfo{};
	cmdPoolCreationInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolCreationInfo.queueFamilyIndex = queueFamilyIndices.graphics_family_index;
	cmdPoolCreationInfo.flags = 0; // Optional

	if(vkCreateCommandPool(device, &cmdPoolCreationInfo, nullptr, &commandPool) != VK_SUCCESS) {
		std::cerr << "Couldn't create the command pool!\n";
		_ASSERT(false);
	}
	else {
		std::cout << "Created the command pool. :D\n";
	}
}
void vulkan_app::clean_command_pool()
{
	vkDestroyCommandPool(device, commandPool, nullptr);
}