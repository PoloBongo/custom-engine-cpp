#include "Modules/WindowModule.h"
#include "Engine/CoreEngine.h"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>

// std
#include <cassert>
#include <chrono>
#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>
#include "Modules/FileManager.h"
#include "Modules/RHIVulkanModule.h"
#include "Modules/TimeModule.h"
#include "Scene/SceneManager.h"
#include "Tools/functionHelpers.h"

using json = nlohmann::json;


WindowModule::WindowModule(): windowName("An-Gine"), window(nullptr), size(WIDTH, HEIGHT),
                              lastWindowedSize(glm::ivec2(0)), lastWindowedPos(glm::ivec2(0))
{
	lastNonFullscreenWindowMode = WindowMode::WINDOWED;
	updateWindowTitleFrequency  = 0.2f;
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
	const auto window_module          = static_cast<WindowModule*>(glfwGetWindowUserPointer(_window));
	window_module->bFrameBufferResize = true;
	window_module->SetSize(_width, _height);
}

void WindowModule::CubeCursorWindow(const int _color) const
{
	unsigned char pixels[16 * 16 * 4];
	memset(pixels, _color, sizeof(pixels));

	GLFWimage image;
	image.width  = 16;
	image.height = 16;
	image.pixels = pixels;

	GLFWcursor* cursor = glfwCreateCursor(&image, 0, 0);
	glfwSetCursor(window, cursor);
}

void WindowModule::StandardCursorWindow(const GlfwCursorType _cursorType) const
{
	GLFWcursor* glfw_cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);

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
	glfwSetCursor(window, glfw_cursor);
}

void WindowModule::SetInputCursorMode(const GlfwCursorMode _mode)
{
	if (cursorMode != _mode)
	{
		SetCursorMode(_mode);

		glm::i32 glfw_cursor_mode = 0;

		switch (_mode)
		{
			case NORMAL:
				glfw_cursor_mode = GLFW_CURSOR_NORMAL;
				break;
			case HIDDEN:
				glfw_cursor_mode = GLFW_CURSOR_HIDDEN;
				break;
			case DISABLED:
				glfw_cursor_mode = GLFW_CURSOR_DISABLED;
				break;
			case CAPTURED:
				glfw_cursor_mode = GLFW_CURSOR_CAPTURED;
				break;
			case _NONE:
			default:
				std::cout << ("Unhandled cursor mode passed to GLFWWindowWrapper::SetCursorMode: %i\n", static_cast<
					              glm::i32>(_mode));
				break;
		}

		glfwSetInputMode(window, GLFW_CURSOR, glfw_cursor_mode);

		// Enable raw motion when cursor disabled for smoother camera controls
		if (glfw_cursor_mode == GLFW_CURSOR_DISABLED) if (glfwRawMouseMotionSupported()) glfwSetInputMode(
			window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
}


WindowMode WindowModule::StrToWindowMode(const char* _modeStr)
{
	for (uint32_t i = 0; i < static_cast<uint32_t>(WindowMode::_NONE); ++i)
	{
		if (strcmp(WindowModeStrings[i], _modeStr) == 0) return static_cast<WindowMode>(i);
	}

	std::cout << ("Unhandled window mode passed to StrToWindowMode: %s, returning WindowMode::WINDOWED\n", _modeStr) <<
		std::endl;

	return WindowMode::WINDOWED;
}

void WindowModule::Init()
{
	Module::Init();
	// Initialisation GLFW
	if (!glfwInit())
	{
		std::cerr << "Erreur lors de l'initialisation de GLFW" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Configure la fenêtre pour ne pas être redimensionnable
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	// Crée la fenêtre GLFW avec la taille et le nom spécifiés
	window = glfwCreateWindow(size.x, size.y, windowName.c_str(), nullptr, nullptr);
	if (!window)
	{
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
	if (bShowMSInWindowTitle) result += " | " + lve::FloatToString(TimeModule::GetDeltaTimeMilliseconds(), 2) + "ms";
	if (bShowFPSInWindowTitle) result += " - " + lve::FloatToString(1.0f / TimeModule::GetDeltaTime(), 0) + " FPS ";
	return result;
}


bool WindowModule::InitFromConfig()
{
	if (FileManager::FileExists(WINDOW_CONFIG_LOCATION))
		try
		{
			std::ifstream config_file(WINDOW_CONFIG_LOCATION);
			if (!config_file.is_open())
			{
				std::cerr << (std::string("Failed to open window settings config file %s\n") + WINDOW_CONFIG_LOCATION)
					<< std::endl;
				return false;
			}

			json root_object;
			config_file >> root_object;

			bMoveConsoleToOtherMonitor = root_object.value("move console to other monitor on bootup",
			                                               bMoveConsoleToOtherMonitor);
			bAutoRestoreStateOnBootup = root_object.value("auto restore state", bAutoRestoreStateOnBootup);

			if (bAutoRestoreStateOnBootup)
			{
				glm::ivec2 initial_window_pos;

				if (auto it = root_object.find("initial window position");
					it != root_object.end() && it.value().is_array() && it.value().size() == 2)
				{
					initial_window_pos.x = it.value()[0].get<int>();
					initial_window_pos.y = it.value()[1].get<int>();
				}
				else
				{
					initial_window_pos = glm::ivec2(0, 0);
					// Valeur par défaut si la clé n'est pas trouvée ou si elle est mal formée
				}
				position = glm::ivec2(initial_window_pos);

				glm::ivec2 initial_window_size;
				if (auto it = root_object.find("initial window size");
					it != root_object.end() && it.value().is_array() && it.value().size() == 2)
				{
					initial_window_size.x = it.value()[0].get<int>();
					initial_window_size.y = it.value()[1].get<int>();
				}
				else
				{
					initial_window_size = glm::ivec2(800.0f, 600.0f);
					// Valeur par défaut si la clé n'est pas trouvée ou si elle est mal formée
				}
				size = glm::ivec2(initial_window_size);

				bMaximized = root_object.value("maximized", bMaximized);

				const std::string window_mode_str = root_object.value("window mode", "default");
				currentWindowMode                 = StrToWindowMode(window_mode_str.c_str());
			}

			bool b_v_sync_enabled = root_object.value("v-sync", true);
			SetVSyncEnabled(b_v_sync_enabled);

			return true;
		}
		catch (const std::exception& e)
		{
			std::cout << (
				std::string("Failed to parse window settings config file ") + WINDOW_CONFIG_LOCATION + "\n\terror: " + e
				.what()) << std::endl;
			return false;
		}
	std::cout << (std::string("Window settings config file not found: %s\n") + WINDOW_CONFIG_LOCATION) << std::endl;
	return false;
}


void WindowModule::SaveToConfig()
{
	json root_object;

	root_object["move console to other monitor on bootup"] = bMoveConsoleToOtherMonitor;
	root_object["auto restore state"]                      = bAutoRestoreStateOnBootup;
	root_object["initial window position"]                 = {position.x, position.y};
	root_object["initial window size"]                     = {size.x, size.y};
	root_object["maximized"]                               = bMaximized;
	root_object["window mode"]                             = WindowModeToStr();
	root_object["v-sync"]                                  = bVSyncEnabled;

	if (std::ofstream config_file(WINDOW_CONFIG_LOCATION); config_file.is_open())
	{
		config_file << root_object.dump(4); // 4-space indentation for pretty printing
		config_file.close();
	}
	else
	{
		std::cerr << ("Failed to write window settings config file\n");
	}
}
