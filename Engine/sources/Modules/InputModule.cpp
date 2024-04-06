#include "Modules/InputModule.h"
#include <iostream>

InputModule::InputModule(GLFWwindow* _window) : window(_window), mouseX(0.0), mouseY(0.0)
{
	glfwSetKeyCallback(_window, KeyCallback);
	glfwSetMouseButtonCallback(_window, MouseButtonCallback);
	glfwSetCursorPosCallback(_window, CursorPositionCallback);
	glfwSetJoystickCallback(JoystickCallback);
}

InputModule::~InputModule()
{
}

void InputModule::ProcessInput()
{
	// Reset mouse position
	mouseX = 0.0;
	mouseY = 0.0;

	// Process joystick and gamepad input
	for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid)
	{
		if (glfwJoystickPresent(jid))
		{
			if (const float* axes = glfwGetJoystickAxes(jid, nullptr))
				// Process joystick axes
				for (int i = 0; i < 6; ++i)
				{
					if (axes[i] != 0.0)
					{
						// Example: process joystick axis input
					}
				}

			if (const unsigned char* buttons = glfwGetJoystickButtons(jid, nullptr))
				// Process joystick buttons
				for (int i = 0; i < 16; ++i)
				{
					if (buttons[i] == GLFW_PRESS)
					{
						// Example: process joystick button input
					}
				}

			if (const unsigned char* hats = glfwGetJoystickHats(jid, nullptr))
				// Process joystick hats
				for (int i = 0; i < 4; ++i)
				{
					if (hats[i] != GLFW_HAT_CENTERED)
					{
						// Example: process joystick hat input
					}
				}
		}
	}

	glfwPollEvents();
}


bool InputModule::IsKeyPressed(const int _key)
{
	return keys[_key];
}

bool InputModule::IsMouseButtonPressed(const int _button)
{
	return mouseButtons[_button];
}

void InputModule::GetMousePosition(double& _xPos, double& _yPos) const
{
	_xPos = mouseX;
	_yPos = mouseY;
}

void InputModule::KeyCallback(GLFWwindow* _window, const int _key, int _scancode, const int _action, int _mods)
{
	const auto input_module = static_cast<InputModule*>(glfwGetWindowUserPointer(_window));

	if (_action == GLFW_PRESS) input_module->keys[_key] = true;
	else if (_action == GLFW_RELEASE) input_module->keys[_key] = false;
}

void InputModule::MouseButtonCallback(GLFWwindow* _window, const int _button, const int _action, int _mods)
{
	const auto input_module = static_cast<InputModule*>(glfwGetWindowUserPointer(_window));

	if (_action == GLFW_PRESS) input_module->mouseButtons[_button] = true;
	else if (_action == GLFW_RELEASE) input_module->mouseButtons[_button] = false;
}

void InputModule::CursorPositionCallback(GLFWwindow* _window, const double _xPos, const double _yPos)
{
	const auto input_module = static_cast<InputModule*>(glfwGetWindowUserPointer(_window));

	input_module->mouseX = _xPos;
	input_module->mouseY = _yPos;
}

void InputModule::JoystickCallback(const int _jid, const int _event)
{
	if (_event == GLFW_CONNECTED) std::cout << "Joystick connected: " << _jid << std::endl;
	else if (_event == GLFW_DISCONNECTED) std::cout << "Joystick disconnected: " << _jid << std::endl;
}

void InputModule::GamepadInput(int _jid, const GLFWgamepadstate* _state)
{
	// Example: Process gamepad input
	if (_state->buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS)
	{
		// Button A pressed
	}

	// Example: Use gamepad axes
	float left_stick_x = _state->axes[GLFW_GAMEPAD_AXIS_LEFT_X];
	float left_stick_y = _state->axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
}

void InputModule::Init()
{
	Module::Init();
}

void InputModule::Start()
{
	Module::Start();
}

void InputModule::FixedUpdate()
{
	Module::FixedUpdate();
}

void InputModule::Update()
{
	Module::Update();
}

void InputModule::PreRender()
{
	Module::PreRender();
}

void InputModule::Render()
{
	Module::Render();
}

void InputModule::RenderGui()
{
	Module::RenderGui();
}

void InputModule::PostRender()
{
	Module::PostRender();
}

void InputModule::Release()
{
	Module::Release();
}

void InputModule::Finalize()
{
	Module::Finalize();
}
