#include <iostream>
#include <string>
#include <filesystem>

// ANSI escape codes for colors.
#define RESET "\033[0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"

// Unicode characters for check and cross marks.
#define CHECK_MARK "\u2714"
#define CROSS_MARK "\u2718"

// Helper function to print test results.
void checkTest(bool condition, const std::string& testName) {
    if (condition) {
        std::cout << GREEN << CHECK_MARK << " " << testName << RESET << std::endl;
    }
    else {
        std::cout << RED << CROSS_MARK << " " << testName << RESET << std::endl;
    }
}

#include "ChessBoard.hpp"
#include "GUIBoard.hpp"
// Include any other necessary headers, e.g. "GetBitIndex.cpp"

void tests() {
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    ChessBoard cb(fen);


    // Optional: print board state.
    cb.printBoard();

    int fromRow;
    int fromCol;
    int newRow;
    int newCol;

    // Test valid knight move: b1 (row=7, col=1) to c3 (row=5, col=2).
    fromRow = 7;
    fromCol = 1;
    newRow = 5;
    newCol = 2;
    //bool validKnightMove = cb.validateMove(knight_from, knight_to);
    bool validWhiteKnightMove = cb.movePiece(fromRow, fromCol, newRow, newCol);
    std::cout << cb.getString() << std::endl;
    checkTest(validWhiteKnightMove, "Knight move from b1 to c3");

    // Test valid knight move: b1 (row=7, col=1) to c3 (row=5, col=2).
    fromRow = 0;
    fromCol = 1;
    newRow = 2;
    newCol = 2;
    //bool validKnightMove = cb.validateMove(knight_from, knight_to);
    bool validBlackKnightMove = cb.movePiece(fromRow, fromCol, newRow, newCol);
    std::cout << cb.getString() << std::endl;
    checkTest(validBlackKnightMove, "Knight move from b1 to c3");


    // Test an invalid knight move: b1 to b3 (not an L-shaped move).
    fromRow = 7;
    fromCol = 1;
    newRow = 6;
    newCol = 1;
    bool invalidKnightMove = cb.movePiece(fromRow, fromCol, newRow, newCol);
    std::cout << cb.getString() << std::endl;
    checkTest(!invalidKnightMove, "Invalid knight move from b1 to b3");

    // Test valid pawn move: e2 (row=6, col=4) to e3 (row=5, col=4).
    fromRow = 6;
    fromCol = 4;
    newRow = 5;
    newCol = 4;
    bool validPawnMove = cb.movePiece(fromRow, fromCol, newRow, newCol);
    std::cout << cb.getString() << std::endl;
    checkTest(validPawnMove, "Pawn move from e2 to e3");


    fen = "rn1qk2r/pppPpppp/6bn/2b5/8/8/PPPP1PPP/RNBQKBNR w KQkq - 0 5";
	cb.setString(fen);
    cb.movePieceUCI("a2a4");
    cb.movePieceUCI("e8d8");
    cb.movePieceUCI("e7d8q");
    
    std::cout << "All tests executed." << std::endl;
}

// Run PGN validation on all sample PGN files before launching the GUI.
void runPGNTests() {
    namespace fs = std::filesystem;
    bool allPassed = true;
    for (const auto& entry : fs::directory_iterator("test_pgns")) {
        if (entry.path().extension() == ".pgn") {
            ChessBoard validator;
            bool ok = validator.validatePGN(entry.path().string());
            std::cout << entry.path().filename().string() << ": "
                      << (ok ? "passed" : "failed") << std::endl;
            if (!ok) {
                allPassed = false;
            }
        }
    }
    if (allPassed) {
        std::cout << "All PGN tests passed." << std::endl;
    } else {
        std::cout << "Some PGN tests failed." << std::endl;
    }
}

int main(int argc, char** argv) {
    // Run PGN tests before showing the GUI board.
    runPGNTests();

    // Initialize the FEN string for the standard starting position.
    //tests();

    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0";
    //fen = "rn1qk2r/pppPpppp/6bn/2b5/8/8/PPPP1PPP/RNBQKBNR w KQkq - 0 5";
    fen = "4r1k1/1p4pp/6r1/3pPq2/p2RpB2/P3Q2P/1P3PP1/6K1 b - - 4 58";
    
    ChessBoard cb(fen);
    // cb.movePieceSAN("Rxd5");

    //cb.movePieceUCI("e2e4");
    //cb.movePieceUCI("d7d5");
    //cb.movePieceUCI("e4d5");
    //cb.movePieceUCI("e7e5");
    //cb.movePieceUCI("d5d6");
    //cb.movePieceUCI("c8f5");
    //cb.movePieceUCI("d6c7");
    //cb.movePieceUCI("f8c5");
    //cb.movePieceUCI("c7b8q");
    //cb.movePieceUCI("d8b8");


    //cb.movePieceUCI("a2a4");

    //cb.movePieceUCI("c5b4");
    //cb.movePieceUCI("d1g4");
    //cb.movePieceUCI("b4a5");
    //cb.movePieceUCI("d2d4");
    //cb.movePieceUCI("a5b4");
    //cb.movePieceUCI("b1c3");
    //cb.movePieceUCI("b4a5");
    //cb.movePieceUCI("c1e3");
    //cb.movePieceUCI("a5b4");

    // cb.movePieceUCI("e2e4");
    // cb.movePieceUCI("e7e5");
    // cb.movePieceUCI("g1f3");
    // cb.movePieceUCI("d7d5");
    // cb.movePieceUCI("d2d3");
    // cb.movePieceUCI("c8g4");
    // cb.movePieceUCI("b8c6");
    // cb.movePieceUCI("b1c3");
    // cb.movePieceUCI("f8b4");
    // cb.movePieceUCI("h2h3");
    // cb.movePieceUCI("g4h3");
    // cb.movePieceUCI("h1h3");
    // cb.movePieceUCI("b4a5");
    // cb.movePieceUCI("h3h6");
    // cb.movePieceUCI("a5b4");
    // cb.movePieceUCI("h6e6");
    // cb.movePieceUCI("d8e7");
    // cb.movePieceUCI("e6e7");
    // cb.movePieceUCI("e8f8");
    // cb.movePieceUCI("e7d7");
    // cb.movePieceUCI("a7a6");
    // cb.movePieceUCI("c1d2");
    // cb.movePieceUCI("a6a5");
    // cb.movePieceUCI("c3d5");
    //cb.movePieceUCI("");
    //cb.movePieceUCI("");
    //cb.movePieceUCI("");
    //cb.movePieceUCI("");
    //cb.movePieceUCI("");
    //cb.movePieceUCI("");
    //cb.movePieceUCI("");


    // Proceed with GUI initialization if needed.
    GUIBoard gb(cb);
    gb.createSFMLWindow();

    return 0;
}
