#pragma once
#include "lve_constants.h"
#include "lve_game_object.h"

namespace lve
{
	class PlaneGameObject
	{
		public:
			static LveGameObject Create(LveDevice& _lveDevice, glm::vec3 _position = VEC3_ZERO,
			                            glm::vec3  _scale = VEC3_ONE, glm::vec3 _rotation = VEC3_ZERO)
			{
				std::shared_ptr<LveModel> lve_model = LveModel::CreateModelFromFile(_lveDevice, "Models\\quad.obj");

				auto game_object                  = LveGameObject::CreateGameObject();
				game_object.model                 = lve_model;
				game_object.transform.translation = _position;
				game_object.transform.scale       = _scale;
				game_object.transform.rotation    = _rotation;

				return game_object;
			};
	};
}
