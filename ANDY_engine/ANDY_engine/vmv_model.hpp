#pragma once
#include "vmv_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace vmv {
	class vmvModel {
	public:

		struct Vertex {
			glm::vec3 position;
			glm::vec3 color;
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		vmvModel(vmvDevice &vmv_device, const std::vector<Vertex>& vertices);
		~vmvModel();

		vmvModel(const vmvModel&) = delete;
		vmvModel& operator=(const vmvModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);

		vmvDevice& vmv_device;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};
}
