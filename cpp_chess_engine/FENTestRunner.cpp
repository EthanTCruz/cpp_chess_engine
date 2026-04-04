#include "FENTestRunner.hpp"

#include "FenMoveTester.hpp"

bool runFENMoveTest(const std::string& fen, const std::string& sanMove, bool shouldFail)
{
    const FenMoveTestCase testCase{fen, sanMove, !shouldFail, "Single FEN move test"};
    return runFenMoveTest(testCase);
}

bool runFENTests()
{
    return runFenMoveTests(createDefaultFenMoveTests());
}
