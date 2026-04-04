#include <filesystem>
#include <iostream>

#include "FenMoveTester.hpp"
#include "PGNTestRunner.hpp"

int main(int argc, char** argv)
{
    std::filesystem::path pgnDirectory = "test_pgns";
    if (argc > 1) {
        pgnDirectory = argv[1];
    }

    std::cout << "Running PGN validation suite..." << std::endl;
    const bool pgnPassed = runPGNTests(pgnDirectory);

    std::cout << "\nRunning FEN move validation suite..." << std::endl;
    const bool fenPassed = runFenMoveTests(createDefaultFenMoveTests());

    const bool allPassed = pgnPassed && fenPassed;

    std::cout << "\nOverall test result: " << (allPassed ? "PASS" : "FAIL")
              << std::endl;

    return allPassed ? 0 : 1;
}
