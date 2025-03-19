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
std::unordered_map<char, int> piece_to_idx = {
    {'P', w_pawn_idx},
    {'N', w_knight_idx},
    {'B', w_bishop_idx},
    {'R', w_rook_idx},
    {'Q', w_queen_idx},
    {'K', w_king_idx},
    {'p', b_pawn_idx},
    {'n', b_knight_idx},
    {'b', b_bishop_idx},
    {'r', b_rook_idx},
    {'q', b_queen_idx},
    {'k', b_king_idx}
};


ChessBoard::ChessBoard(const std::string& str) : fen(str) {
    initialize();
}


const uint64_t* ChessBoard::data() const { return bitboards.data(); }
uint64_t* ChessBoard::data() { return bitboards.data(); }

bool ChessBoard::getTurn() {
	return whiteToMove;
}
void ChessBoard::changeTurn() {
    whiteToMove = !whiteToMove;
}


void ChessBoard::parseFEN() {
    int piece_idx;
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
                piece_idx = get_bitindex(row,col);

				bitboards[piece_to_idx[ch]] |= 1ULL << piece_idx;


            }
            col++;
        }
    }
}

int ChessBoard::get_bitindex(int row, int col) {
    return (7 - row) * 8 + col;
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

    int from_idx = get_bitindex(from.y, from.x);
    int to_idx = get_bitindex(newRow, newCol);

	uint64_t whitePieces = getWhitePieces();
	uint64_t blackPieces = getBlackPieces();
    (whitePieces >> from_idx) & 1ULL;

	uint64_t from_bb = 1ULL << from_idx;
	uint64_t to_bb = 1ULL << to_idx;
	
    /*std::cout << "================from bb=================" << "\n" << std::bitset<64>(from_bb) << "\n" << std::bitset<64>(to_bb) << "\n" ;*/
    from_bb &= (whiteToMove) ? whitePieces : blackPieces;
    to_bb &= (whiteToMove) ? ~whitePieces : ~blackPieces;

    board[from.y][from.x] = '.';
    board[newRow][newCol] = piece;



    if ((from_bb * to_bb) > 0) {



        // Clear the bit at the original position
        bitboards[piece_to_idx[piece]] &= ~(1ULL << from_idx);

        // Set the bit at the new position
        bitboards[piece_to_idx[piece]] |= 1ULL << to_idx;
        std::cout << "preturn chage: " << whiteToMove << "\n";
        changeTurn();
        std::cout << "postturn chage: " << whiteToMove << "\n";
	}
	else {
		std::cout << "Invalid move\n";
        
	}
    std::cout << "from & to bb: " << "\n" << std::bitset<64>(from_bb) << "\n"  << std::bitset<64>(to_bb) << "\n" << from_bb <<"\n" << to_bb << "\n" << whiteToMove << "\n" << (!whiteToMove) << "\n" << "=========================" << "\n";
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

uint64_t ChessBoard::getWhitePieces() const {
    return bitboards[w_pawn_idx] | bitboards[w_knight_idx] | bitboards[w_bishop_idx] |
        bitboards[w_rook_idx] | bitboards[w_queen_idx] | bitboards[w_king_idx];
}

uint64_t ChessBoard::getBlackPieces() const {
    return bitboards[b_pawn_idx] | bitboards[b_knight_idx] | bitboards[b_bishop_idx] |
        bitboards[b_rook_idx] | bitboards[b_queen_idx] | bitboards[b_king_idx];
}