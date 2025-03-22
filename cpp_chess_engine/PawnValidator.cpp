#include "PawnValidator.hpp"
#include "ChessBoard.hpp"
#include <cstdint>
#include <iostream>
#include <string>
#include <array>
#include <bitset>

PawnValidator::PawnValidator() {
    initPawnMoves();
}

void PawnValidator::initPawnMoves() {
    // Precompute Pawn moves for each square, using bitboards.
	// If decided pawn look tables are desirable this will be used to generate them.
    
}

// should get rid of board functions in the future and pass directly in
bool PawnValidator::validate(int from_idx, int to_idx, const ChessBoard& board) const {
    // Retrieve the appropriate Pawn bitboard based on whose turn it is.
	bool whiteToMove = board.getTurn();
    uint64_t Pawns = whiteToMove ? board.getWhitePawnBitboard() : board.getBlackPawnBitboard();
	int rankIncrement = whiteToMove ? 8 : -8;
	uint64_t rankMask = whiteToMove ?  0x000000000000FF00 : 0x00FF000000000000;
    uint64_t backRankMask = whiteToMove ? 0xFF00000000000000 : 0x00000000000000FF;

    uint64_t allPieces = board.getAllPieces();
    uint64_t target_bb = 1ULL << to_idx;
    uint64_t from_bb = 1ULL << from_idx;
    uint64_t active_p_bb = Pawns & from_bb;

    // Check that there is a Pawn at the starting square.
    if (!(Pawns & (1ULL << from_idx))) {
        return false;
    }

    // focusing only on the case where the pawn is moving forward
    bool is_two_step_move = (((rankIncrement * 2) + from_idx) == to_idx); // pawn matches two step index
    is_two_step_move = is_two_step_move && ((rankMask & from_bb) != 0); // confirms pawn is on right rank
    bool is_one_step_move = ((rankIncrement + from_idx) == to_idx); // pawn matches one step index
    if (!is_two_step_move && !is_one_step_move) return false;

    // Use the precomputed moves to see if the destination is legal.

    if (whiteToMove) {

        active_p_bb = (active_p_bb << rankIncrement) & ~allPieces;

        if (active_p_bb & target_bb) return true;

        active_p_bb = (active_p_bb << rankIncrement) & ~allPieces;

        if (active_p_bb & target_bb) return true;
    }
    else {
        active_p_bb = (rankIncrement >> active_p_bb) & ~allPieces;

        if (active_p_bb & target_bb) return true;

        active_p_bb = (rankIncrement >> active_p_bb) & ~allPieces;
        if (active_p_bb & target_bb) return true;
    }

        return false;
    }

