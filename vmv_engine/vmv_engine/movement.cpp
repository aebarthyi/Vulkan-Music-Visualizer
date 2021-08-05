#include "movement.hpp"

namespace vmv {
	void MovementController::moveKeyboard(GLFWwindow* window, float dt, vmvGameObject& gameObject) {
		float yaw = gameObject.transform.rotation.y;
		const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
		const glm::vec3 upDir{ 0.f, -1.f, 0.f };

		glm::vec3 moveDir{ 0.f };

		if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
		if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
		if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
		if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
		if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
		if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
			gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
		}
	}

	void MovementController::moveMouse(GLFWwindow* window, float dt, vmvGameObject& gameObject, float sensitivity) {
		glm::vec3 rotate{ 0 };
		double newXpos;
		double newYpos;

		glfwGetCursorPos(window, &newXpos, &newYpos);

		if (newXpos != xPos || newYpos != yPos) {
			rotate.x -= newYpos - yPos;
			rotate.y += newXpos - xPos;

			if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
				gameObject.transform.rotation += sensitivity * dt * glm::normalize(rotate);
			}

			gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
			gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

			xPos = newXpos;
			yPos = newYpos;
		}
	}
}