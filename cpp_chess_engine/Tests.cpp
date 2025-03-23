// tests.cpp

#include <iostream>
#include <string>
#include "ChessBoard.hpp"
// Include other headers if needed (e.g., "GUIBoard.hpp", etc.)

// ANSI escape codes for colored output.
#define RESET "\033[0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"

// Unicode symbols for check and cross marks.
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

int main() {
    // Use a specific FEN for testing.
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    ChessBoard cb(fen);

    // Optionally, print the board for debugging.
    cb.printBoard();

    // Test valid knight move: b1 (row=7, col=1) to c3 (row=5, col=2).
    int knight_from = cb.get_bitindex(7, 1); // b1
    int knight_to = cb.get_bitindex(5, 2); // c3
    bool validKnightMove = cb.validateMove(knight_from, knight_to);
    checkTest(validKnightMove, "Knight move from b1 to c3");

    // Test valid pawn move: e2 (row=6, col=4) to e3 (row=5, col=4).
    int pawn_from = cb.get_bitindex(6, 4); // e2
    int pawn_to = cb.get_bitindex(5, 4); // e3
    bool validPawnMove = cb.validateMove(pawn_from, pawn_to);
    checkTest(validPawnMove, "Pawn move from e2 to e3");

    // Test an invalid knight move: b1 to b3 (not an L-shaped move).
    knight_to = cb.get_bitindex(6, 1); // b3 (invalid for knight)
    bool invalidKnightMove = cb.validateMove(knight_from, knight_to);
    checkTest(!invalidKnightMove, "Invalid knight move from b1 to b3");

    //std::cout << "All tests executed." << std::endl;

    return 0;
}
