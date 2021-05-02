#include "ae_window.hpp"
#include <stdexcept>

namespace ae {
	aeWindow::aeWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name } {
		initWindow();
	}

	aeWindow::~aeWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void aeWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
	}

	void aeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface");
		}
	}

	void aeWindow::frameBufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto ae_window = reinterpret_cast<aeWindow*>(glfwGetWindowUserPointer(window));
		ae_window->frameBufferResized = true;
		ae_window->width = width;
		ae_window->height = height;
	}
}