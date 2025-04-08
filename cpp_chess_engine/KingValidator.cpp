#include "KingValidator.hpp"
#include "ChessBoard.hpp"
#include <cstdint>

const uint64_t A_FILE = 0x0101010101010101ULL;
const uint64_t B_FILE = 0x0202020202020202ULL;
const uint64_t C_FILE = 0x0404040404040404ULL;
const uint64_t D_FILE = 0x0808080808080808ULL;
const uint64_t E_FILE = 0x1010101010101010ULL;
const uint64_t F_FILE = 0x2020202020202020ULL;
const uint64_t G_FILE = 0x4040404040404040ULL;
const uint64_t H_FILE = 0x8080808080808080ULL;

const uint64_t RANK_1 = 0x00000000000000FFULL;
const uint64_t RANK_2 = 0x000000000000FF00ULL;
const uint64_t RANK_3 = 0x0000000000FF0000ULL;
const uint64_t RANK_4 = 0x00000000FF000000ULL;
const uint64_t RANK_5 = 0x000000FF00000000ULL;
const uint64_t RANK_6 = 0x0000FF0000000000ULL;
const uint64_t RANK_7 = 0x00FF000000000000ULL;
const uint64_t RANK_8 = 0xFF00000000000000ULL;


const uint64_t A1 = A_FILE | RANK_1;
const uint64_t A8 = A_FILE | RANK_8;
const uint64_t H1 = H_FILE | RANK_1;
const uint64_t H8 = H_FILE | RANK_8;

void printBitboard(uint64_t bitboard) {
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int index = rank * 8 + file;
            if (bitboard & (1ULL << index)) {
                std::cout << "1 ";
            }
            else {
                std::cout << "0 ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void printBitset(const std::bitset<64>& bitset) {
    for (int i = 63; i >= 0; --i) {
        std::cout << bitset[i] << " ";
    }
    std::cout << std::endl;
}


KingValidator::KingValidator() {
    initKingMoves();
}

void KingValidator::initKingMoves() {

    // Precompute king moves for each square using bitboards.
    for (int sq = 0; sq < 64; ++sq) {
        uint64_t king = 1ULL << sq;
        uint64_t moves = 0ULL;
        // Use appropriate edge masks to avoid wrap-around moves.
        
        //start up and rotate clockwise
        if ((king << 8) & ~RANK_1) moves |= (king << 8);
        
        if (king  & ~A_FILE) moves |= (king << 7);
        if (king & ~A_FILE) moves |= (king >> 1);
        if (king & ~A_FILE) moves |= (king >> 9);

        if (king & ~RANK_8) moves |= (king >> 8);
        
        if (king & ~H_FILE) moves |= (king >> 7);
        if (king  & ~H_FILE) moves |= (king << 1);
        if (king & ~H_FILE) moves |= (king << 9);




        kingMoves[sq] = moves;
    }
}

bool KingValidator::validate(int from_idx, int to_idx, const ChessBoard& board) const {
    // Retrieve the king bitboard based on whose turn it is.
    uint64_t kings = board.getTurn() ? board.getWhiteKingBitboard() : board.getBlackKingBitboard();

    // Check that a king exists at the starting square.
    if (!(kings & (1ULL << from_idx))) {
        return false;
    }

    // Use precomputed moves to verify the destination.
    uint64_t legalMoves = kingMoves[from_idx];
    uint64_t destination = 1ULL << to_idx;

    if (!(legalMoves & destination)) {
        return false;
    }

    // Ensure the destination square is not occupied by a friendly piece.
    uint64_t friendlyPieces = board.getTurn() ? board.getWhitePieces() : board.getBlackPieces();
    if (friendlyPieces & destination) {
        return false;
    }
    return true;
}
