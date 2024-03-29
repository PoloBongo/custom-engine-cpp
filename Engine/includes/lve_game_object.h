#pragma once

#include "lve_model.h"

#include <vulkan/vulkan.hpp>

//std
#include <memory>
#include <unordered_map>
#include <gtc/matrix_transform.hpp>

namespace lve {
	struct TransformComponent {
		glm::vec3 translation{}; // Position offset
		glm::vec3 scale{ 1.f,1.f, 1.f };
		glm::vec3 rotation{};

		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	struct PointLightComponent {
		float lightIntensity = 1.0f;
	};

	class LveGameObject
	{
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, LveGameObject>;
		
		static LveGameObject CreateGameObject() {
			static id_t currentId = 0;
			return LveGameObject{ currentId++ };
		}

		LveGameObject(const LveGameObject&) = delete;
		LveGameObject& operator=(const LveGameObject&) = delete;
		LveGameObject(LveGameObject&&) = default;
		LveGameObject& operator=(LveGameObject&&) = default;

		id_t GetId() { return id; }


		/**
		 * @brief Crée un objet de jeu représentant une lumière ponctuelle.
		 *
		 * Cette méthode statique crée un objet de jeu représentant une lumière ponctuelle avec les paramètres spécifiés.
		 *
		 * @param _intensity L'intensité de la lumière.
		 * @param _radius Le rayon d'éclairage de la lumière.
		 * @param _color La couleur de la lumière.
		 * @return L'objet de jeu représentant la lumière ponctuelle.
		 */
		static LveGameObject MakePointLight(
			float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));
		
		glm::vec3 color{};
		TransformComponent transform{};

		// Optional pointer components
		std::shared_ptr<LveModel> model{};
		std::unique_ptr<PointLightComponent> pointLight = nullptr;
	private:
		LveGameObject(id_t _objId) : id(_objId) {}

		id_t id;
	};
} //namespace lve


