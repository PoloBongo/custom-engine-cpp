#include "Modules/WindowModule.h"
#include "CoreEngine.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>

// std
#include <cassert>
#include <chrono>
#include <stdexcept>

#include "functionHelpers.h"
#include "Modules/RHIVulkanModule.h"
#include "Modules/TimeModule.h"
#include "Scene/SceneManager.h"




WindowModule::WindowModule(): windowName("VulkanIty"), window(nullptr), size(WIDTH,HEIGHT), lastWindowedSize(glm::ivec2(0)), lastWindowedPos(glm::ivec2(0))
{
	updateWindowTitleFrequency = 0.2f;
}

// Destructeur de la classe LveWindow
WindowModule::~WindowModule()
{
	// Détruit la fenêtre GLFW
	glfwDestroyWindow(window);
	// Termine GLFW
	glfwTerminate();
}

void WindowModule::CreateWindowSurface(const vk::Instance _instance, vk::SurfaceKHR* _surface) const
{
	if (glfwCreateWindowSurface(_instance, window, nullptr, reinterpret_cast<VkSurfaceKHR*>(_surface)) !=
		VK_SUCCESS)
		throw std::runtime_error("failed to create window surface");
}

void WindowModule::FrameBufferResizeCallBack(GLFWwindow* _window, const int _width, const int _height)
{
	const auto window_module = static_cast<WindowModule*>(glfwGetWindowUserPointer(_window));
	window_module->bFrameBufferResize = true;
	window_module->SetSize(_width, _height);
}

void WindowModule::CubeCursorWindow(const int _color) const
{
	unsigned char pixels[16 * 16 * 4];
	memset(pixels, _color, sizeof(pixels));

	GLFWimage image;
	image.width = 16;
	image.height = 16;
	image.pixels = pixels;

	GLFWcursor* cursor = glfwCreateCursor(&image, 0, 0);
	glfwSetCursor(window, cursor);
}

void WindowModule::StandardCursorWindow(const GlfwCursorType _cursorType) const
{
	GLFWcursor* GLFW_cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);

	switch (_cursorType)
	{
	case ARROW:
		glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		break;
	case IBEAM:
		glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
		break;
	case CROSSHAIR:
		glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
		break;
	case POINTING_HAND:
		glfwCreateStandardCursor(GLFW_POINTING_HAND_CURSOR);
		break;
	case RESIZE_EW:
		glfwCreateStandardCursor(GLFW_RESIZE_EW_CURSOR);
		break;
	case RESIZE_NS:
		glfwCreateStandardCursor(GLFW_RESIZE_NS_CURSOR);
		break;
	case RESIZE_NWSE:
		glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
		break;
	case RESIZE_NESW:
		glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
		break;
	case RESIZE_ALL:
		glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
		break;
	case NOT_ALLOWED:
		glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
		break;
	}
	glfwSetCursor(window, GLFW_cursor);
}

void WindowModule::SetInputCursorMode(const GlfwCursorMode _mode) const
{
	switch (_mode)
	{
	case CAPTURED:
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
		break;
	case NORMAL:
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		break;
	case DISABLED:
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		break;
	case HIDDEN:
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		break;
	}
}


void WindowModule::Init()
{
	Module::Init();
	// Initialisation GLFW
	if (!glfwInit()) {
		std::cerr << "Erreur lors de l'initialisation de GLFW" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Configure la fenêtre pour ne pas être redimensionnable
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	// Crée la fenêtre GLFW avec la taille et le nom spécifiés
	window = glfwCreateWindow(size.x, size.y, windowName.c_str(), nullptr, nullptr);
	if (!window) {
		std::cerr << "Erreur lors de la création de la fenêtre GLFW" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, FrameBufferResizeCallBack);
}

void WindowModule::Start()
{
	Module::Start();
	sceneManager = moduleManager->GetModule<SceneManager>();
}

void WindowModule::FixedUpdate()
{
	Module::FixedUpdate();
}

void WindowModule::Update()
{
	Module::Update();

	if (!ShouldClose())
	{
		glfwPollEvents();
		secondsSinceTitleUpdate += TimeModule::GetDeltaTime();
		if (secondsSinceTitleUpdate >= updateWindowTitleFrequency)
		{
			secondsSinceTitleUpdate = 0.0f;
			SetWindowTitle(GenerateWindowTitle());
		}
	}
	else
	{
		moduleManager->GetModule<RHIModule>()->GetDevice()->Device().waitIdle();
		Engine::GetInstance()->Quit();
	}
}

void WindowModule::PreRender()
{
	Module::PreRender();
}

void WindowModule::Render()
{
	Module::Render();
	
}

void WindowModule::RenderGui()
{
	Module::RenderGui();
}

void WindowModule::PostRender()
{
	Module::PostRender();

	//window->display();
}

void WindowModule::Release()
{
	Module::Release();

	//window->close();
}
void WindowModule::Finalize()
{
	Module::Finalize();

	//window->close();
}

std::string WindowModule::GenerateWindowTitle() const
{
	std::string result = windowName;
	result += " | " + sceneManager->GetMainScene()->GetName();
	if (bShowMSInWindowTitle)
	{
		result += " | " + lve::FloatToString(TimeModule::GetDeltaTimeMilliseconds(), 2) + "ms";
	}
	if (bShowFPSInWindowTitle)
	{
		result += " - " + lve::FloatToString(1.0f / TimeModule::GetDeltaTime(), 0) + " FPS ";
	}


	return result;
}
