#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class vulkan_app
{
public:
	vulkan_app();
	~vulkan_app();

	void on_loop();

private:
	void init_window();
	void clean_window();

	void init_vulkan();
	void clean_vulkan();

	void create_instance();

	GLFWwindow*	window;
	VkInstance	instance;
};