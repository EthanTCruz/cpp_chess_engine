#include "PGNTestRunner.hpp"

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

    for (const auto& entry : fs::directory_iterator(directory)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".pgn") {
            continue;
        }

        ChessBoard validator;
        bool ok = validator.validatePGN(entry.path().string());
        std::cout << entry.path().filename().string() << ": "
                  << (ok ? "passed" : "failed") << std::endl;

        if (!ok) {
            allPassed = false;
        }
    }

    if (allPassed) {
        std::cout << "All PGN tests passed." << std::endl;
    } else {
        std::cout << "Some PGN tests failed." << std::endl;
    }

    return allPassed;
}
