#include <chrono>
#include <filesystem>
#include <iostream>

#include "FenMoveTester.hpp"
#include "PGNTestRunner.hpp"
#include "PerftTester.hpp"

int main(int argc, char** argv)
{
    const auto start = std::chrono::steady_clock::now();
    std::filesystem::path pgnDirectory = "test_pgns";
    std::filesystem::path fenDirectory = "test_fens";
    std::filesystem::path perftDirectory = "test_perft";

    if (argc > 1) {
        pgnDirectory = argv[1];
    }

    if (argc > 2) {
        fenDirectory = argv[2];
    }

    if (argc > 3) {
        perftDirectory = argv[3];
    }

    std::cout << "Running PGN validation suite..." << std::endl;
    const bool pgnPassed = runPGNTests(pgnDirectory);

    std::cout << "\nRunning FEN move validation suite..." << std::endl;
    const bool fenPassed = runFenMoveTestsFromDirectory(fenDirectory);

    std::cout << "\nRunning PERFT move-count suite..." << std::endl;
    const bool perftPassed = runPerftTestsFromDirectory(perftDirectory);

    const bool allPassed = pgnPassed && fenPassed && perftPassed;
    const auto end = std::chrono::steady_clock::now();
    const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "\nOverall test result: " << (allPassed ? "PASS" : "FAIL")
              << std::endl;
    std::cout << "[PERF] Overall test runtime: " << elapsedMs << " ms" << std::endl;

    return allPassed ? 0 : 1;
}
