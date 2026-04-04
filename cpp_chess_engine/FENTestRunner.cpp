#include "FENTestRunner.hpp"

#include <vector>

#include "FenMoveTester.hpp"

bool runFENMoveTest(const std::string& fen, const std::string& sanMove, bool shouldFail)
{
    const FenMoveTestCase testCase{fen, sanMove, !shouldFail, "Single FEN move test", "API"};
    return runFenMoveTest(testCase);
}

bool runFENTests()
{
    return runFenMoveTestsFromDirectory("test_fens");
}
