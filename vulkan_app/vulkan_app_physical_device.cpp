#include "vulkan_app.hpp"

#include <iostream>
#include <vector>
#include <set>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// checks if the needed physical device's extensions are supported
bool vulkan_app::check_device_extension_support(VkPhysicalDevice device)
{
	// get how many extensions are abailable
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	// create a dynamic array filled with available extensions
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	// create a copy of our array of required extensions
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	// we iterate through every single available extension
	for(const auto& extension : availableExtensions) {
		// if it is a part of the required extensions, delete it's entry from the copy
		requiredExtensions.erase(extension.extensionName);
	}

	// if our copy is empty (if every required extension has been found), it returns true.
	// false otherwise
	return requiredExtensions.empty();
}
// gets the details of the swapchain support
vulkan_app::SwapChainSupportDetails vulkan_app::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	// query for capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	// query for formats
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if(formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	// query for present modes
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	if(presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}
// Checks if the found GPU is compatible with our needs 
bool vulkan_app::is_device_compatible(VkPhysicalDevice device)
{
	// Get the device's properties (name, type, supported Vulkan version, etc)
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	// Get the device's features (multi-viewport rendering, 64 bit floats, texture compression, etc)
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	// Get the device's available queue families
	vulkan_app::queue_family_indices queueFamInds = find_queue_families(device);

	// checks for extension support
	bool extensionsSupported = check_device_extension_support(device);

	// checks if we have proper swap chain support 
	bool swapChainAdequate = false;
	if(extensionsSupported) {
		// finds the details of our swap chain
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		// the swap chain is adequate if some formats and some present modes were found
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	// Only if we have a geometry shader, 
	// make sure that all the needed queue fimilies were found 
	// and all the extensions needed are supported 
	// and that the swap chain support details are correct
	// can we be sure everything works as expected
	return deviceFeatures.geometryShader && queueFamInds.is_okay() && extensionsSupported && swapChainAdequate;
}
// Finds every GPU, and picks the first compatible one
void vulkan_app::pick_physical_device()
{
	// Check the number of available GPU's
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	// Error if no found
	if(deviceCount == 0) {
		std::cerr << "No vulkan compatible GPU's found.\n";
		_ASSERT(false);
	}

	// Make a list of all of the available GPU's
	std::vector<VkPhysicalDevice> devices(deviceCount);
	// Fill the list
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	std::cout << "There " << (deviceCount == 1 ? "is " : "are ") << deviceCount << " physical device" << (deviceCount == 1 ? "" : "s") << " installed.\n";

	// Check every GPU in the list of all available ones to rule out the first one that matches our expectations
	for(const VkPhysicalDevice& device : devices) {
		if(is_device_compatible(device)) {
			physicalDevice = device;
			break;
		}
	}

	// If we found none, we break lol
	if(physicalDevice == VK_NULL_HANDLE) {
		std::cerr << "None of the GPU's found are suitable for this application.\n";
		_ASSERT(false);
	}

	// Fetches the currently chosen GPU's properties
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
	// Displays it's vendor name and such
	std::cout << "The chosen GPU's name is: " << deviceProperties.deviceName << "! :D\n"; 
}
