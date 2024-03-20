#pragma once

#include "lve_window.h"
#include "lve_pipeline.h"

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

        /**
         * @brief Lance l'exécution de l'application.
         *
         * Cette fonction démarre l'exécution de l'application Vulkan, y compris la création de la fenêtre.
         */
        void Run();

    private:
        LveWindow lveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" }; /// Fenêtre de l'application.
        LvePipeline lvePipeline{ "Shaders/simple_shader.vert.spv", "Shaders/simple_shader.frag.spv" };
    };

} // namespace lve
