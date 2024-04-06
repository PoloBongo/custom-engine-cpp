#include "keyboard_movement_controller.h"

namespace lve
{
	void KeyboardMovementController::MoveInPlaneXZ(GLFWwindow*    _window, float _deltaTime,
	                                               LveGameObject& _gameObject) const
	{
		glm::vec3 rotate{0.f};
		if (glfwGetKey(_window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
		if (glfwGetKey(_window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
		if (glfwGetKey(_window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
		if (glfwGetKey(_window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

		if (dot(rotate, rotate) > std::numeric_limits<
			    float>::epsilon())
			_gameObject.transform.rotation += lookSpeed * _deltaTime * normalize(rotate);
		_gameObject.transform.rotation.x = glm::clamp(_gameObject.transform.rotation.x, -1.5f, 1.5f);
		_gameObject.transform.rotation.y = glm::mod(_gameObject.transform.rotation.y, glm::two_pi<float>());

		float               yaw = _gameObject.transform.rotation.y;
		const glm::vec3     forward_dir{sin(yaw), 0.f, cos(yaw)};
		const glm::vec3     right_dir{forward_dir.z, 0.f, -forward_dir.x};
		constexpr glm::vec3 up_dir{0.f, -1.f, 0.f};

		glm::vec3 move_dir{0.f};
		if (glfwGetKey(_window, keys.moveForward) == GLFW_PRESS) move_dir += forward_dir;
		if (glfwGetKey(_window, keys.moveBackward) == GLFW_PRESS) move_dir -= forward_dir;
		if (glfwGetKey(_window, keys.moveRight) == GLFW_PRESS) move_dir += right_dir;
		if (glfwGetKey(_window, keys.moveLeft) == GLFW_PRESS) move_dir -= right_dir;
		if (glfwGetKey(_window, keys.moveUp) == GLFW_PRESS) move_dir += up_dir;
		if (glfwGetKey(_window, keys.moveDown) == GLFW_PRESS) move_dir -= up_dir;

		if (dot(move_dir, move_dir) > std::numeric_limits<
			    float>::epsilon())
			_gameObject.transform.translation += moveSpeed * _deltaTime * normalize(move_dir);
	}
} // namespace lve
