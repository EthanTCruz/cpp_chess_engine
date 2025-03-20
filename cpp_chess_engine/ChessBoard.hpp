// ChessBoard.hpp
#ifndef CHESSBOARD_HPP
#define CHESSBOARD_HPP

#include <string>
#include <array>
#include <iostream>
#include <cctype>
#include <bitset>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <intrin.h>
#include "Move.hpp"

extern int w_pawn_idx;
extern int w_knight_idx;
extern int w_bishop_idx;
extern int w_rook_idx;
extern int w_queen_idx;
extern int w_king_idx;

extern int b_pawn_idx;
extern int b_knight_idx;
extern int b_bishop_idx;
extern int b_rook_idx;
extern int b_queen_idx;
extern int b_king_idx;
extern std::unordered_map<char, int> piece_to_idx;



class ChessBoard {
private:
    char board[8][8];
    std::string fen;
    std::array<uint64_t, 12> bitboards = {};
    uint64_t KNIGHT_MOVES[64];


    bool whiteToMove = true;

    void parseFEN();


public:
    ChessBoard(const std::string& str);
    const uint64_t* data() const;
    uint64_t* data();


    void initialize();
    bool getTurn();
	void changeTurn();
    std::vector<Move> get_knight_moves();
    bool validateMove(const int& from_idx, const int& to_idx);
    void setString(const std::string& newFen);
    void movePiece(const sf::Vector2i& from, const int& newRow, const int& newCol);
    const char (*getBoard() const)[8];
    void printBoard() const;
    int get_bitindex(int row, int col); // Ensure this declaration matches the definition
    uint64_t getWhitePieces() const;
    uint64_t getBlackPieces() const;
    void init_knight_moves();
};

#endif // CHESSBOARD_HPP