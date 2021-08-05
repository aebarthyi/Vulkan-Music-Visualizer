#pragma once

#include "vmv_game_object.hpp"
#include "vmv_window.hpp"

namespace vmv {
	class MovementController {
	public:
		struct KeyMappings {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_SPACE;
            int moveDown = GLFW_KEY_LEFT_SHIFT;
		};

        void moveKeyboard(GLFWwindow* window, float dt, vmvGameObject& gameObject);
        void moveMouse(GLFWwindow* window, float dt, vmvGameObject& gameObject, float sensitivity);

        KeyMappings keys{};
        float moveSpeed{ 3.f };
        double xPos = 0;
        double yPos = 0;
	};

}