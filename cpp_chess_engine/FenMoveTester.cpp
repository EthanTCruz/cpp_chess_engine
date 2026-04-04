#include "FenMoveTester.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>

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

bool parseExpectedResult(const std::string& value, bool& result)
{
    std::string normalized;
    normalized.reserve(value.size());
    std::transform(value.begin(), value.end(), std::back_inserter(normalized), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });

    if (normalized == "1" || normalized == "true" || normalized == "pass" || normalized == "success") {
        result = true;
        return true;
    }

    if (normalized == "0" || normalized == "false" || normalized == "fail" || normalized == "failure") {
        result = false;
        return true;
    }

    return false;
}
} // namespace

bool runFenMoveTest(const FenMoveTestCase& testCase)
{
    ChessBoard board(testCase.fen);
    const bool moveAccepted = board.movePieceSAN(testCase.sanMove);
    const bool passed = (moveAccepted == testCase.shouldSucceed);

    std::cout << "[FEN] " << (passed ? "PASS" : "FAIL") << " | "
              << (testCase.shouldSucceed ? "expect success" : "expect failure")
              << " | move=" << testCase.sanMove << " | " << testCase.description;

    if (!testCase.source.empty()) {
        std::cout << " | source=" << testCase.source;
    }

    std::cout << std::endl;

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

std::vector<FenMoveTestCase> loadFenMoveTestsFromDirectory(const std::filesystem::path& directory)
{
    namespace fs = std::filesystem;
    std::vector<FenMoveTestCase> testCases;

    if (!fs::exists(directory)) {
        std::cerr << "[FEN] Test directory '" << directory.string() << "' was not found." << std::endl;
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
            std::cerr << "[FEN] Failed to open CSV file: " << csvFile.string() << std::endl;
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
            if (columns.size() < 3) {
                std::cerr << "[FEN] Skipping malformed row in " << csvFile.filename().string()
                          << ':' << lineNumber << " (expected at least 3 columns)." << std::endl;
                continue;
            }

            if (lineNumber == 1 && (columns[0] == "fen" || columns[0] == "FEN")) {
                continue;
            }

            bool shouldSucceed = false;
            if (!parseExpectedResult(columns[2], shouldSucceed)) {
                std::cerr << "[FEN] Skipping row with invalid result in " << csvFile.filename().string()
                          << ':' << lineNumber << "." << std::endl;
                continue;
            }

            FenMoveTestCase testCase;
            testCase.fen = columns[0];
            testCase.sanMove = columns[1];
            testCase.shouldSucceed = shouldSucceed;
            testCase.description = (columns.size() >= 4) ? columns[3] : "";
            testCase.source = csvFile.filename().string() + ":" + std::to_string(lineNumber);

            testCases.push_back(std::move(testCase));
        }
    }

    return testCases;
}

bool runFenMoveTestsFromDirectory(const std::filesystem::path& directory)
{
    const std::vector<FenMoveTestCase> testCases = loadFenMoveTestsFromDirectory(directory);

    if (testCases.empty()) {
        std::cerr << "[FEN] No CSV test cases were loaded from '" << directory.string() << "'." << std::endl;
        return false;
    }

    return runFenMoveTests(testCases);
}
