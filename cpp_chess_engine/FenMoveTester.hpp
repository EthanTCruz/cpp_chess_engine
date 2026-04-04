#ifndef FEN_MOVE_TESTER_HPP
#define FEN_MOVE_TESTER_HPP

#include <filesystem>
#include <string>
#include <vector>

struct FenMoveTestCase {
    std::string fen;
    std::string sanMove;
    bool shouldSucceed;
    std::string description;
    std::string source;
};

// Runs a single FEN + SAN move test using ChessBoard. Returns true if the
// observed result matches shouldSucceed.
bool runFenMoveTest(const FenMoveTestCase& testCase);

// Runs every test case in order and prints a summary.
bool runFenMoveTests(const std::vector<FenMoveTestCase>& testCases);

// Loads every .csv file from the directory and parses FEN test cases.
// CSV format:
//   fen,san_move,should_succeed,description
// Lines beginning with '#' are ignored.
std::vector<FenMoveTestCase> loadFenMoveTestsFromDirectory(const std::filesystem::path& directory);

// Convenience helper to load and run all CSV-based FEN move tests.
bool runFenMoveTestsFromDirectory(const std::filesystem::path& directory = "test_fens");

#endif // FEN_MOVE_TESTER_HPP
