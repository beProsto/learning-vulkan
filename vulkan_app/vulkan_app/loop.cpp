#include "../vulkan_app.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// The app start & loop
void vulkan_app::begin()
{
	

	// Game loop
	while (!glfwWindowShouldClose(window)) {
        // Event polling every frame
		glfwPollEvents();
    }
}