#pragma once
#include "lve_constants.h"
#include "Transform.h"
#include "GameObject/GameObject.h"

namespace lve
{
	/**
	 * @brief Représente un objet cube dans le jeu.
	 *
	 * Cette classe représente un objet cube dans le jeu. Un objet cube est un type d'objet géométrique tridimensionnel
	 * défini par ses dimensions et sa position dans l'espace tridimensionnel.
	 */
	class CubeGameObject
	{
		public:
			/**
			 * @brief Crée un nouvel objet cube.
			 *
			 * Cette méthode statique crée un nouvel objet cube avec les paramètres spécifiés.
			 *
			 * @param _lveDevice Une référence vers l'objet LveDevice utilisé pour créer l'objet cube.
			 * @param _position La position initiale de l'objet cube dans l'espace tridimensionnel (par défaut : glm::vec3(0, 0, 0)).
			 * @param _scale L'échelle initiale de l'objet cube (par défaut : glm::vec3(1, 1, 1)).
			 * @param _rotation La rotation initiale de l'objet cube (par défaut : glm::vec3(0, 0, 0)).
			 * @return Un pointeur vers le nouvel objet cube créé.
			 */
			static GameObject* Create(LveDevice& _lveDevice, glm::vec3 _position = VEC3_ZERO,
			                          glm::vec3  _scale = VEC3_ONE, glm::vec3 _rotation = VEC3_ZERO);


			/**
			 * @brief Crée un nouvel objet cube avec une couleur aléatoire.
			 *
			 * Cette méthode statique crée un nouvel objet cube avec une couleur aléatoire pour chaque face.
			 *
			 * @param _lveDevice Une référence vers l'objet LveDevice utilisé pour créer l'objet cube.
			 * @param _position La position initiale de l'objet cube dans l'espace tridimensionnel (par défaut : glm::vec3(0, 0, 0)).
			 * @param _scale L'échelle initiale de l'objet cube (par défaut : glm::vec3(1, 1, 1)).
			 * @param _rotation La rotation initiale de l'objet cube (par défaut : glm::vec3(0, 0, 0)).
			 * @return Un pointeur vers le nouvel objet cube créé avec des couleurs aléatoires pour chaque face.
			 */
			static GameObject* CreateColor(LveDevice& _lveDevice, glm::vec3 _position = VEC3_ZERO,
			                               glm::vec3  _scale = VEC3_ONE, glm::vec3 _rotation = VEC3_ZERO);
	};
}
