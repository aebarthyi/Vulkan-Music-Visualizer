#pragma once

#include "vmv_camera.hpp"
#include "vmv_game_object.hpp"

#include <vulkan/vulkan.h>

namespace vmv {
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		vmvCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		vmvGameObject::Map& gameObjects;
	};
}