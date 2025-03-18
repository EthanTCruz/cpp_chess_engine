#include "ChessBoard.hpp"



int w_pawn_idx = 0;
int w_knight_idx = 1;
int w_bishop_idx = 2;
int w_rook_idx = 3;
int w_queen_idx = 4;
int w_king_idx = 5;

int b_pawn_idx = 6;
int b_knight_idx = 7;
int b_bishop_idx = 8;
int b_rook_idx = 9;
int b_queen_idx = 10;
int b_king_idx = 11;

ChessBoard::ChessBoard(const std::string& str) : fen(str) {
    initialize();
}

const uint64_t* ChessBoard::data() const { return bitboards.data(); }
uint64_t* ChessBoard::data() { return bitboards.data(); }

uint64_t& ChessBoard::white_pawn() { return bitboards[w_pawn_idx]; }
const uint64_t& ChessBoard::white_pawn() const { return bitboards[w_pawn_idx]; }

uint64_t& ChessBoard::white_knight() { return bitboards[w_knight_idx]; }
const uint64_t& ChessBoard::white_knight() const { return bitboards[w_knight_idx]; }

uint64_t& ChessBoard::white_bishop() { return bitboards[w_bishop_idx]; }
const uint64_t& ChessBoard::white_bishop() const { return bitboards[w_bishop_idx]; }

uint64_t& ChessBoard::white_rook() { return bitboards[w_rook_idx]; }
const uint64_t& ChessBoard::white_rook() const { return bitboards[w_rook_idx]; }

uint64_t& ChessBoard::white_queen() { return bitboards[w_queen_idx]; }
const uint64_t& ChessBoard::white_queen() const { return bitboards[w_queen_idx]; }

uint64_t& ChessBoard::white_king() { return bitboards[w_king_idx]; }
const uint64_t& ChessBoard::white_king() const { return bitboards[w_king_idx]; }

uint64_t& ChessBoard::black_pawn() { return bitboards[b_pawn_idx]; }
const uint64_t& ChessBoard::black_pawn() const { return bitboards[b_pawn_idx]; }

uint64_t& ChessBoard::black_knight() { return bitboards[b_knight_idx]; }
const uint64_t& ChessBoard::black_knight() const { return bitboards[b_knight_idx]; }

uint64_t& ChessBoard::black_bishop() { return bitboards[b_bishop_idx]; }
const uint64_t& ChessBoard::black_bishop() const { return bitboards[b_bishop_idx]; }

uint64_t& ChessBoard::black_rook() { return bitboards[b_rook_idx]; }
const uint64_t& ChessBoard::black_rook() const { return bitboards[b_rook_idx]; }

uint64_t& ChessBoard::black_queen() { return bitboards[b_queen_idx]; }
const uint64_t& ChessBoard::black_queen() const { return bitboards[b_queen_idx]; }

uint64_t& ChessBoard::black_king() { return bitboards[b_king_idx]; }
const uint64_t& ChessBoard::black_king() const { return bitboards[b_king_idx]; }

void ChessBoard::parseFEN() {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c)
            board[r][c] = '.';
    }

    int row = 0, col = 0;
    for (char ch : fen) {
        if (ch == ' ') {
            break;
        }
        else if (ch == '/') {
            row++;
            col = 0;
        }
        else if (std::isdigit(ch)) {
            int count = ch - '0';
            for (int i = 0; i < count; ++i) {
                if (col < 8) {
                    board[row][col++] = '.';
                }
            }
        }
        else {
            if (row < 8 && col < 8) {
                board[row][col] = ch;
            }
            col++;
        }
    }
}

void ChessBoard::initialize() {
    parseFEN();
    std::cout << "Initialization logic executed.\n";
}

void ChessBoard::setString(const std::string& newFen) {
    fen = newFen;
    initialize();
}

void ChessBoard::movePiece(const sf::Vector2i& from, const int& newRow, const int& newCol) {
    char piece = board[from.y][from.x];
    board[from.y][from.x] = '.';
    board[newRow][newCol] = piece;
}

const char (*ChessBoard::getBoard() const)[8] {
    return board;
}

void ChessBoard::printBoard() const {
    std::cout << "Board state:\n";
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            char symbol = board[r][c];
            std::cout << (symbol == '.' ? '.' : symbol);
            if (c < 7) std::cout << " ";
        }
        std::cout << "\n";
    }
}