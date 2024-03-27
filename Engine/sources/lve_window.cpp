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

} // namespace lve
