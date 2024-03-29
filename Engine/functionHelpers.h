#pragma once

#include "lve_constants.h"

namespace lve {
	FLEX_NO_DISCARD float Lerp(float a, float b, float t);
	FLEX_NO_DISCARD glm::vec2 Lerp(const glm::vec2& a, const glm::vec2& b, float t);
	FLEX_NO_DISCARD glm::vec3 Lerp(const glm::vec3& a, const glm::vec3& b, float t);
	FLEX_NO_DISCARD glm::vec4 Lerp(const glm::vec4& a, const glm::vec4& b, float t);

	FLEX_NO_DISCARD inline float Saturate(float val)
	{
		return glm::clamp(val, 0.0f, 1.0f);
	}

	template<typename T>
	FLEX_NO_DISCARD T Saturate(T val)
	{
		return glm::clamp(val, T(0), T(1));
	}
}

