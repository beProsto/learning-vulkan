#include "../vulkan_app.hpp"

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

void vulkan_app::init_vulkan()
{
	create_instance();

	setup_debug_messenger();

	create_surface();

	pick_physical_device();

	create_logical_device();

	create_swap_chain();
}
void vulkan_app::clean_vulkan()
{
	clean_swap_chain();

	vkDestroyDevice(device, nullptr);

	clean_debug_messenger();

	vkDestroySurfaceKHR(instance, surface, nullptr);

    vkDestroyInstance(instance, nullptr);
}