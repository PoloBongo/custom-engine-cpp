#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace lve {

    class LveWindow {
    public:
        /**
         * @brief Constructeur de la classe LveWindow.
         *
         * Initialise une instance de LveWindow avec les dimensions spécifiées et le nom de la fenêtre.
         *
         * @param _width La largeur de la fenêtre.
         * @param _height La hauteur de la fenêtre.
         * @param _name Le nom de la fenêtre.
         * @return Rien.
         */
        LveWindow(int _width, int _height, std::string _name);

        /**
         * @brief Destructeur de la classe LveWindow.
         *
         * Détruit la fenêtre GLFW.
         */
        ~LveWindow();

        LveWindow(const LveWindow&) = delete;
        LveWindow& operator=(const LveWindow&) = delete;

        /**
         * @brief Vérifie si la fenêtre doit être fermée.
         *
         * Cette fonction renvoie true si la fenêtre doit être fermée, false sinon.
         *
         * @return true si la fenêtre doit être fermée, false sinon.
         */
        bool ShouldClose() { return glfwWindowShouldClose(window); }

        VkExtent2D GetExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }

        bool WasWindowResized() { return frameBufferResize;  }

        void ResetWindowResizedFlag() { frameBufferResize = false;  }

        /**
        * @brief Crée une surface Vulkan associée à une fenêtre.
        *
        * Cette fonction crée une surface Vulkan associée à une fenêtre, permettant à Vulkan de dessiner dans cette fenêtre.
        *
        * @param _instance L'instance Vulkan utilisée pour créer la surface.
        * @param _surface Un pointeur vers l'objet de surface Vulkan à créer. Ce pointeur sera mis à jour pour contenir la surface créée.
        * @throws Une exception en cas d'échec lors de la création de la surface.
        */
        void CreateWindowSurface(VkInstance _instance, VkSurfaceKHR* _surface);

    private:

        static void FramebufferResizeCallBack(GLFWwindow* _window, int _width, int _height);

        /**
         * @brief Initialise la fenêtre GLFW.
         *
         * Cette fonction initialise GLFW, configure la fenêtre pour être non redimensionnable et crée une fenêtre GLFW avec la taille et le nom spécifiés.
         */
        void InitWindow();

        int width; // Largeur de la fenêtre
        int height; // Hauteur de la fenêtre
        bool frameBufferResize = false;

        std::string windowName; // Nom de la fenêtre
        GLFWwindow* window; // Fenêtre GLFW
    };

} // namespace lve
