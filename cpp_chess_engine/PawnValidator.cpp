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



Bitboard PawnValidator::getAttacks(Bitboard origin, const ChessBoard& board) const {
    // Retrieve the appropriate Pawn bitboard based on whose turn it is.
    bool whiteToMove = board.getTurn();
    Bitboard Pawns = whiteToMove ? board.getWhitePawnBitboard() : board.getBlackPawnBitboard();
    int rankIncrement = whiteToMove ? 8 : -8;

    Bitboard rankMask = whiteToMove ? 0x000000000000FF00 : 0x00FF000000000000;
    Bitboard backRankMask = whiteToMove ? 0xFF00000000000000 : 0x00000000000000FF;
    const Bitboard A_FILE = 0x0101010101010101ULL;
    const Bitboard H_FILE = 0x8080808080808080ULL;

    Bitboard allPieces = board.getAllPieces();
    
    Bitboard active_p_bb = Pawns & origin;
    Bitboard a_file_p_bb = active_p_bb & ~A_FILE;
    Bitboard h_file_p_bb = active_p_bb & ~H_FILE;
    Bitboard enemy_pieces = board.getEnemyPieces();
    Bitboard attack_p_bb = whiteToMove ? h_file_p_bb << 9 | a_file_p_bb << 7 : a_file_p_bb >> 9 | h_file_p_bb >> 7;
    Bitboard enPassant = board.getEnPassant();

    Bitboard legalMoves = 0ULL;



    // Check that there is a Pawn at the starting square.
    if (!(Pawns & origin)) {
        return false;
    }

    // getting attacks
    legalMoves |= (attack_p_bb & enemy_pieces) | (attack_p_bb & enPassant);

    // detecting possible one step moves
    Bitboard oneStepMove = (whiteToMove) ? (origin << (8)) : (origin >> (8));
    // detecting possible two step moves
    Bitboard twoStepMove = (whiteToMove) ? (oneStepMove << (8)) : (oneStepMove >> (8));
    twoStepMove &= ((rankMask & origin) != 0) ? ~allPieces : 0;
    legalMoves |= oneStepMove | twoStepMove;



    return legalMoves;
}

bool PawnValidator::validate(int from_idx, int to_idx, const ChessBoard& board) const {
    return false;
}

bool PawnValidator::validate(Bitboard origin, Bitboard target, const ChessBoard& board) const {
    Bitboard attacks = getAttacks(origin, board);

    return (attacks & target) != 0;
}