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
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        // Crée la fenêtre GLFW avec la taille et le nom spécifiés
        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    }

    void LveWindow::CreateWindowSurface(VkInstance _instance, VkSurfaceKHR* _surface) {
        if (glfwCreateWindowSurface(_instance, window, nullptr, _surface) != VK_SUCCESS) {
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

} // namespace lve
