#pragma once
#include "lve_game_object.h"
#include "lve_constants.h"

namespace lve
{
	class PlaneGameObject
	{
	public:
		static LveGameObject Create(LveDevice& _lveDevice, glm::vec3 _position = VEC3_ZERO, glm::vec3 _scale = VEC3_ONE, glm::vec3 _rotation = VEC3_ZERO)
		{
			std::shared_ptr<LveModel> lveModel = LveModel::CreateModelFromFile(_lveDevice, "Models\\quad.obj");

			auto gameObject = LveGameObject::CreateGameObject();
			gameObject.model = lveModel;
			gameObject.transform.translation = _position;
			gameObject.transform.scale = _scale;
			gameObject.transform.rotation = _rotation;

			return gameObject;
		};
	};
}
