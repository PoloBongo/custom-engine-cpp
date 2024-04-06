#pragma once
#include "lve_constants.h"
#include "lve_game_object.h"

namespace lve
{
	class LightGameObject
	{
		public:
			static LveGameObject Create(float _intensity = 10.f, float _radius = 1.f, glm::vec3 _position = VEC3_ZERO,
			                            glm::vec3 _rotate = VEC3_ZERO, glm::vec3 _color = VEC3_ONE)
			{
				LveGameObject game_object              = LveGameObject::CreateGameObject();
				game_object.color                      = _color;
				game_object.transform.translation      = _position;
				game_object.transform.scale.x          = _radius;
				game_object.transform.rotation         = _rotate;
				game_object.pointLight                 = std::make_unique<PointLightComponent>();
				game_object.pointLight->lightIntensity = _intensity;
				return game_object;
			};
	};
}
