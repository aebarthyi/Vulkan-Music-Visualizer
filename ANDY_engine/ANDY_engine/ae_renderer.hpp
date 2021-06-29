#pragma once

#include "ae_window.hpp"
#include "ae_device.hpp"
#include "ae_swap_chain.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include <cassert>

namespace ae {
	class aeRenderer {
	public:

		aeRenderer(aeWindow& window, aeDevice& device);
		~aeRenderer();

		aeRenderer(const aeRenderer&) = delete;
		aeRenderer& operator=(const aeRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return ae_SwapChain->getRenderPass(); }
		float getAspectRatio() const { return ae_SwapChain->extentAspectRatio(); }

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


	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		aeWindow& ae_Window;
		aeDevice& ae_Device;
		std::unique_ptr<aeSwapChain> ae_SwapChain;
		std::vector<VkCommandBuffer> commandBuffers;
		
		uint32_t currentImageIndex;
		int currentFrameIndex{ 0 };
		bool isFrameStarted{ false };
	};
}