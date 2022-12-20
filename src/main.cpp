#include <iostream>

#include "powder_playground.hpp"

int main()
{
    try {
        pop::run();
    }
    catch (std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
