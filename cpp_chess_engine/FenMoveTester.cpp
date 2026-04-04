#include "FenMoveTester.hpp"

#include <iostream>

#include "ChessBoard.hpp"

bool runFenMoveTest(const FenMoveTestCase& testCase)
{
    ChessBoard board(testCase.fen);
    const bool moveAccepted = board.movePieceSAN(testCase.sanMove);
    const bool passed = (moveAccepted == testCase.shouldSucceed);

    std::cout << "[FEN] " << (passed ? "PASS" : "FAIL") << " | "
              << (testCase.shouldSucceed ? "expect success" : "expect failure")
              << " | move=" << testCase.sanMove << " | " << testCase.description
              << std::endl;

    if (!passed) {
        std::cout << "      FEN: " << testCase.fen << std::endl;
    }

    return passed;
}

bool runFenMoveTests(const std::vector<FenMoveTestCase>& testCases)
{
    if (testCases.empty()) {
        std::cout << "[FEN] No test cases provided." << std::endl;
        return true;
    }

    bool allPassed = true;
    std::size_t passedCount = 0;

    for (const FenMoveTestCase& testCase : testCases) {
        if (runFenMoveTest(testCase)) {
            ++passedCount;
        } else {
            allPassed = false;
        }
    }

    std::cout << "[FEN] Summary: " << passedCount << "/" << testCases.size()
              << " passed." << std::endl;

    return allPassed;
}

std::vector<FenMoveTestCase> createDefaultFenMoveTests()
{
    return {
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "e4", true,
         "Standard opening pawn advance"},
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "Bh5", false,
         "Bishop cannot jump pawns from the initial position"},
        {"r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1", "O-O", true,
         "King-side castling allowed with clear path"},
        {"r3k2r/8/8/8/8/8/4r3/R3K2R w KQkq - 0 1", "O-O", false,
         "Castling through check is illegal"},
        {"4k3/8/8/3p4/4P3/8/8/4K3 w - - 0 1", "exd5", true,
         "White pawn captures diagonally"},
        {"4k3/8/8/3p4/4P3/8/8/4K3 w - - 0 1", "exd6", false,
         "Pawn cannot capture an empty target square"},
    };
}
