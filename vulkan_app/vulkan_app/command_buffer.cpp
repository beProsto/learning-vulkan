#include "../vulkan_app.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

void vulkan_app::create_command_buffers()
{
	// We want a single command buffer for every framebuffer
	commandBuffers.resize(swapChainFramebuffers.size());

	// Allocating all of the command buffers
	VkCommandBufferAllocateInfo cmdBufferAllocInfo{};
	cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferAllocInfo.commandPool = commandPool; // link to a cmd pool
	cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // can be submitted to a queue
	cmdBufferAllocInfo.commandBufferCount = (uint32_t) commandBuffers.size(); // how many cmd buffers are needed

	// try and allocate all of the needed command buffer
	if(vkAllocateCommandBuffers(device, &cmdBufferAllocInfo, commandBuffers.data()) != VK_SUCCESS) {
		std::cerr << "Failed to allocate command buffers!\n";
		_ASSERT(false);
	}
	else {
		std::cout << "Allocated the command buffers!\n";
	}

	// Recording the command buffers' commands
	for(size_t i = 0; i < commandBuffers.size(); i++) {	
		VkCommandBufferBeginInfo cmdBufferBeginInfo{};
		cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufferBeginInfo.flags = 0; // Optional
		cmdBufferBeginInfo.pInheritanceInfo = nullptr; // Optional

		// __ START THE RECORDING __
		if(vkBeginCommandBuffer(commandBuffers[i], &cmdBufferBeginInfo) != VK_SUCCESS) {
			std::cerr << "Couldn't start recording to the command buffer of ID: " << i << " !\n";
			_ASSERT(false);
		}

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.framebuffer = swapChainFramebuffers[i]; // The only point to having multiple command buffers
		// every single command buffer corresponds to every single frame buffer possible to be drawn to by it
		renderPassBeginInfo.renderArea.offset = {0, 0};
		renderPassBeginInfo.renderArea.extent = swapChainExtent; // setting the correct sizing
		VkClearValue clearColor = {{{0.3f, 0.4f, 0.9f, 1.0f}}}; // set the colour to clear the screen with - light blue :D
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor; // set the clear color to be it

		// Begin the render pass
		vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Bind the pipeline
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
		// The draw call
		vkCmdDraw(commandBuffers[i], 3, 1, 0, 0); // We draw with 3 vertices, cuz we technically have that ammount

		// End the render pass
		vkCmdEndRenderPass(commandBuffers[i]);

		// __ END THE RECORDING __
		if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			std::cerr << "Couldn't end recording to the command buffer of ID: " << i << " !\n";
			_ASSERT(false);
		}
	}
	
}