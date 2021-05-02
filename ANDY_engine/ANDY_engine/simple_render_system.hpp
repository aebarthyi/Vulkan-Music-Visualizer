#pragma once

#include "ae_window.hpp"
#include "ae_pipeline.hpp"
#include "ae_game_object.hpp"
#include "ae_device.hpp"
#include "ae_renderer.hpp"
#include <memory>
#include <vector>
#include <iostream>

namespace ae {
	class SimpleRenderSystem {
	public:

		SimpleRenderSystem(aeDevice& device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandbuffer, std::vector<aeGameObject> &gameObjects);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		aeDevice &ae_Device;
		std::unique_ptr<aePipeline> ae_Pipeline;
		VkPipelineLayout pipelineLayout;
	};
}