#pragma once

#include "ae_model.hpp"
#include <memory>

namespace ae {

	struct Transform2dComponent {
		glm::vec2 translation{};
		glm::vec2 scale{1.f, 1.f};
		float rotation;

		glm::mat2 mat2() {
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotationMat{ {c, s}, {-s, c} };
			glm::mat2 scaleMat{ {scale.x, .0f}, {.0f, scale.y} };
			return rotationMat * scaleMat;
		}
	};

	struct RigidBody2D {
		float mass{1.0f};
		glm::vec2 velocity{};
	};


	class aeGameObject {
	public:
		using id_t = unsigned int;

		static aeGameObject createGameObject() {
			static id_t currentId = 0;
			return aeGameObject{ currentId++ };
		}

		aeGameObject(const aeGameObject&) = delete;
		aeGameObject& operator=(const aeGameObject&) = delete;
		aeGameObject(aeGameObject&&) = default;
		aeGameObject& operator=(aeGameObject&&) = default;

		id_t getId() {
			return id;
		}

		std::shared_ptr<aeModel> model{};
		glm::vec3 color{};
		Transform2dComponent transform2d;
		RigidBody2D rigidBody2d;

	private:
		id_t id;
		aeGameObject(id_t objId) : id{ objId } {}
	};
}