#include "lve_game_object.h"

namespace lve
{
	glm::mat4 TransformComponent::Mat4()
	{
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		return glm::mat4{
			{
				scale.x * (c1 * c3 + s1 * s2 * s3),
				scale.x * (c2 * s3),
				scale.x * (c1 * s2 * s3 - c3 * s1),
				0.0f,
			},
			{
				scale.y * (c3 * s1 * s2 - c1 * s3),
				scale.y * (c2 * c3),
				scale.y * (c1 * c3 * s2 + s1 * s3),
				0.0f,
			},
			{
				scale.z * (c2 * s1),
				scale.z * (-s2),
				scale.z * (c1 * c2),
				0.0f,
			},
			{translation.x, translation.y, translation.z, 1.0f}
		};
	}

	glm::mat3 TransformComponent::NormalMatrix() const
	{
		const float     c3        = glm::cos(rotation.z);
		const float     s3        = glm::sin(rotation.z);
		const float     c2        = glm::cos(rotation.x);
		const float     s2        = glm::sin(rotation.x);
		const float     c1        = glm::cos(rotation.y);
		const float     s1        = glm::sin(rotation.y);
		const glm::vec3 inv_scale = 1.0f / scale;

		return glm::mat3{
			{
				inv_scale.x * (c1 * c3 + s1 * s2 * s3),
				inv_scale.x * (c2 * s3),
				inv_scale.x * (c1 * s2 * s3 - c3 * s1),
			},
			{
				inv_scale.y * (c3 * s1 * s2 - c1 * s3),
				inv_scale.y * (c2 * c3),
				inv_scale.y * (c1 * c3 * s2 + s1 * s3),
			},
			{
				inv_scale.z * (c2 * s1),
				inv_scale.z * (-s2),
				inv_scale.z * (c1 * c2),
			}
		};
	}

	LveGameObject LveGameObject::MakePointLight(const float _intensity, float _radius, const glm::vec3 _color)
	{
		LveGameObject game_obj              = CreateGameObject();
		game_obj.color                      = _color;
		game_obj.transform.scale.x          = 0.2f;
		game_obj.pointLight                 = std::make_unique<PointLightComponent>();
		game_obj.pointLight->lightIntensity = _intensity;
		return game_obj;
	}
} // namespace lve
