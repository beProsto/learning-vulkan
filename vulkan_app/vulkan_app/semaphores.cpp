#include "../vulkan_app.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

void vulkan_app::create_semaphores()
{
	VkSemaphoreCreateInfo semaphoreCreationInfo{};
	semaphoreCreationInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if(vkCreateSemaphore(device, &semaphoreCreationInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(device, &semaphoreCreationInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS) {
		
		std::cerr << "Couldn't create one or more semaphores!\n";
		_ASSERT(false);
	}
	else {
		std::cout << "Created both semaphores! :D\n";
	}
}
void vulkan_app::clean_semaphores()
{
	vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
}