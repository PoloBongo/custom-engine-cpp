#include "first_app.h"

namespace lve {

    void FirstApp::Run() {
        /**
         * @brief Boucle principale de l'application.
         *
         * Cette boucle s'exécute tant que la fenêtre de l'application n'a pas été fermée.
         */
        while (!lveWindow.ShouldClose()) {
            glfwPollEvents(); // Traite tous les événements de la fenêtre
        }
    }

} // namespace lve
