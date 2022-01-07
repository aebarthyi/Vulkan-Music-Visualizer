#pragma once

#include "vmv_window.hpp"
#include "vmv_pipeline.hpp"
#include "vmv_game_object.hpp"
#include "vmv_device.hpp"
#include "vmv_renderer.hpp"
#include "vmv_camera.hpp"
#include "vmv_frame_info.hpp"
#include <memory>
#include <vector>
#include <iostream>

namespace vmv {
	class SimpleRenderSystem {
	public:

		SimpleRenderSystem(vmvDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(FrameInfo &frameInfo, std::vector<vmvGameObject> &gameObjects);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		vmvDevice &vmv_Device;
		std::unique_ptr<vmvPipeline> vmv_Pipeline;
		VkPipelineLayout pipelineLayout;
	};
}