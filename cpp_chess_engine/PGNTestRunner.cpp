#include "PGNTestRunner.hpp"

#include <chrono>
#include <iostream>

#include "ChessBoard.hpp"

bool runPGNTests(const std::filesystem::path& directory)
{
    namespace fs = std::filesystem;

    if (!fs::exists(directory)) {
        std::cerr << "PGN directory '" << directory.string()
                  << "' was not found." << std::endl;
        return false;
    }

    bool allPassed = true;
    std::size_t fileCount = 0;
    const auto suiteStart = std::chrono::steady_clock::now();

    for (const auto& entry : fs::directory_iterator(directory)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".pgn") {
            continue;
        }

        const auto fileStart = std::chrono::steady_clock::now();
        ChessBoard validator;
        bool ok = validator.validatePGN(entry.path().string());
        const auto fileEnd = std::chrono::steady_clock::now();
        const auto fileMs = std::chrono::duration_cast<std::chrono::milliseconds>(fileEnd - fileStart).count();
        ++fileCount;

        std::cout << entry.path().filename().string() << ": "
                  << (ok ? "passed" : "failed")
                  << " | " << fileMs << " ms" << std::endl;

        if (!ok) {
            allPassed = false;
        }
    }

    const auto suiteEnd = std::chrono::steady_clock::now();
    const auto suiteMs = std::chrono::duration_cast<std::chrono::milliseconds>(suiteEnd - suiteStart).count();
    const double averageMs = fileCount == 0 ? 0.0 : static_cast<double>(suiteMs) / static_cast<double>(fileCount);

    if (allPassed) {
        std::cout << "All PGN tests passed." << std::endl;
    } else {
        std::cout << "Some PGN tests failed." << std::endl;
    }
    std::cout << "[PERF] PGN total: " << suiteMs << " ms"
              << " | files: " << fileCount
              << " | average: " << averageMs << " ms/file" << std::endl;

    return allPassed;
}
