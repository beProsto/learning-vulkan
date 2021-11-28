#include "../vulkan_app.hpp"

#include "../read_file.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

VkShaderModule createShaderModule(const std::string& _code, VkDevice _device);

// We create our graphics pipeline
void vulkan_app::create_pipeline()
{
	std::string vertexShaderCode = readFile("./triangle_v.spv");
	std::string fragmentShaderCode = readFile("./triangle_f.spv");

	std::cout << "\nVERT SHADER CODE SIZE: " << vertexShaderCode.size() << "\nFRAG SHADER CODE SIZE: " << fragmentShaderCode.size() << "\n";

	VkShaderModule vertexShaderMod = createShaderModule(vertexShaderCode, device);
	VkShaderModule fragmentShaderMod = createShaderModule(fragmentShaderCode, device);

	// code lol

	vkDestroyShaderModule(device, fragmentShaderMod, nullptr);
	vkDestroyShaderModule(device, vertexShaderMod, nullptr);
}

// Shader Module Creation (sub shaders / shaders in opengl)
VkShaderModule createShaderModule(const std::string& _code, VkDevice _device)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = _code.size();
	createInfo.pCode = (uint32_t*)_code.data();
	
	VkShaderModule subshader;
	if(vkCreateShaderModule(_device, &createInfo, nullptr, &subshader) != VK_SUCCESS) {
		std::cerr << "Couldn't create a subshader!\n";
		_ASSERT(false);
	}
	
	std::cout << "Created a subshader.\n";

	return subshader;
}