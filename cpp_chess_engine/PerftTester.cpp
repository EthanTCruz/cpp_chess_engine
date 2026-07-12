#include "PerftTester.hpp"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#include "BitOps.hpp"
#include "ChessBoard.hpp"

namespace {
std::string trim(const std::string& value)
{
    const auto isSpace = [](unsigned char c) { return std::isspace(c) != 0; };

    std::size_t start = 0;
    while (start < value.size() && isSpace(static_cast<unsigned char>(value[start]))) {
        ++start;
    }

    std::size_t end = value.size();
    while (end > start && isSpace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }

    return value.substr(start, end - start);
}

std::vector<std::string> parseCsvLine(const std::string& line)
{
    std::vector<std::string> columns;
    std::string current;
    bool inQuotes = false;

    for (std::size_t i = 0; i < line.size(); ++i) {
        const char ch = line[i];
        if (ch == '"') {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                current.push_back('"');
                ++i;
            } else {
                inQuotes = !inQuotes;
            }
            continue;
        }

        if (ch == ',' && !inQuotes) {
            columns.push_back(trim(current));
            current.clear();
            continue;
        }

        current.push_back(ch);
    }

    columns.push_back(trim(current));
    return columns;
}

int rowFromIndex(int index)
{
    return 7 - (index / 8);
}

int colFromIndex(int index)
{
    return index % 8;
}

std::string squareName(int index)
{
    std::string square;
    square.push_back(static_cast<char>('a' + colFromIndex(index)));
    square.push_back(static_cast<char>('8' - rowFromIndex(index)));
    return square;
}

int configValue(const std::string& name, int fallback)
{
    if (const char* envValue = std::getenv(name.c_str())) {
        return std::stoi(envValue);
    }

    std::ifstream input("test_config.env");
    std::string line;
    while (std::getline(input, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        const std::size_t equals = line.find('=');
        if (equals == std::string::npos) {
            continue;
        }

        if (trim(line.substr(0, equals)) == name) {
            return std::stoi(trim(line.substr(equals + 1)));
        }
    }

    return fallback;
}

int perftMaxDepth()
{
    static const int value = configValue("PERFT_MAX_DEPTH", 2);
    return value;
}

std::uint64_t perft(ChessBoard board, int depth)
{
    if (depth <= 0) {
        return 1;
    }

    std::uint64_t nodes = 0;
    const auto moves = board.getAllMoves();

    for (const auto& [origin, destinations] : moves) {
        if (origin == 0ULL) {
            continue;
        }

        const int fromIndex = bitScanForward(origin);
        Bitboard remainingDestinations = destinations;
        while (remainingDestinations) {
            const int toIndex = bitScanForward(remainingDestinations);
            remainingDestinations &= remainingDestinations - 1;

            ChessBoard child = board;
            if (!child.movePiece(rowFromIndex(fromIndex), colFromIndex(fromIndex),
                                 rowFromIndex(toIndex), colFromIndex(toIndex))) {
                continue;
            }

            nodes += perft(child, depth - 1);
        }
    }

    return nodes;
}

void printPerftDivide(const std::string& fen, int depth)
{
    if (depth <= 0) {
        return;
    }

    ChessBoard board(fen);
    const auto moves = board.getAllMoves();
    std::cout << "        Divide:";

    for (const auto& [origin, destinations] : moves) {
        if (origin == 0ULL) {
            continue;
        }

        const int fromIndex = bitScanForward(origin);
        Bitboard remainingDestinations = destinations;
        while (remainingDestinations) {
            const int toIndex = bitScanForward(remainingDestinations);
            remainingDestinations &= remainingDestinations - 1;

            ChessBoard child = board;
            if (!child.movePiece(rowFromIndex(fromIndex), colFromIndex(fromIndex),
                                 rowFromIndex(toIndex), colFromIndex(toIndex))) {
                continue;
            }

            std::cout << ' ' << squareName(fromIndex) << squareName(toIndex)
                      << '=' << perft(child, depth - 1);
        }
    }

    std::cout << std::endl;
}
} // namespace

std::uint64_t countLegalMoveNodes(const std::string& fen, int depth)
{
    const int maxDepth = perftMaxDepth();
    if (depth > maxDepth) {
        depth = maxDepth;
    }

    ChessBoard board(fen);
    return perft(board, depth);
}

