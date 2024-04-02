#pragma once

#include "lve_model.h"

#include <vulkan/vulkan.hpp>

//std
#include <memory>
#include <unordered_map>
#include <gtc/matrix_transform.hpp>

namespace lve {

	struct TransformComponent {
		glm::vec3 translation{}; /**< Décalage de position. */
		glm::vec3 scale{ 1.f,1.f, 1.f }; /**< Facteurs d'échelle sur les axes x, y et z. */
		glm::vec3 rotation{}; /**< Angles de rotation autour des axes x, y et z (en radians). */

		/**
		 * @brief Retourne la matrice de transformation correspondante.
		 *
		 * Cette fonction retourne la matrice de transformation correspondant aux décalages de position,
		 * échelles et rotations spécifiés dans la structure TransformComponent.
		 *
		 * @return La matrice de transformation.
		 */
		glm::mat4 mat4();

		/**
		 * @brief Retourne la matrice de transformation normale.
		 *
		 * Cette fonction retourne la matrice de transformation normale correspondante aux décalages de position,
		 * échelles et rotations spécifiés dans la structure TransformComponent.
		 * Cette matrice est utilisée pour la transformation des normales des surfaces afin de les maintenir correctement orientées
		 * lors de transformations non uniformes.
		 *
		 * @return La matrice de transformation normale.
		 */
		glm::mat3 normalMatrix();
	};


	struct PointLightComponent {
		float lightIntensity = 1.0f; /**< Intensité lumineuse de la lumière ponctuelle. */
	};


	class LveGameObject
	{
	public:
		using id_t = unsigned int; /**< Type pour identifier les objets. */
		using Map = std::unordered_map<id_t, LveGameObject>; /**< Type pour représenter une carte d'objets avec des identifiants. */
		
		/**
		 * @brief Crée un nouvel objet de jeu.
		 *
		 * Cette fonction crée un nouvel objet de jeu en générant un identifiant unique pour cet objet.
		 *
		 * @return L'objet de jeu créé.
		 */
		static LveGameObject CreateGameObject() {
			static id_t currentId = 0; /**< Identifiant actuel pour les objets créés. */
			return LveGameObject{ currentId++ }; /**< Retourne un nouvel objet de jeu avec l'identifiant incrémenté. */
		}

		LveGameObject(const LveGameObject&) = delete; /**< Constructeur de copie supprimé. */
		LveGameObject& operator=(const LveGameObject&) = delete; /**< Opérateur d'affectation supprimé. */
		LveGameObject(LveGameObject&&) = default; /**< Constructeur de déplacement par défaut. */
		LveGameObject& operator=(LveGameObject&&) = default; /**< Opérateur de déplacement par défaut. */

		/**
		 * @brief Obtient l'identifiant de l'objet.
		 *
		 * @return L'identifiant de l'objet.
		 */
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
		static LveGameObject MakePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));
		
		glm::vec3 color{}; /**< Couleur de l'objet. */
		TransformComponent transform{}; /**< Composant de transformation de l'objet. */

		// Optional pointer components
		std::shared_ptr<LveModel> model{}; /**< Modèle de l'objet. */
		std::unique_ptr<PointLightComponent> pointLight = nullptr; /**< Composant de lumière ponctuelle de l'objet, s'il y en a un. */

	private:
		id_t id; /**< Identifiant de l'objet. */

		/**
		 * @brief Constructeur de la classe LveGameObject.
		 *
		 * @param _objId Identifiant de l'objet.
		 */
		LveGameObject(id_t _objId) : id(_objId) {}

	};
} //namespace lve


