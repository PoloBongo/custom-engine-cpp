#pragma once

#include "lve_window.h"
#include "lve_device.h"
#include "lve_renderer.h"
#include "lve_game_object.h"

//std
#include <memory>
#include <vector>

namespace lve {

    /**
     * @brief Classe représentant la première application utilisant Vulkan.
     *
     * Cette classe gère l'exécution de la première application Vulkan, incluant la création de la fenêtre.
     */
    class FirstApp {
    public:
        static constexpr int WIDTH = 800; ///< Largeur de la fenêtre par défaut.
        static constexpr int HEIGHT = 600; ///< Hauteur de la fenêtre par défaut.

        FirstApp();
        ~FirstApp();

        FirstApp(const FirstApp&) = delete;
        FirstApp operator=(const FirstApp&) = delete;

        /**
         * @brief Lance l'exécution de l'application.
         *
         * Cette fonction démarre l'exécution de l'application Vulkan, y compris la création de la fenêtre.
         */
        void Run();

    private:
        void LoadGameObjects();

        LveWindow lveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" }; /// Fenêtre de l'application.
        LveDevice lveDevice{ lveWindow };
        LveRenderer lveRenderer{ lveWindow, lveDevice };

        VkPipelineLayout pipelineLayout;
        std::vector<LveGameObject> gameObjects;
    };

} // namespace lve
