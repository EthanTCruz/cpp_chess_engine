#ifndef CHESSBOARD_HPP
#define CHESSBOARD_HPP

#include <string>
#include <cstdint>
#include <array>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cctype>
#include <bitset>

// The ChessBoard class manages the board state and delegates move validation.
class ChessBoard {
public:
    ChessBoard(const std::string& fen);
    std::string getString() const;
    void setString(const std::string& newFen);
    void printBoard() const;
    const char (*getBoard() const)[8];
    // Validate a move from 'from_idx' to 'to_idx'
    bool validateMove(const int& from_idx, const int& to_idx) ;
    // Move a piece on the board
    void movePiece(const sf::Vector2i& from, const int& newRow, const int& newCol);
    bool movePiece(const int& fromRow, const int& fromCol, const int& newRow, const int& newCol);

    // Bitboard getters for friendly pieces.
    uint64_t getWhitePieces() const;
    uint64_t getBlackPieces() const;
    bool getTurn() const;

    // Bitboard getters for knights.
    uint64_t getWhiteKnightBitboard() const;
    uint64_t getBlackKnightBitboard() const;

    uint64_t getWhitePawnBitboard() const;
    uint64_t getBlackPawnBitboard() const;

    uint64_t getFriendlyPieces() const;
    uint64_t getEnemyPieces() const;
    uint64_t getAllPieces() const;


    // Public helper to convert board coordinates to a bit index.
    int get_bitindex(int row, int col) ;

private:
    std::string fen;
    bool whiteToMove = true;
	int halfmoveClock = 0;
	int fullmoveNumber = 1;
    std::string castlingRights;
    char board[8][8];
    std::array<uint64_t, 12> bitboards = { 0ULL };
    std::unordered_map<char, int> piece_to_idx;

    void initialize();
    void parseFEN();

};

#endif // CHESSBOARD_HPP


