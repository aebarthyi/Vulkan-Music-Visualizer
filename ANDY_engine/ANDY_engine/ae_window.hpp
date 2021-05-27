#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace ae {
	class aeWindow {
	public:
		aeWindow(int w, int h, std::string name);
		~aeWindow();

		bool shouldClose() { return glfwWindowShouldClose(window); }

		bool wasWindowResized() { return frameBufferResized; }

		void resetWindowResizedFlag() { frameBufferResized = false; }

		VkExtent2D getExtent() { return { static_cast<uint32_t>(width),  static_cast<uint32_t>(height) }; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

		GLFWwindow* getWindow() { return window; }

	private:
		static void frameBufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow();

		int width;
		int height;
		bool frameBufferResized = false;
		std::string windowName;
		GLFWwindow* window;
	
	};
}