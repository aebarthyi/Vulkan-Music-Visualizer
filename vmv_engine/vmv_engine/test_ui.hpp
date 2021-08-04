#pragma once

#include "vmv_device.hpp"
#include "vmv_window.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <stdexcept>

namespace vmv {

	static void check_vk_result(VkResult err) {
		if (err == 0) return;
		fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
		if (err < 0) abort();
	}

	class vmvTestUI {
	public:
		vmvTestUI(vmvWindow& window, vmvDevice& device, VkRenderPass renderPasss, uint32_t imageCount);
		~vmvTestUI();

		void newFrame();
		void render(VkCommandBuffer commandBuffer);

	private:
		vmvDevice& vmv_Device;
		VkDescriptorPool descriptorPool;

	};
}
