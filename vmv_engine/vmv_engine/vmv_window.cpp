#include "vmv_window.hpp"
#include <stdexcept>

namespace vmv {
	vmvWindow::vmvWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name } {
		initWindow();
	}

	vmvWindow::~vmvWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void vmvWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
	}

	void vmvWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface");
		}
	}

	void vmvWindow::frameBufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto vmv_window = reinterpret_cast<vmvWindow*>(glfwGetWindowUserPointer(window));
		vmv_window->frameBufferResized = true;
		vmv_window->width = width;
		vmv_window->height = height;
	}
}