#include "vulkan_app.hpp"

#include <iostream>
#include <vector>
#include <set>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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