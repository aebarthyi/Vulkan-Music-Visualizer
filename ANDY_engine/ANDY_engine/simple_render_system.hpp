#pragma once

#include "vmv_window.hpp"
#include "vmv_pipeline.hpp"
#include "vmv_game_object.hpp"
#include "vmv_device.hpp"
#include "vmv_renderer.hpp"
#include "vmv_camera.hpp"
#include <memory>
#include <vector>
#include <iostream>

namespace vmv {
	class SimpleRenderSystem {
	public:

		SimpleRenderSystem(vmvDevice& device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandbuffer, std::vector<vmvGameObject> &gameObjects, const vmvCamera &camera);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		vmvDevice &vmv_Device;
		std::unique_ptr<vmvPipeline> vmv_Pipeline;
		VkPipelineLayout pipelineLayout;
	};
}