#include <iostream>

#include "powder_playground.hpp"

#include "util/logger.hpp"

int main()
{
    util::init_logger();

    try {
        pop::run();
    }
    catch (std::exception& e) {
        LOG->error(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
