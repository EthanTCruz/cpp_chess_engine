#include "KnightValidator.hpp"
#include "ChessBoard.hpp"
#include <cstdint>

KnightValidator::KnightValidator() {
    initKnightMoves();
}

void KnightValidator::initKnightMoves() {
    // Precompute knight moves for each square using bitboards.
    for (int sq = 0; sq < 64; ++sq) {
        uint64_t knight = 1ULL << sq;
        uint64_t moves = 0ULL;
        // Use appropriate edge masks to avoid wrap-around moves.
        if ((knight >> 17) & ~0x8080808080808080ULL) moves |= (knight >> 17);
        if ((knight >> 15) & ~0x0101010101010101ULL) moves |= (knight >> 15);
        if ((knight >> 10) & ~0xC0C0C0C0C0C0C0C0ULL) moves |= (knight >> 10);
        if ((knight >> 6) & ~0x0303030303030303ULL) moves |= (knight >> 6);
        if ((knight << 17) & ~0x0101010101010101ULL) moves |= (knight << 17);
        if ((knight << 15) & ~0x8080808080808080ULL) moves |= (knight << 15);
        if ((knight << 10) & ~0x0303030303030303ULL) moves |= (knight << 10);
        if ((knight << 6) & ~0xC0C0C0C0C0C0C0C0ULL) moves |= (knight << 6);
        knightMoves[sq] = moves;
    }
}

bool KnightValidator::validate(int from_idx, int to_idx, const ChessBoard& board) const {
    // Retrieve the knight bitboard based on whose turn it is.
    uint64_t knights = board.getTurn() ? board.getWhiteKnightBitboard() : board.getBlackKnightBitboard();

    // Check that a knight exists at the starting square.
    if (!(knights & (1ULL << from_idx))) {
        return false;
    }

    // Use precomputed moves to verify the destination.
    uint64_t legalMoves = knightMoves[from_idx];
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
