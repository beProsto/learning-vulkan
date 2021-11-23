#include "vulkan_app.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void vulkan_app::init_window()
{
	// Initialise glfw
    glfwInit();

	// Window doesn't use opengl
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // Window isn't resizable
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// create a 720p window
    window = glfwCreateWindow(1280, 720, "Learning Vulkan", nullptr, nullptr);
}
void vulkan_app::clean_window()
{
	glfwDestroyWindow(window);

    glfwTerminate();
}
