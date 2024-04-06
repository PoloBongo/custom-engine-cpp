#pragma once

#include <cmath>
#include <glm.hpp>
#include <iostream>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Component.h"

class Transform final : public Component
{
	public:
		// Méthode pour récupérer et définir la position
		glm::vec3 GetPosition() const { return position; }
		void      SetPosition(const float& _x, const float& _y, const float& _z) { position = glm::vec3(_x, -_y, -_z); }
		void      SetPosition(const glm::vec3& _position) { position = _position; }

		// Méthode pour récupérer et définir la rotation en utilisant un angle
		float GetRotation() const { return rotation; }
		void  SetRotation(const float _angle) { rotation = _angle; }

		// Méthode pour récupérer et définir l'échelle
		glm::vec3 GetScale() const { return scale; }

		void SetScale(const float& _scaleX, const float& _scaleY, const float& _scaleZ)
		{
			scale = glm::vec3(_scaleX, _scaleY, _scaleZ);
		}

		void SetScale(const glm::vec3& _scale) { scale = _scale; }

		glm::vec3 TransformPoint() const
		{
			// Création de la matrice de transformation
			auto rotation_matrix = glm::mat4(1.0f);
			rotation_matrix      = translate(rotation_matrix, position);
			rotation_matrix      = rotate(rotation_matrix, rotation, glm::vec3(1.0f, 0.0f, 0.0f));
			rotation_matrix      = rotate(rotation_matrix, rotation, glm::vec3(0.0f, 1.0f, 0.0f));
			rotation_matrix      = rotate(rotation_matrix, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
			rotation_matrix      = glm::scale(rotation_matrix, scale);
			// Transformation du point
			const glm::vec4 transformed_point = rotation_matrix * glm::vec4(position, 1.0f);

			// Récupération des coordonnées transformées
			return glm::vec3(transformed_point.x, transformed_point.y, transformed_point.z);
		}

		// Méthode pour afficher les propriétés du Transform
		void Display();

	private:
		glm::vec3 vectorZero = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 vectorOne  = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 position   = vectorZero;
		glm::vec3 scale      = vectorOne;
		float     rotation   = 0.f;
};
