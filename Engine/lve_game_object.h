#pragma once

#include "lve_model.h"

//std
#include <memory>


namespace lve {
	struct Transform2dComponent {
		glm::vec2 translation{}; //position offset
		glm::vec2 scale{ 1.f,1.f };
		float rotation;

		glm::mat2 mat2() { 
			const float sin_roation = glm::sin(rotation);
			const float cos_rotation = glm::cos(rotation);
			glm::mat2 rotMatrix{ {cos_rotation, sin_roation},{-sin_roation, cos_rotation} };

			glm::mat2 scaleMat{ {scale.x, .0f}, {.0f, scale.y} };
			return rotMatrix * scaleMat;
		}
	};

	class LveGameObject
	{
	public:
		using id_t = unsigned int;
		
		static LveGameObject CreateGameObject() {
			static id_t currentId = 0;
			return LveGameObject{ currentId++ };
		}

		LveGameObject(const LveGameObject&) = delete;
		LveGameObject& operator=(const LveGameObject&) = delete;
		LveGameObject(LveGameObject&&) = default;
		LveGameObject& operator=(LveGameObject&&) = default;

		id_t GetIde() { return id; }

		std::shared_ptr<LveModel> model{};
		glm::vec3 color{};
		Transform2dComponent transform2d{};

	private:
		LveGameObject(id_t _objId) : id(_objId) {}

		id_t id;
	};
} //namespace lve


