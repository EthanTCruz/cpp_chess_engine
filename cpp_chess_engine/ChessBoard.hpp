#ifndef CHESSBOARD_HPP
#define CHESSBOARD_HPP

#include <string>
#include <array>
#include <iostream>
#include <cctype>
#include <bitset>
#include <SFML/Graphics.hpp>

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

class ChessBoard {
private:
    char board[8][8];
    std::string fen;
    std::array<uint64_t, 12> bitboards;

    void parseFEN();

public:
    ChessBoard(const std::string& str);
    const uint64_t* data() const;
    uint64_t* data();

    uint64_t& white_pawn();
    const uint64_t& white_pawn() const;

    uint64_t& white_knight();
    const uint64_t& white_knight() const;

    uint64_t& white_bishop();
    const uint64_t& white_bishop() const;

    uint64_t& white_rook();
    const uint64_t& white_rook() const;

    uint64_t& white_queen();
    const uint64_t& white_queen() const;

    uint64_t& white_king();
    const uint64_t& white_king() const;

    uint64_t& black_pawn();
    const uint64_t& black_pawn() const;

    uint64_t& black_knight();
    const uint64_t& black_knight() const;

    uint64_t& black_bishop();
    const uint64_t& black_bishop() const;

    uint64_t& black_rook();
    const uint64_t& black_rook() const;

    uint64_t& black_queen();
    const uint64_t& black_queen() const;

    uint64_t& black_king();
    const uint64_t& black_king() const;

    void initialize();
    void setString(const std::string& newFen);
    void movePiece(const sf::Vector2i& from, const int& newRow, const int& newCol);
    const char (*getBoard() const)[8];
    void printBoard() const;
};

#endif // CHESSBOARD_HPP