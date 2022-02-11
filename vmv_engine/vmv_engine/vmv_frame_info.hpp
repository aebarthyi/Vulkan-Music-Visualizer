#pragma once

#include "vmv_camera.hpp"
#include "vmv_game_object.hpp"

#include <vulkan/vulkan.h>

namespace vmv {
	#define MAX_LIGHTS 10

	struct PointLight {
		glm::vec4 position{};
		glm::vec4 color{};
	};

	struct GlobalUbo {
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::vec4 ambientLightColors{ 1.f,1.f,1.f,0.2f };
		PointLight pointLights[MAX_LIGHTS];
		int numLights;
	};

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		vmvCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		vmvGameObject::Map& gameObjects;
	};
}