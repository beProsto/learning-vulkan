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

	// Vertex input state creation info - Vertex input state is somewhat like VA's in OpenGL afaik
	// For now empty - we're not feeding the shaders any data anyway
	VkPipelineVertexInputStateCreateInfo vertexInputCreationInfo{};
	vertexInputCreationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputCreationInfo.vertexBindingDescriptionCount = 0;
	vertexInputCreationInfo.pVertexBindingDescriptions = nullptr;
	vertexInputCreationInfo.vertexAttributeDescriptionCount = 0;
	vertexInputCreationInfo.pVertexAttributeDescriptions = nullptr;

	// Input assembly - essentially just sets the stuffyou'd tell opengl at drawtime :p
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreationInfo{};
	inputAssemblyCreationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyCreationInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyCreationInfo.primitiveRestartEnable = VK_FALSE;
	
	// Now we define a Viewport o. o (how the resulting data is rescaled onto the image)
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width  = (float) swapChainExtent.width;
	viewport.height = (float) swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	// The scsisor defines how the now rescaled data is to be cut
	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = swapChainExtent;

	// Viewport creation info - regards the resulting viewport
	VkPipelineViewportStateCreateInfo viewportCreationInfo{};
	viewportCreationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportCreationInfo.viewportCount = 1; // how many viewports
	viewportCreationInfo.pViewports = &viewport; // pointer to em
	viewportCreationInfo.scissorCount = 1; // how much scissors
	viewportCreationInfo.pScissors = &scissor; // the pointer to them

	// rasterizer create info defines rasterization rules
	VkPipelineRasterizationStateCreateInfo rasterizerCreationInfo{};
	rasterizerCreationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	
	rasterizerCreationInfo.depthClampEnable = VK_FALSE; // if true, it will keep geometry with z-buffer value lower than 0.0 even tho it should be discarded
	rasterizerCreationInfo.rasterizerDiscardEnable = VK_FALSE; // if true, disables rasterization
	rasterizerCreationInfo.polygonMode = VK_POLYGON_MODE_FILL; // other modes are LINE and POINT, it's a 'fill style' per se
	rasterizerCreationInfo.lineWidth = 1.0f; // how THICC te lines are in pixels (fragments)
	
	rasterizerCreationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizerCreationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE; // face culling
	
	rasterizerCreationInfo.depthBiasEnable = VK_FALSE; // it can add some value to the depth buffer (idk what for, the tutorial says it's useful in shadow mapping)
	rasterizerCreationInfo.depthBiasConstantFactor = 0.0f; // Optional
	rasterizerCreationInfo.depthBiasClamp = 0.0f; // Optional
	rasterizerCreationInfo.depthBiasSlopeFactor = 0.0f; // Optional

	// Multisampling is a way of anti-aliasing
	VkPipelineMultisampleStateCreateInfo multisamplingCreationInfo{};
	multisamplingCreationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisamplingCreationInfo.sampleShadingEnable = VK_FALSE;
	multisamplingCreationInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisamplingCreationInfo.minSampleShading = 1.0f; // Optional
	multisamplingCreationInfo.pSampleMask = nullptr; // Optional
	multisamplingCreationInfo.alphaToCoverageEnable = VK_FALSE; // Optional
	multisamplingCreationInfo.alphaToOneEnable = VK_FALSE; // Optional

	// Color blending defines what to do when the alpha part isn't 1.0
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = 
		VK_COLOR_COMPONENT_R_BIT | 
		VK_COLOR_COMPONENT_G_BIT | 
		VK_COLOR_COMPONENT_B_BIT | 
		VK_COLOR_COMPONENT_A_BIT; 
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	// Creation info for the color blending state
	VkPipelineColorBlendStateCreateInfo colorBlendingCreationInfo{};
	colorBlendingCreationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendingCreationInfo.logicOpEnable = VK_FALSE;
	colorBlendingCreationInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlendingCreationInfo.attachmentCount = 1;
	colorBlendingCreationInfo.pAttachments = &colorBlendAttachment; // we give it the attachements we just defined
	colorBlendingCreationInfo.blendConstants[0] = 0.0f; // Optional
	colorBlendingCreationInfo.blendConstants[1] = 0.0f; // Optional
	colorBlendingCreationInfo.blendConstants[2] = 0.0f; // Optional
	colorBlendingCreationInfo.blendConstants[3] = 0.0f; // Optional

	// Dynamic states - viewport can be dynamic
	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};
	VkPipelineDynamicStateCreateInfo dynamicStatesCreationInfo{};
	dynamicStatesCreationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStatesCreationInfo.dynamicStateCount = 2; // how many
	dynamicStatesCreationInfo.pDynamicStates = dynamicStates; // pointer to them

	// The pipeline layout defines the uniforms in our shaders
	VkPipelineLayoutCreateInfo pipelineLayoutCreationInfo{};
	pipelineLayoutCreationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreationInfo.setLayoutCount = 0; // Optional
	pipelineLayoutCreationInfo.pSetLayouts = nullptr; // Optional
	pipelineLayoutCreationInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutCreationInfo.pPushConstantRanges = nullptr; // Optional

	if(vkCreatePipelineLayout(device, &pipelineLayoutCreationInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		std::cerr << "Couldn't create a pipeline layout!\n";
		_ASSERT(false);
	}
	else {
		std::cout << "Created the pipeline layout.\n";
	}

	// We free the shader modules
	vkDestroyShaderModule(device, fragmentShaderMod, nullptr);
	vkDestroyShaderModule(device, vertexShaderMod, nullptr);
}
void vulkan_app::clean_pipeline()
{
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
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