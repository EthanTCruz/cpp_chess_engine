#ifndef FEN_MOVE_TESTER_HPP
#define FEN_MOVE_TESTER_HPP

#include <string>
#include <vector>

struct FenMoveTestCase {
    std::string fen;
    std::string sanMove;
    bool shouldSucceed;
    std::string description;
};

// Runs a single FEN + SAN move test using ChessBoard. Returns true if the
// observed result matches shouldSucceed.
bool runFenMoveTest(const FenMoveTestCase& testCase);

// Runs every test case in order and prints a summary.
bool runFenMoveTests(const std::vector<FenMoveTestCase>& testCases);

// Returns the built-in regression suite for move validation.
std::vector<FenMoveTestCase> createDefaultFenMoveTests();

#endif // FEN_MOVE_TESTER_HPP
