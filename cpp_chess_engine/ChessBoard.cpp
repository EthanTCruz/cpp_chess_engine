#include "ChessBoard.hpp"
#include "Move.hpp"



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
	init_knight_moves();
    std::cout << "Initialization logic executed.\n";
}

void ChessBoard::setString(const std::string& newFen) {
    fen = newFen;
    initialize();
}

int bitScan(uint64_t bb) {
    unsigned long index;
    // _BitScanForward64 returns a nonzero value if a set bit was found.
    if (_BitScanForward64(&index, bb))
        return static_cast<int>(index);
    // Handle the case where bb is 0 (if needed)
	std::cout << "Bitboard is 0\n";
    return -1;
}


void printBitsetGrid(uint64_t value) {
    std::bitset<64> bits(value);

    // Print bits in an 8x8 grid
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            // Get bit at position (7-row) * 8 + col (to match chessboard layout)
            std::cout << bits[63 - (row * 8 + col)] << " ";
        }
        std::cout << std::endl;
    }
}

void ChessBoard::init_knight_moves() {
    for (int sq = 0; sq < 64; sq++) {
        uint64_t knight = 1ULL << sq;
        uint64_t moves = 0ULL;
        // Compute knight moves with proper edge masks:
        if ((knight >> 17) & ~0x8080808080808080ULL) moves |= (knight >> 17);
        if ((knight >> 15) & ~0x0101010101010101ULL) moves |= (knight >> 15);
        if ((knight >> 10) & ~0xC0C0C0C0C0C0C0C0ULL) moves |= (knight >> 10);
        if ((knight >> 6) & ~0x0303030303030303ULL) moves |= (knight >> 6);
        if ((knight << 17) & ~0x0101010101010101ULL) moves |= (knight << 17);
        if ((knight << 15) & ~0x8080808080808080ULL) moves |= (knight << 15);
        if ((knight << 10) & ~0x0303030303030303ULL) moves |= (knight << 10);
        if ((knight << 6) & ~0xC0C0C0C0C0C0C0C0ULL) moves |= (knight << 6);
        KNIGHT_MOVES[sq] = moves;
		//std::cout << "Knight moves for square: " << sq << "\n";
        //printBitsetGrid(moves);
    }
}


std::vector<Move> ChessBoard::get_knight_moves() {
    std::vector<Move> moves;
    int knight_idx = (whiteToMove) ? w_knight_idx : b_knight_idx;
    uint64_t knightPositions = bitboards[knight_idx];

    while (knightPositions) {
        uint64_t lsb = knightPositions & static_cast<int64_t>(-static_cast<int64_t>(knightPositions)); // Fix LSB extraction
        int fromSquare = bitScan(lsb);

        uint64_t moveMask = KNIGHT_MOVES[fromSquare];

        while (moveMask) {
            uint64_t destLSB = moveMask & static_cast<int64_t>(-static_cast<int64_t>(moveMask)); // Fix LSB extraction
            int toSquare = bitScan(destLSB);

            moves.push_back({ fromSquare, toSquare });

            moveMask &= moveMask - 1; // Remove the processed bit
        }

        knightPositions &= knightPositions - 1; // Remove the processed knight
    }

    return moves;
}

bool ChessBoard::validateMove(const int& from_idx, const int& to_idx) {

    
    uint64_t whitePieces = getWhitePieces();
    uint64_t blackPieces = getBlackPieces();
    (whitePieces >> from_idx) & 1ULL;

    uint64_t from_bb = 1ULL << from_idx;
    uint64_t to_bb = 1ULL << to_idx;

    
    from_bb &= (whiteToMove) ? whitePieces : blackPieces;
    to_bb &= (whiteToMove) ? ~whitePieces : ~blackPieces;

    from_bb &= (whiteToMove) ? whitePieces : blackPieces;
    to_bb &= (whiteToMove) ? ~whitePieces : ~blackPieces;
    
    bool found = false;
    std::vector<Move> knightMoves = get_knight_moves();
    for (const Move& move : knightMoves) {
        std::cout << "checking knight move from: " << move.from << " move from idx: " << from_idx << " Move to: " << move.to  << " move to idx: " << to_idx << "\n";
        
        if (move.from == from_idx) {
            if (move.to == to_idx) {
            found = true;
            std::cout << "found knight move\n";
            break;
        }
            else {
                return false;
            }
        }
    }

    return ((from_bb != 0) && (to_bb != 0));
}

void ChessBoard::movePiece(const sf::Vector2i& from, const int& newRow, const int& newCol) {

    char piece = board[from.y][from.x];

    int from_idx = get_bitindex(from.y, from.x);
    int to_idx = get_bitindex(newRow, newCol);






    if (validateMove(from_idx,to_idx)) {


        board[from.y][from.x] = '.';
        board[newRow][newCol] = piece;

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