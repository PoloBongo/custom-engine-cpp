#pragma once
#include "lve_constants.h"
#include "Transform.h"
#include "GameObject/GameObject.h"

namespace lve
{
	/**
	 * @brief Représente un objet plan dans le jeu.
	 *
	 * Cette classe représente un objet plan dans le jeu. Un objet plan est un type d'objet géométrique bidimensionnel
	 * défini par sa position, sa taille et son orientation dans l'espace tridimensionnel.
	 */
	class PlaneGameObject
	{
		public:
			/**
			 * @brief Crée un nouvel objet plan.
			 *
			 * Cette méthode statique crée un nouvel objet plan avec les paramètres spécifiés.
			 *
			 * @param _lveDevice Une référence vers l'objet LveDevice utilisé pour créer l'objet plan.
			 * @param _position La position initiale de l'objet plan dans l'espace tridimensionnel (par défaut : glm::vec3(0, 0, 0)).
			 * @param _scale L'échelle initiale de l'objet plan (par défaut : glm::vec3(1, 1, 1)).
			 * @param _rotation La rotation initiale de l'objet plan (par défaut : glm::vec3(0, 0, 0)).
			 * @return Un pointeur vers le nouvel objet plan créé.
			 */
			static GameObject* Create(LveDevice& _lveDevice, glm::vec3 _position = VEC3_ZERO,
			                          glm::vec3  _scale = VEC3_ONE, glm::vec3 _rotation = VEC3_ZERO);
	};
}
