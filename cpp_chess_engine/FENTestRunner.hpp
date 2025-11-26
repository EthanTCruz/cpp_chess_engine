#ifndef FEN_TEST_RUNNER_HPP
#define FEN_TEST_RUNNER_HPP

#include <string>

// Validate a single SAN move against a provided FEN position. When shouldFail is true,
// the function expects the move to be rejected by the validator.
bool runFENMoveTest(const std::string& fen, const std::string& sanMove, bool shouldFail);

// Execute the built-in suite of FEN/move validation tests.
bool runFENTests();

#endif // FEN_TEST_RUNNER_HPP
