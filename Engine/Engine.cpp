#include "first_app.h"

#include <iostream>
#include <cstdlib>
#include <stdexcept>


int main() {

    lve::FirstApp app{};

    try{
        app.Run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}