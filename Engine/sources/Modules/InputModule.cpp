#include "Modules/InputModule.h"
#include <iostream>

InputModule::InputModule(GLFWwindow* window) : m_window(window), m_mouseX(0.0), m_mouseY(0.0)
{
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);
	glfwSetJoystickCallback(joystickCallback);
}

InputModule::~InputModule()
{
}

void InputModule::processInput()
{
	// Reset mouse position
	m_mouseX = 0.0;
	m_mouseY = 0.0;

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


bool InputModule::isKeyPressed(int key)
{
	return m_keys[key];
}

bool InputModule::isMouseButtonPressed(int button)
{
	return m_mouseButtons[button];
}

void InputModule::getMousePosition(double& xPos, double& yPos)
{
	xPos = m_mouseX;
	yPos = m_mouseY;
}

void InputModule::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto inputModule = static_cast<InputModule*>(glfwGetWindowUserPointer(window));

	if (action == GLFW_PRESS) inputModule->m_keys[key] = true;
	else if (action == GLFW_RELEASE) inputModule->m_keys[key] = false;
}

void InputModule::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	auto inputModule = static_cast<InputModule*>(glfwGetWindowUserPointer(window));

	if (action == GLFW_PRESS) inputModule->m_mouseButtons[button] = true;
	else if (action == GLFW_RELEASE) inputModule->m_mouseButtons[button] = false;
}

void InputModule::cursorPositionCallback(GLFWwindow* window, double xPos, double yPos)
{
	auto inputModule = static_cast<InputModule*>(glfwGetWindowUserPointer(window));

	inputModule->m_mouseX = xPos;
	inputModule->m_mouseY = yPos;
}

void InputModule::joystickCallback(int jid, int event)
{
	if (event == GLFW_CONNECTED) std::cout << "Joystick connected: " << jid << std::endl;
	else if (event == GLFW_DISCONNECTED) std::cout << "Joystick disconnected: " << jid << std::endl;
}

void InputModule::gamepadInput(int jid, const GLFWgamepadstate* state)
{
	// Example: Process gamepad input
	if (state->buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS)
	{
		// Button A pressed
	}

	// Example: Use gamepad axes
	float leftStickX = state->axes[GLFW_GAMEPAD_AXIS_LEFT_X];
	float leftStickY = state->axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
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
