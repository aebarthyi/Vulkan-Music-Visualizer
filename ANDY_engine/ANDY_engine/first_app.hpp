#pragma once

#include "vmv_window.hpp"
#include "vmv_game_object.hpp"
#include "vmv_device.hpp"
#include "vmv_renderer.hpp"
#include <memory>
#include <vector>
#include <iostream>

namespace vmv {
	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;

		void run();

	private:
		void loadGameObjects();

		vmvWindow vmv_Window{ WIDTH, HEIGHT, "HI VULKAN!" };
		vmvDevice vmv_Device{ vmv_Window };
		vmvRenderer vmv_Renderer{ vmv_Window, vmv_Device };
		std::vector<vmvGameObject> gameObjects;
	};
}