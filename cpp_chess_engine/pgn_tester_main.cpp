#include <filesystem>
#include <iostream>

#include "PGNTestRunner.hpp"

int main(int argc, char** argv)
{
    std::filesystem::path directory = "test_pgns";
    if (argc > 1) {
        directory = argv[1];
    }

    bool passed = runPGNTests(directory);
    if (!passed) {
        std::cerr << "PGN validation failed." << std::endl;
    }

    return passed ? 0 : 1;
}
