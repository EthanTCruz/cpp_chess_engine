#ifndef PGN_TEST_RUNNER_HPP
#define PGN_TEST_RUNNER_HPP

#include <filesystem>

// Validate every PGN file inside the provided directory. The function prints a
// pass/fail line per file and a summary before returning the aggregate result.
bool runPGNTests(const std::filesystem::path& directory = "test_pgns");

#endif // PGN_TEST_RUNNER_HPP
