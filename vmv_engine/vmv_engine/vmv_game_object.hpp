#pragma once

#include "vmv_model.hpp"
#include <memory>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>

namespace vmv {

	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{1.f, 1.f, 1.f};
		glm::vec3 rotation{};
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	struct RigidBody2D {
		float mass{1.0f};
		glm::vec2 velocity{};
	};


	class vmvGameObject {
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, vmvGameObject>;

		static vmvGameObject createGameObject() {
			static id_t currentId = 0;
			return vmvGameObject{ currentId++ };
		}

		vmvGameObject(const vmvGameObject&) = delete;
		vmvGameObject& operator=(const vmvGameObject&) = delete;
		vmvGameObject(vmvGameObject&&) = default;
		vmvGameObject& operator=(vmvGameObject&&) = default;

		id_t getId() {
			return id;
		}

		std::shared_ptr<vmvModel> model{};
		glm::vec3 color{};
		TransformComponent transform{};
		RigidBody2D rigidBody2d;

	private:
		id_t id;
		vmvGameObject(id_t objId) : id{ objId } {}
	};
}