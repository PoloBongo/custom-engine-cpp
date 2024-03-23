#pragma once

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>

namespace lve {
	class LveCamera {
	public:

		void SetOrthographicProjection(float _left, float _right, float _top, float _bottom, float _near, float _far);

		void SetPerspectiveProjection(float _fovy, float _aspect, float _near, float _far);

		void SetViewDirection(glm::vec3 _position, glm::vec3 _direction, glm::vec3 _up = glm::vec3{0.f,-1.f, 0.f});

		void SetViewTarget(glm::vec3 _position, glm::vec3 _target, glm::vec3 _up = glm::vec3{0.f,-1.f, 0.f});

		void SetViewYXZ(glm::vec3 _position, glm::vec3 _rotation);

		const glm::mat4& GetProjection() const { return projectionMatrix; }
		const glm::mat4& GetView() const { return viewMatrix; }

	private:
		glm::mat4 projectionMatrix{ 1.f };
		glm::mat4 viewMatrix{ 1.f };

	};
}
