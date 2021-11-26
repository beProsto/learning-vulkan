#include "../vulkan_app.hpp"

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void vulkan_app::create_surface()
{
	// creates a VkSurfaceKHR based on the window
	// it's implementation isn't OS agnostic
	// gonna have to get 'em working in OWL and all that lol
	if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		std::cerr << "failed to create window surface!\n";
		_ASSERT(false);
	}
	else {
		std::cout << "created a surface successfully!\n";
	}
}
