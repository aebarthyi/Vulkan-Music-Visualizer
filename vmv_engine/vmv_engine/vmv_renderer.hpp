#pragma once

#include "vmv_window.hpp"
#include "vmv_device.hpp"
#include "vmv_swap_chain.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include <cassert>

namespace vmv {
	class vmvRenderer {
	public:

		vmvRenderer(vmvWindow& window, vmvDevice& device);
		~vmvRenderer();

		vmvRenderer(const vmvRenderer&) = delete;
		vmvRenderer& operator=(const vmvRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return vmv_SwapChain->getRenderPass(); }
		float getAspectRatio() const { return vmv_SwapChain->extentAspectRatio(); }

		VkCommandBuffer beginFrame();
		bool isFrameInProgress() const { return isFrameStarted; }
		VkCommandBuffer getCurrentCommandBuffer() const { 
			assert(isFrameStarted && "cannot get command buffer when frame is not in progress");
			return commandBuffers[currentFrameIndex]; 
		} 

		int getFrameIndex() const { 
			assert(isFrameStarted && "cannot get frame index when frame is not in progress");
			return currentFrameIndex;
		}

		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
		uint32_t getImageCount() const { return vmv_SwapChain->imageCount(); }

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		vmvWindow& vmv_Window;
		vmvDevice& vmv_Device;
		std::unique_ptr<vmvSwapChain> vmv_SwapChain;
		std::vector<VkCommandBuffer> commandBuffers;
		
		uint32_t currentImageIndex;
		int currentFrameIndex{ 0 };
		bool isFrameStarted{ false };
	};
}