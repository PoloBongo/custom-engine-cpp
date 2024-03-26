#include "first_app.h"

#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include "EngineTestCode.h"

int main() {
    /**
     * @brief Fonction principale du programme.
     *
     * Cette fonction crée une instance de l'application FirstApp et lance son exécution en appelant la méthode Run().
     *
     * @return EXIT_SUCCESS si l'application s'est terminée avec succès, EXIT_FAILURE sinon.
     */
    lve::FirstApp app{};

    try {
        app.Run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
int main()
{
    EngineTestCode engine;
    engine.TestCode();

    std::cout << "Hello World!\n";
}

    return EXIT_SUCCESS;
}