#pragma once

#include "ae_window.hpp"
#include "ae_game_object.hpp"
#include "ae_device.hpp"
#include "ae_renderer.hpp"
#include <memory>
#include <vector>
#include <iostream>

namespace ae {
	class FirstApp {
	public:
		static constexpr int WIDTH = 1920;
		static constexpr int HEIGHT = 1080;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;

		void run();

	private:
		void loadGameObjects();

		aeWindow ae_Window{ WIDTH, HEIGHT, "HI VULKAN!" };
		aeDevice ae_Device{ ae_Window };
		aeRenderer ae_Renderer{ ae_Window, ae_Device };
		std::vector<aeGameObject> gameObjects;
	};
}