bool runPerftTest(const PerftTestCase& testCase)
{
    const auto start = std::chrono::steady_clock::now();
    const std::uint64_t actualNodes = countLegalMoveNodes(testCase.fen, testCase.depth);
    const auto end = std::chrono::steady_clock::now();
    const auto elapsedUs = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    const bool passed = actualNodes == testCase.expectedNodes;

    std::cout << "[PERFT] " << (passed ? "PASS" : "FAIL")
              << " | depth=" << testCase.depth
              << " | nodes=" << actualNodes
              << " | expected=" << testCase.expectedNodes
              << " | " << elapsedUs << " us"
              << " | " << testCase.description;

    if (!testCase.source.empty()) {
        std::cout << " | source=" << testCase.source;
    }

    std::cout << std::endl;

    if (!passed) {
        std::cout << "        FEN: " << testCase.fen << std::endl;
        printPerftDivide(testCase.fen, testCase.depth);
    }

    return passed;
}

bool runPerftTests(const std::vector<PerftTestCase>& testCases)
{
    if (testCases.empty()) {
        std::cout << "[PERFT] No test cases provided." << std::endl;
        return true;
    }

    bool allPassed = true;
    std::size_t passedCount = 0;
    const auto start = std::chrono::steady_clock::now();

    for (const PerftTestCase& testCase : testCases) {
        if (runPerftTest(testCase)) {
            ++passedCount;
        } else {
            allPassed = false;
        }
    }

    const auto end = std::chrono::steady_clock::now();
    const auto elapsedUs = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    const double averageUs = static_cast<double>(elapsedUs) / static_cast<double>(testCases.size());

    std::cout << "[PERFT] Summary: " << passedCount << "/" << testCases.size()
              << " passed." << std::endl;
    std::cout << "[PERF] PERFT total: " << elapsedUs << " us"
              << " | average: " << averageUs << " us/test" << std::endl;

    return allPassed;
}

std::vector<PerftTestCase> loadPerftTestsFromDirectory(const std::filesystem::path& directory)
{
    namespace fs = std::filesystem;
    std::vector<PerftTestCase> testCases;

    if (!fs::exists(directory)) {
        std::cerr << "[PERFT] Test directory '" << directory.string() << "' was not found." << std::endl;
        return testCases;
    }

    std::vector<fs::path> csvFiles;
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == ".csv") {
            csvFiles.push_back(entry.path());
        }
    }

    std::sort(csvFiles.begin(), csvFiles.end());

    for (const fs::path& csvFile : csvFiles) {
        std::ifstream input(csvFile);
        if (!input.is_open()) {
            std::cerr << "[PERFT] Failed to open CSV file: " << csvFile.string() << std::endl;
            continue;
        }

        std::string line;
        std::size_t lineNumber = 0;
        while (std::getline(input, line)) {
            ++lineNumber;
            const std::string trimmed = trim(line);
            if (trimmed.empty() || trimmed[0] == '#') {
                continue;
            }

            const std::vector<std::string> columns = parseCsvLine(line);
            if (lineNumber == 1 && (columns[0] == "fen" || columns[0] == "FEN")) {
                continue;
            }

            if (columns.size() < 3) {
                std::cerr << "[PERFT] Skipping malformed row in " << csvFile.filename().string()
                          << ':' << lineNumber << " (expected at least 3 columns)." << std::endl;
                continue;
            }

            const int depth = std::stoi(columns[1]);
            if (depth > perftMaxDepth()) {
                continue;
            }

            PerftTestCase testCase;
            testCase.fen = columns[0];
            testCase.depth = depth;
            testCase.expectedNodes = static_cast<std::uint64_t>(std::stoull(columns[2]));
            testCase.description = (columns.size() >= 4) ? columns[3] : "";
            testCase.source = csvFile.filename().string() + ":" + std::to_string(lineNumber);
            testCases.push_back(std::move(testCase));
        }
    }

    return testCases;
}

bool runPerftTestsFromDirectory(const std::filesystem::path& directory)
{
    const std::vector<PerftTestCase> testCases = loadPerftTestsFromDirectory(directory);

    if (testCases.empty()) {
        std::cerr << "[PERFT] No CSV test cases were loaded from '" << directory.string() << "'." << std::endl;
        return false;
    }

    return runPerftTests(testCases);
}
