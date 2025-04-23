#include "KingValidator.hpp"
#include "ChessBoard.hpp"
#include <cstdint>

#include "Constants.hpp"




const Bitboard A1 = Files::A | Rank::RANK_1;
const Bitboard A8 = Files::A | Rank::RANK_8;
const Bitboard H1 = Files::H | Rank::RANK_1;
const Bitboard H8 = Files::H | Rank::RANK_8;




KingValidator::KingValidator() {
    initKingMoves();
}

void KingValidator::initKingMoves() {

    // Precompute king moves for each square using bitboards.
    for (int sq = 0; sq < 64; ++sq) {
        Bitboard king = 1ULL << sq;
        Bitboard moves = 0ULL;
        // Use appropriate edge masks to avoid wrap-around moves.
        
        //start up and rotate clockwise
        if ((king << 8) & ~Rank::RANK_1) moves |= (king << 8);
        
        if (king  & ~Files::A) moves |= (king << 7);
        if (king & ~Files::A) moves |= (king >> 1);
        if (king & ~Files::A) moves |= (king >> 9);

        if (king & ~Rank::RANK_8) moves |= (king >> 8);
        
        if (king & ~Files::H) moves |= (king >> 7);
        if (king  & ~Files::H) moves |= (king << 1);
        if (king & ~Files::H) moves |= (king << 9);

        


        kingMoves[sq] = moves;
    }
}

Bitboard KingValidator::getAttacks(int square, const ChessBoard& board) const {
    // Retrieve the king bitboard based on whose turn it is.
    bool whiteToMove = board.getTurn();
    Bitboard kings = whiteToMove ? board.getWhiteKingBitboard() : board.getBlackKingBitboard();



    // Use precomputed moves to verify the destination.
    Bitboard legalMoves = kingMoves[square];
	legalMoves &= ~board.getFriendlyPieces();

    return legalMoves;
}


Bitboard KingValidator::getAttacks(Bitboard origin, const ChessBoard& board) const {
    // Retrieve the king bitboard based on whose turn it is.
    Bitboard kings = board.getTurn() ? board.getWhiteKingBitboard() : board.getBlackKingBitboard();

    // Use precomputed moves to verify the destination.
    Bitboard legalMoves = kingMoves[bitScanForward(origin)];
    legalMoves &= ~board.getFriendlyPieces();

    return legalMoves;
}

bool KingValidator::validate(int from_idx, int to_idx, const ChessBoard& board) const {
	Bitboard legalMoves = getAttacks(from_idx, board);
	return (legalMoves & (1ULL << to_idx));
}


bool KingValidator::validate(int from_idx, Bitboard target, const ChessBoard& board) const {
    Bitboard legalMoves = getAttacks(from_idx, board);
    return (legalMoves & target);
}