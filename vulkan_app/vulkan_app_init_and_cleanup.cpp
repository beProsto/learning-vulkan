#include "vulkan_app.hpp"

#include <iostream>
#include <vector>
#include <set>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// Add functions from extensions
#include "vk_ext/all.hpp"

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

	if(enableValidationLayers) {
		vk_ext::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(instance, surface, nullptr);

    vkDestroyInstance(instance, nullptr);
}

// Fetches the extensions required by glfw, and adds another, special one to them, if debugging is enabled 
std::vector<const char*> vulkan_app::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

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

void vulkan_app::create_instance()
{
	if(enableValidationLayers && !check_validation_layer_support()) {
		std::cerr << "Requested validation layers, but one or more of them hasn't been found.\n";
    	_ASSERT(false);
    }

	// Get the number of extensions
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	
	std::vector<VkExtensionProperties> extensions(extensionCount);
	
	// Write the data to the array
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	// Print the extensions in the terminal
	std::cout << "Available extensions:\n";
	for(const auto& extension : extensions) 
	{
		std::cout << '\t' << extension.extensionName << '\n';
	}


	// Define information about the app itself
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Learning Vulkan";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// Define CREATION INFORMATION for the VkInstance
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// Basically just take all the extensions we can
	auto glfwExtensions = getRequiredExtensions();
	// And enable them
	createInfo.enabledExtensionCount = static_cast<uint32_t>(glfwExtensions.size());
	createInfo.ppEnabledExtensionNames = glfwExtensions.data();

	// If we want the debug - validation layers
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if(enableValidationLayers) {
		// We tell the instance's creation information know how many validation layers we want
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		// And which ones to be exact
		createInfo.ppEnabledLayerNames = validationLayers.data();
		
		// We tell our instance's create info to also create a Debug Util Messenger (it's an additional one, it will be used when the first one's not available).
		fill_debug_utils_messenger_create_info(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
	}
	else {
		// If we don't want em, we just tell it to not bother
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	// Creates the instance using the creation info
	if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		std::cerr << "Could not create a VkInstance based on the given creation information.\n";
		_ASSERT(false);
	}
	else {
		std::cout << "VkInstance created succesfully!\n";
	}
}

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

// Creates a VkDevice
void vulkan_app::create_logical_device()
{
	// a little test of wether or not std::set stores repeating data;
	std::set<uint32_t> testSet = {1, 1, 2, 2, 4, 4, 4, 4, 3, 3, 3, 5, 3, 4, 5};
	for(uint32_t testSetElement : testSet) {
		std::cout << "DEB TEST SET EL: " << testSetElement << "\n";
	}
	// after this little test we see that std::set discards repeating elements and sorts the ones left
	// that's actually great for our usecase here, as we don't want any queues with the same indices!

	float queuePriority = 1.0f;

	// we find all the queue families we need
	queue_family_indices indices = find_queue_families(physicalDevice);

	// THIS IS THE "DEPRECATED" (AND BROKEN) WAY TO DO THIS

	// const uint32_t QUEUE_FAMILIES_COUNT = 2;

	// // we create one of em needed queues (first we define it's data)
	// VkDeviceQueueCreateInfo graphicsQueueCreateInfo{};
	// graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	// graphicsQueueCreateInfo.queueFamilyIndex = indices.graphics_family_index;
	// graphicsQueueCreateInfo.queueCount = 1;
	// // we assign our queue it's priority, 1.0 means the highest
	// graphicsQueueCreateInfo.pQueuePriorities = &queuePriority; // i don't think this is safe at all - queuePriority could be well gone when the gpu tries to get it's value from a pointer :/

	// // we create the second one of em needed queues (same thing rly)
	// VkDeviceQueueCreateInfo presentQueueCreateInfo{};
	// presentQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	// presentQueueCreateInfo.queueFamilyIndex = indices.present_family_index;
	// presentQueueCreateInfo.queueCount = 1;
	// presentQueueCreateInfo.pQueuePriorities = &queuePriority;

	// // we want to have an array of 2 queues, as that's how much we have
	// VkDeviceQueueCreateInfo queueCreateInfos[QUEUE_FAMILIES_COUNT] = {
	// 	graphicsQueueCreateInfo,
	// 	presentQueueCreateInfo
	// }; 

	// THAT WAS THE "DEPRECATED" (AND BROKEN) WAY TO DO THIS

	// Let's create a set of the needed queue families' indices
	std::set<uint32_t> queueCreateInfoIndices = {
		indices.graphics_family_index,
		indices.present_family_index
	};
	// you see - any repeatitions will be discarded, thus if the graphics family index and the present family index are the same, 
	// we avoid having two conflicting device queues.

	// This dynamic array will contain all of our needed queue create infos
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	// We loop through every element of our previously defined set
	// Remember - repeatitions get discarded, so it should lead to only one cycle of this loop
	// if both indices are equal
	for(uint32_t queueFamilyIndex : queueCreateInfoIndices) {
		// we create one of em needed queues (first we define it's data)
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex; // we assign the currently checked queue family index
		queueCreateInfo.queueCount = 1;
		// we assign our queue it's priority, 1.0 means the highest
		queueCreateInfo.pQueuePriorities = &queuePriority; // i don't think this is safe at all - queuePriority could be well gone when the gpu tries to get it's value from a pointer :/

		// we assign our newly created queue creation info into the previously created dynamic array of those
		queueCreateInfos.push_back(queueCreateInfo);
	}

	// we specify the device features we want (geometry shaders for instance)
	VkPhysicalDeviceFeatures deviceFeatures{};

	// the actual create info for the device
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	// we tell it about the queues we wanted to create also
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = queueCreateInfos.size();
	// we tell it about the device features we want (geometry shaders etc...)
	createInfo.pEnabledFeatures = &deviceFeatures;

	// we don't have to anymore, these will simply be derived from the instance
	// however we'll still do it for the sake of support
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}
	
	// We enable the required extensions (deviceExtensions) to be able to use the swapchain
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	
	// actually creates the logical device!
	if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
    	std::cerr << "couldn't create a logical device!\n";
		_ASSERT(false);
	}
	else {
		std::cout << "created a logical device successfully\n";
	}

	// sets graphicsQueue as a handle to the graphics queue in our logical device :p
	vkGetDeviceQueue(device, indices.graphics_family_index, 0, &graphicsQueue);
	// sets presentQueue as a handle to the present queue in our logical device
	vkGetDeviceQueue(device, indices.present_family_index, 0, &presentQueue);

	// Technically speaking, in most cases, as we've made sure to account for earlier,
	// these queues will be the same.
	// Thus we can optimise this whole ordeal by just making it into one, graphics queue.

	std::cout << "got em queues\n";
}

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


