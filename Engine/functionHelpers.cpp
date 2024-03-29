#include "functionHelpers.h"

namespace lve {
	float Lerp(const float a, const float b, float t)
	{
		return a * (1.0f - t) + b * t;
	}

	glm::vec2 Lerp(const glm::vec2& a, const glm::vec2& b, float t)
	{
		return a * (1.0f - t) + b * t;
	}

	glm::vec3 Lerp(const glm::vec3& a, const glm::vec3& b, float t)
	{
		return a * (1.0f - t) + b * t;
	}

	glm::vec4 Lerp(const glm::vec4& a, const glm::vec4& b, float t)
	{
		return a * (1.0f - t) + b * t;
	}
} // namespace lve