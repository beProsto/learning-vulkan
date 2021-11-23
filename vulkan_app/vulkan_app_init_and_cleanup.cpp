#include "vulkan_app.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

vulkan_app::vulkan_app()
{
	// When the app starts, we initialize the window
	init_window();
	// We also want to initialise Vulkan
	init_vulkan();
}
vulkan_app::~vulkan_app()
{
	// Clean vulkan stuff
	clean_vulkan();
	// When the app emds we clean the window related stuff 
	clean_window();
}

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

void vulkan_app::init_vulkan()
{
	create_instance();

	setup_debug_messenger();

	create_surface();

	pick_physical_device();

	create_logical_device();
}
void vulkan_app::clean_vulkan()
{
	vkDestroyDevice(device, nullptr);

	clean_debug_messenger();

	vkDestroySurfaceKHR(instance, surface, nullptr);

    vkDestroyInstance(instance, nullptr);
}