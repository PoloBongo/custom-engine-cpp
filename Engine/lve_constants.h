#pragma once

#include <glm.hpp>
#include <gtc/quaternion.hpp> 

namespace lve
{
	// Constants
	extern const glm::vec3 VEC3_RIGHT;
	extern const glm::vec3 VEC3_UP;
	extern const glm::vec3 VEC3_FORWARD;
	extern const glm::vec2 VEC2_ONE;
	extern const glm::vec2 VEC2_NEG_ONE;
	extern const glm::vec2 VEC2_ZERO;
	extern const glm::vec3 VEC3_ONE;
	extern const glm::vec3 VEC3_NEG_ONE;
	extern const glm::vec3 VEC3_ZERO;
	extern const glm::vec3 VEC3_GAMMA;
	extern const glm::vec3 VEC3_GAMMA_INVERSE;
	extern const glm::vec4 VEC4_ONE;
	extern const glm::vec4 VEC4_NEG_ONE;
	extern const glm::vec4 VEC4_ZERO;
	extern const glm::vec4 VEC4_GAMMA;
	extern const glm::vec4 VEC4_GAMMA_INVERSE;
	extern const glm::quat QUAT_IDENTITY;
	extern const glm::mat2 MAT2_IDENTITY;
	extern const glm::mat3 MAT3_IDENTITY;
	extern const glm::mat4 MAT4_IDENTITY;
	extern const glm::mat4 MAT4_ZERO;
	extern const uint32_t COLOUR32U_WHITE;
	extern const uint32_t COLOUR32U_BLACK;
	extern const glm::vec4 COLOUR128F_WHITE;
	extern const glm::vec4 COLOUR128F_BLACK;

	extern const std::string EMPTY_STRING;

	extern const uint32_t MAX_TEXTURE_DIM;
}
