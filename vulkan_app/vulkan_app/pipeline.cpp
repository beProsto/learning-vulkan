#include "../vulkan_app.hpp"

#include "../read_file.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

VkShaderModule createSubShader(const std::string& _code);

// We create our graphics pipeline
void vulkan_app::create_pipeline()
{
	std::string vertexShaderCode = readFile("./triangle_v.spv");
	std::string fragmentShaderCode = readFile("./triangle_f.spv");

	std::cout << "\nVERT SHADER CODE SIZE: " << vertexShaderCode.size() << "\nFRAG SHADER CODE SIZE: " << fragmentShaderCode.size() << "\n";
}

// Shader Module Creation (sub shaders / shaders in opengl)
VkShaderModule createSubShader(const std::string& _code)
{
	return VkShaderModule{};
}