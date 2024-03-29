#include "lve_window.h"

#include <stdexcept>    

namespace lve {

    // Constructeur  de la classe LveWindow
    LveWindow::LveWindow(int _width, int _height, std::string _name) : width{ _width }, height{ _height }, windowName{ _name } {
        // Initialise la fenêtre GLFW
        InitWindow();
    }

    void LveWindow::InitWindow() {
        // Initialise GLFW
        glfwInit();

        // Configure la fenêtre pour ne pas être redimensionnable
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        // Crée la fenêtre GLFW avec la taille et le nom spécifiés
        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, FramebufferResizeCallBack);
    }

    void LveWindow::CreateWindowSurface(vk::Instance _instance, vk::SurfaceKHR* _surface) {
        if (glfwCreateWindowSurface(static_cast<VkInstance>(_instance), window, nullptr, reinterpret_cast<VkSurfaceKHR*>(_surface)) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface");
        }
    }

    // Destructeur de la classe LveWindow
    LveWindow::~LveWindow() {
        // Détruit la fenêtre GLFW
        glfwDestroyWindow(window);
        // Termine GLFW
        glfwTerminate();
    }

    void LveWindow::FramebufferResizeCallBack(GLFWwindow* _window, int _width, int _height) {
        auto lveWindow = reinterpret_cast<LveWindow*>(glfwGetWindowUserPointer(_window));
        lveWindow->frameBufferResize = true;
        lveWindow->width = _width;
        lveWindow->height = _height;
    }

    void LveWindow::CubeCursorWindow(int color) {
        unsigned char pixels[16 * 16 * 4];
        memset(pixels, color, sizeof(pixels));

        GLFWimage image;
        image.width = 16;
        image.height = 16;
        image.pixels = pixels;

        GLFWcursor* cursor = glfwCreateCursor(&image, 0, 0);
        glfwSetCursor(window, cursor);
    }

    void LveWindow::StandarCursorWindow(GLFW_CURSOR_TYPE CursorType) {
    
        GLFWcursor* GLFWcursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);

        switch (CursorType) {
            case GLFW_CURSOR_TYPE::ARROW:
                glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
                break;
            case GLFW_CURSOR_TYPE::IBEAM:
                glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
                break;
            case GLFW_CURSOR_TYPE::CROSSHAIR:
                glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
                break;
            case GLFW_CURSOR_TYPE::POINTING_HAND:
                glfwCreateStandardCursor(GLFW_POINTING_HAND_CURSOR);
                break;
            case GLFW_CURSOR_TYPE::RESIZE_EW:
                glfwCreateStandardCursor(GLFW_RESIZE_EW_CURSOR);
                break;
            case GLFW_CURSOR_TYPE::RESIZE_NS:
                glfwCreateStandardCursor(GLFW_RESIZE_NS_CURSOR);
                break;
            case GLFW_CURSOR_TYPE::RESIZE_NWSE:
                glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
                break;
            case GLFW_CURSOR_TYPE::RESIZE_NESW:
                glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
                break;
            case GLFW_CURSOR_TYPE::RESIZE_ALL:
                glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
                break;
            case GLFW_CURSOR_TYPE::NOT_ALLOWED:
                glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
                break;


        }
        glfwSetCursor(window, GLFWcursor);
    }

    void LveWindow::SetInputCursorMode(GLFW_CURSOR_MODE mode) {
        switch (mode) {
            case GLFW_CURSOR_MODE::CAPTURED:
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
                break;
            case GLFW_CURSOR_MODE::NORMAL:
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                break;
            case GLFW_CURSOR_MODE::DISABLED:
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                break;
            case GLFW_CURSOR_MODE::HIDDEN:
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
                break;
        }
    }


} // namespace lve
