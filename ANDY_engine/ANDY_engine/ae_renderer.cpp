#include "ae_renderer.hpp"
#include <stdexcept>
#include <array>
#include <cassert>

namespace ae {

	aeRenderer::aeRenderer(aeWindow& window, aeDevice& device) : ae_Window{ window }, ae_Device{ device }{
		recreateSwapChain();
		createCommandBuffers();
	}

	aeRenderer::~aeRenderer() {
		freeCommandBuffers();
	}

	void  aeRenderer::recreateSwapChain() {
		auto extent = ae_Window.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = ae_Window.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(ae_Device.device());

		if (ae_SwapChain == nullptr) {
			ae_SwapChain = std::make_unique<aeSwapChain>(ae_Device, extent);
		}
		else {
			std::shared_ptr<aeSwapChain> oldSwapChain = std::move(ae_SwapChain);
			ae_SwapChain = std::make_unique<aeSwapChain>(ae_Device, extent, oldSwapChain);
			if (!oldSwapChain->compareSwapFormats(*ae_SwapChain.get())) {
				throw std::runtime_error("Swap chain image format has changed!");
			}
		}
		// fix this
	}

	void aeRenderer::createCommandBuffers() {
		commandBuffers.resize(aeSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = ae_Device.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(ae_Device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers");
		}

	}

	void aeRenderer::freeCommandBuffers() {
		vkFreeCommandBuffers(ae_Device.device(), ae_Device.getCommandPool(), static_cast<float>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

	VkCommandBuffer aeRenderer::beginFrame() {
		assert(!isFrameStarted && "Can't call beginFrame while already in progress");

		auto result = ae_SwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to aquire swap chain image");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer");
		}
		return commandBuffer;
	}

	void aeRenderer::endFrame() {
		assert(isFrameStarted && "Can't end frame while in progress");
		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer");
		}

		auto result = ae_SwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || ae_Window.wasWindowResized()) {
			ae_Window.resetWindowResizedFlag();
			recreateSwapChain();
		}

		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image");
		}
		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % aeSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void aeRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin renderpass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = ae_SwapChain->getRenderPass();
		renderPassInfo.framebuffer = ae_SwapChain->getFrameBuffer(currentImageIndex);
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = ae_SwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(ae_SwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(ae_SwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, ae_SwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}

	void aeRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Can't end renderpass on command buffer from a different frame");
		vkCmdEndRenderPass(commandBuffer);
	}
}
