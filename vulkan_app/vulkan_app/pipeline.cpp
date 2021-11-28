#include "../vulkan_app.hpp"

#include "../read_file.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

VkShaderModule createShaderModule(const std::string& _code, VkDevice _device);
VkPipelineShaderStageCreateInfo getShaderStageCreateInfo(VkShaderModule _module, VkShaderStageFlagBits _type);

// We create our graphics pipeline
void vulkan_app::create_pipeline()
{
	// We read the shaders from files
	std::string vertexShaderCode = readFile("./triangle_v.spv");
	std::string fragmentShaderCode = readFile("./triangle_f.spv");

	std::cout << "\nVERT SHADER CODE SIZE: " << vertexShaderCode.size() << "\nFRAG SHADER CODE SIZE: " << fragmentShaderCode.size() << "\n";

	// We create shader modules out of the shaders' code
	VkShaderModule vertexShaderMod = createShaderModule(vertexShaderCode, device);
	VkShaderModule fragmentShaderMod = createShaderModule(fragmentShaderCode, device);

	// We define stage creation infos for both the vertex shader stage and the fragment shader stage
	VkPipelineShaderStageCreateInfo shaderStagesCreationInfos[2] = {
		getShaderStageCreateInfo(vertexShaderMod, VK_SHADER_STAGE_VERTEX_BIT),
		getShaderStageCreateInfo(fragmentShaderMod, VK_SHADER_STAGE_FRAGMENT_BIT)
	};
	// Now shaderStagesCreationInfos[0] is for the vertex shader stage and shaderStagesCreationInfos[1] if for the fragment shader stage

	

	// We free the shader modules
	vkDestroyShaderModule(device, fragmentShaderMod, nullptr);
	vkDestroyShaderModule(device, vertexShaderMod, nullptr);
}

// Gets the shader stages' create infos
VkPipelineShaderStageCreateInfo getShaderStageCreateInfo(VkShaderModule _module, VkShaderStageFlagBits _type)
{
	VkPipelineShaderStageCreateInfo shaderStageInfo{};
	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo.stage = _type;
	shaderStageInfo.module = _module;
	shaderStageInfo.pName = "main";

	return shaderStageInfo;
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