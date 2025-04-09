#include "KnightValidator.hpp"
#include "ChessBoard.hpp"
#include <cstdint>

KnightValidator::KnightValidator() {
    initKnightMoves();
}

void KnightValidator::initKnightMoves() {
    // Precompute knight moves for each square using bitboards.
    for (int sq = 0; sq < 64; ++sq) {
        Bitboard knight = 1ULL << sq;
        Bitboard moves = 0ULL;
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

Bitboard KnightValidator::getAttacks(int square, const ChessBoard& board) const {

    // Use precomputed moves to verify the destination.
    Bitboard legalMoves = knightMoves[square];

    // Ensure the destination square is not occupied by a friendly piece.
    Bitboard friendlyPieces = board.getFriendlyPieces();

    return (legalMoves & ~friendlyPieces);

}


bool KnightValidator::validate(int from_idx, int to_idx, const ChessBoard& board) const {
    // Retrieve the knight bitboard based on whose turn it is.
    Bitboard knights = board.getTurn() ? board.getWhiteKnightBitboard() : board.getBlackKnightBitboard();



    Bitboard legalMoves  = getAttacks(from_idx, board);
    Bitboard destination = 1ULL << to_idx;

    if (!(legalMoves & destination)) {
        return false;
    }

    return true;
}

bool KnightValidator::validate(int from_idx, Bitboard target, const ChessBoard& board) const {
    Bitboard legalMoves = getAttacks(from_idx, board);


    if (!(legalMoves & target)) {
        return false;
    }

    return true;
}