#ifndef PERFT_TESTER_HPP
#define PERFT_TESTER_HPP

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

struct PerftTestCase {
    std::string fen;
    int depth;
    std::uint64_t expectedNodes;
    std::string description;
    std::string source;
};

std::uint64_t countLegalMoveNodes(const std::string& fen, int depth);
bool runPerftTest(const PerftTestCase& testCase);
bool runPerftTests(const std::vector<PerftTestCase>& testCases);
std::vector<PerftTestCase> loadPerftTestsFromDirectory(const std::filesystem::path& directory);
bool runPerftTestsFromDirectory(const std::filesystem::path& directory = "test_perft");

#endif // PERFT_TESTER_HPP
