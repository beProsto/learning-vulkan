#include "vulkan_app.hpp"

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// Checks the available queue families
vulkan_app::queue_family_indices vulkan_app::find_queue_families(VkPhysicalDevice device)
{
	// We want to find every queue family, and their indices
	vulkan_app::queue_family_indices queueFamInds;

	// DEBUG - DISPLAYS ALL THE VALUES
	if(enableValidationLayers) {
		std::cout << "\nqueue fam inds before looking:\n" << queueFamInds.debug_unwrap() << "\n";
	}

	// Get all the queue families' properties
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	// iterate through the properties looking for their graphics families
	int i = 0; 
	for(const VkQueueFamilyProperties& queueFamilyProperties : queueFamilies) {
		// We check for the graphics family
		if(queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			queueFamInds.graphics_family_index = i;
			queueFamInds.found_graphics_family = true;
		}

		// We check for the presentation family
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
		if(presentSupport) {
			queueFamInds.present_family_index = i;
			queueFamInds.found_present_family = true;
		}

		// breaks the loop if all needed families have been found
		if(queueFamInds.is_okay()) {
			break;
		}

		i++;
	}

	// DEBUG - DISPLAYS ALL THE VALUES
	if(enableValidationLayers) {
		std::cout << "queue fam inds after looking:\n" << queueFamInds.debug_unwrap() << "\n";
	}

	return queueFamInds;
}