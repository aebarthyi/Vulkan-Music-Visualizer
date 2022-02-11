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
	class PointLightSystem {
	public:

		PointLightSystem(vmvDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void update(FrameInfo& frameInfo, GlobalUbo &globalUbo);
		void render(FrameInfo &frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		vmvDevice &vmv_Device;
		std::unique_ptr<vmvPipeline> vmv_Pipeline;
		VkPipelineLayout pipelineLayout;
	};
}