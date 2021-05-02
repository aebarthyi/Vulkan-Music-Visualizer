#include "simple_render_system.hpp"
#include <stdexcept>
#include <array>
#include <cassert>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace ae {

	struct simplePushConstantData {
		glm::mat2 transform{ 1.f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	SimpleRenderSystem::SimpleRenderSystem(aeDevice& device, VkRenderPass renderPass) : ae_Device{ device }{
		createPipelineLayout();
		createPipeline(renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem() {
		vkDestroyPipelineLayout(ae_Device.device(), pipelineLayout, nullptr);
	}

	void SimpleRenderSystem::createPipelineLayout() {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(simplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(ae_Device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout");
		}
	}

	void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "cannot create pipeline before pipeline layout");
		PipelineConfigInfo pipelineConfig{};
		aePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		ae_Pipeline = std::make_unique<aePipeline>(
			ae_Device, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig
			);
	}

	void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandbuffer, std::vector<aeGameObject> &gameObjects) {
		int i = 0;
		for (auto& obj : gameObjects) {
			i += 1;
			obj.transform2d.rotation = glm::mod<float>(obj.transform2d.rotation + 0.0005f * i, 2.f * glm::pi<float>());
		}

		ae_Pipeline->bind(commandbuffer);

		for (auto& obj : gameObjects) {
			simplePushConstantData push{};
			push.offset = obj.transform2d.translation;
			push.color = obj.color;
			push.transform = obj.transform2d.mat2();

			vkCmdPushConstants(commandbuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(simplePushConstantData), &push);
			obj.model->bind(commandbuffer);
			obj.model->draw(commandbuffer);
		}
	}


}