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
				LveGameObject gameObj              = LveGameObject::CreateGameObject();
				gameObj.color                      = _color;
				gameObj.transform.translation      = _position;
				gameObj.transform.scale.x          = _radius;
				gameObj.transform.rotation         = _rotate;
				gameObj.pointLight                 = std::make_unique<PointLightComponent>();
				gameObj.pointLight->lightIntensity = _intensity;
				return gameObj;
			};
	};
}
