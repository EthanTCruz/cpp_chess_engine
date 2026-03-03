#include "FENTestRunner.hpp"

#include <iostream>
#include <vector>

#include "ChessBoard.hpp"

namespace {
struct FENTestCase {
    std::string fen;
    std::string sanMove;
    bool shouldFail;
    std::string description;
};
}

bool runFENMoveTest(const std::string& fen, const std::string& sanMove, bool shouldFail)
{
    ChessBoard board(fen);

    bool moveAccepted = board.movePieceSAN(sanMove);
    bool passed = shouldFail ? !moveAccepted : moveAccepted;

    std::cout << "FEN move test (" << (shouldFail ? "expect fail" : "expect pass")
              << "): " << sanMove << " | " << fen << " -> "
              << (passed ? "passed" : "failed") << std::endl;

    return passed;
}

bool runFENTests()
{
    const std::vector<FENTestCase> testCases = {
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "e4", false,
         "Standard opening pawn advance"},
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "Bh5", true,
         "Bishop cannot jump pawns on opening rank"},
        {"r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1", "O-O", false,
         "Castling is available when paths are clear"},
        {"r3k2r/8/8/8/8/8/4r3/R3K2R w KQkq - 0 1", "O-O", true,
         "Castling through check should be rejected"},
        {"4k3/8/8/3p4/4P3/8/8/4K3 w - - 0 1", "exd5", false,
         "Simple pawn capture"},
        {"4k3/8/8/3p4/4P3/8/8/4K3 w - - 0 1", "exd6", true,
         "Pawn cannot capture empty square"},
    };

    bool allPassed = true;
    for (const auto& testCase : testCases) {
        bool passed = runFENMoveTest(testCase.fen, testCase.sanMove, testCase.shouldFail);
        if (!passed) {
            allPassed = false;
            std::cout << "  Description: " << testCase.description << std::endl;
        }
    }

    if (allPassed) {
        std::cout << "All FEN move tests passed." << std::endl;
    } else {
        std::cout << "Some FEN move tests failed." << std::endl;
    }

    return allPassed;
}
