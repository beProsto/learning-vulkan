#include "../vulkan_app.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

void vulkan_app::create_render_pass()
{
	// description for the color attacment, which will be the only one for us
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // what to do before rendering
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // what to do after rendering

	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Image set to be presented to the swap chain

	// color attachment reference for out subpass
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0; // out layout(location = 0) vec4 o_Color; 
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// There can be multiple subpasses in a pass - they are exactly what you'd think a pass is lol
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef; // we attach the color attachment reference

	// Subpass dependency
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	// Redner pass creation
	VkRenderPassCreateInfo renderPassCreationInfo{};
	renderPassCreationInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreationInfo.attachmentCount = 1; // how many color attachments we'll be using
	renderPassCreationInfo.pAttachments = &colorAttachment; // a pointer to them
	renderPassCreationInfo.subpassCount = 1; // how many subpasses do we want happening
	renderPassCreationInfo.pSubpasses = &subpass; // a pointer to all of em
	renderPassCreationInfo.dependencyCount = 1;
	renderPassCreationInfo.pDependencies = &dependency; // the subpass dependencies

	if(vkCreateRenderPass(device, &renderPassCreationInfo, nullptr, &renderPass) != VK_SUCCESS) {
		std::cerr << "Failed to create the render pass!\n";
		_ASSERT(false);
	}
	else {
		std::cout << "Created the render pass.\n"; 
	}



}
void vulkan_app::clean_render_pass()
{
	vkDestroyRenderPass(device, renderPass, nullptr);
}