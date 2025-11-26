#include <filesystem>
#include <iostream>

#include "FENTestRunner.hpp"
#include "PGNTestRunner.hpp"

int main(int argc, char** argv)
{
    std::filesystem::path directory = "test_pgns";
    if (argc > 1) {
        directory = argv[1];
    }

    bool pgnPassed = runPGNTests(directory);
    if (!pgnPassed) {
        std::cerr << "PGN validation failed." << std::endl;
    }

    bool fenPassed = runFENTests();
    if (!fenPassed) {
        std::cerr << "FEN move validation failed." << std::endl;
    }

    return (pgnPassed && fenPassed) ? 0 : 1;
}
