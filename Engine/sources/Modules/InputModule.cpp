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
			const float* axes = glfwGetJoystickAxes(jid, nullptr);
			if (axes)
				// Process joystick axes
				for (int i = 0; i < 6; ++i)
				{
					if (axes[i] != 0.0)
					{
						// Example: process joystick axis input
					}
				}

			const unsigned char* buttons = glfwGetJoystickButtons(jid, nullptr);
			if (buttons)
				// Process joystick buttons
				for (int i = 0; i < 16; ++i)
				{
					if (buttons[i] == GLFW_PRESS)
					{
						// Example: process joystick button input
					}
				}

			const unsigned char* hats = glfwGetJoystickHats(jid, nullptr);
			if (hats)
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


bool InputModule::IsKeyPressed(int _key)
{
	return keys[_key];
}

bool InputModule::IsMouseButtonPressed(int _button)
{
	return mouseButtons[_button];
}

void InputModule::GetMousePosition(double& _xPos, double& _yPos)
{
	_xPos = mouseX;
	_yPos = mouseY;
}

void InputModule::KeyCallback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
{
	auto inputModule = static_cast<InputModule*>(glfwGetWindowUserPointer(_window));

	if (_action == GLFW_PRESS) inputModule->keys[_key] = true;
	else if (_action == GLFW_RELEASE) inputModule->keys[_key] = false;
}

void InputModule::MouseButtonCallback(GLFWwindow* _window, int _button, int _action, int _mods)
{
	auto inputModule = static_cast<InputModule*>(glfwGetWindowUserPointer(_window));

	if (_action == GLFW_PRESS) inputModule->mouseButtons[_button] = true;
	else if (_action == GLFW_RELEASE) inputModule->mouseButtons[_button] = false;
}

void InputModule::CursorPositionCallback(GLFWwindow* _window, double _xPos, double _yPos)
{
	auto inputModule = static_cast<InputModule*>(glfwGetWindowUserPointer(_window));

	inputModule->mouseX = _xPos;
	inputModule->mouseY = _yPos;
}

void InputModule::JoystickCallback(int _jid, int _event)
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
	float leftStickX = _state->axes[GLFW_GAMEPAD_AXIS_LEFT_X];
	float leftStickY = _state->axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
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