// -- DEBUG - VULKAN VALIDATION LAYERS
bool vulkan_app::check_validation_layer_support() 
{
	// Get the number of validation layers
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	// Allocate the validation layers
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	
	// Go through every requested validation layer 
	for(const char* layerName : validationLayers) {
		bool layerFound = false;

		// Go through every available validation layer
		for(const auto& layerProperties : availableLayers) {
			// check if any of the available layers check out with the one requested
			if(strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		// if any of the requested layers can't be found, the function returns as a failed one
		if(!layerFound) {
			return false;
		}
	}

	return true;
}

void vulkan_app::fill_debug_utils_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT; // Just as sometime earlier, this ensures the API it's dealing with the correct struct
	// We define which messages to accept (forward to our debug callback)
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	// Defines the callback we'd like to use
	createInfo.pfnUserCallback = debug_callback;
	createInfo.pUserData = nullptr; // Optional
}
void vulkan_app::setup_debug_messenger()
{
	if(!enableValidationLayers) return;

	// We're going to need creation information for our debug messenger
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	fill_debug_utils_messenger_create_info(createInfo);

	// Actually create the messenger, using the data
	if(vk_ext::CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		std::cerr << "Could not create a Debug Util Messenger.\n";
		_ASSERT(false);
	}
}
VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_app::debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
) 
{
	// Only inform about warnings and errors
	if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		std::cerr << "[VALIDATION LAYER]: " << pCallbackData->pMessage << std::endl;
	}

	return VK_FALSE;
}
// -- DEBUG - END