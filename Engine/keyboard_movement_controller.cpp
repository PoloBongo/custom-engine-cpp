#include "keyboard_movement_controller.h"

namespace lve {
	void KeyboardMovementController::MoveInPlaneXZ(GLFWwindow* _window, float _deltaTime, LveGameObject& _gameObject) {
		glm::vec3 rotate{ 0.f };
		if (glfwGetKey(_window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
		if (glfwGetKey(_window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
		if (glfwGetKey(_window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
		if (glfwGetKey(_window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
			_gameObject.transform.rotation += lookSpeed * _deltaTime * glm::normalize(rotate);
		}
		_gameObject.transform.rotation.x = glm::clamp(_gameObject.transform.rotation.x, -1.5f, 1.5f);
		_gameObject.transform.rotation.y = glm::mod(_gameObject.transform.rotation.y, glm::two_pi<float>());

		float yaw = _gameObject.transform.rotation.y;
		const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
		const glm::vec3 upDir{ 0.f, -1.f, 0.f };

		glm::vec3 moveDir{ 0.f };
		if (glfwGetKey(_window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
		if (glfwGetKey(_window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
		if (glfwGetKey(_window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
		if (glfwGetKey(_window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
		if (glfwGetKey(_window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
		if (glfwGetKey(_window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
			_gameObject.transform.translation += moveSpeed * _deltaTime * glm::normalize(moveDir);
		}
	}


} // namespace lve