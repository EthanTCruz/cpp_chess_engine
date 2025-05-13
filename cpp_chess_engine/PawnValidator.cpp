#include "PawnValidator.hpp"
#include "ChessBoard.hpp"
#include "Constants.hpp"
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

bool PawnValidator::validate(int from_idx, int to_idx, const ChessBoard& board) const {
    std::cout << "===============================";
    return false;
}

Bitboard PawnValidator::getAttacks(Bitboard origin, const ChessBoard& board) const {
    // Retrieve the appropriate Pawn bitboard based on whose turn it is.
    bool whiteToMove = board.getTurn();
    Bitboard Pawns = whiteToMove ? board.getWhitePawnBitboard() : board.getBlackPawnBitboard();
    
    

    
    Bitboard active_p_bb =  Pawns;
    Bitboard a_file_p_bb = origin & ~Files::A;
    Bitboard h_file_p_bb = origin & ~Files::H;
    
    Bitboard attack_p_bb = whiteToMove ? h_file_p_bb << 9 | a_file_p_bb << 7 : a_file_p_bb >> 9 | h_file_p_bb >> 7;
    

    Bitboard legalMoves = 0ULL;





    // getting attacks
    legalMoves |= (attack_p_bb);




    return legalMoves;
}

Bitboard PawnValidator::getMoves(Bitboard origin, const ChessBoard& board) const {
    // Retrieve the appropriate Pawn bitboard based on whose turn it is.
    
    bool whiteToMove = board.getTurn();

    Bitboard rankMask = whiteToMove ? 0x000000000000FF00 : 0x00FF000000000000;


    Bitboard allPieces = board.getAllPieces();

    
    Bitboard attacks = getAttacks(origin, board);
    

    attacks &= board.getEnemyPieces() | board.getEnPassant();

    Bitboard legalMoves = 0ULL | attacks;



    // detecting possible one step moves
    Bitboard oneStepMove = (whiteToMove) ? (origin << (8)) : (origin >> (8));
    // detecting possible two step moves
    Bitboard twoStepMove = (whiteToMove) ? (oneStepMove << (8)) : (oneStepMove >> (8));
    twoStepMove &= ((rankMask & origin) != 0) ? ~allPieces : 0;
    legalMoves |= oneStepMove | twoStepMove;



    return legalMoves;
}

bool PawnValidator::validate(Bitboard origin, Bitboard target, const ChessBoard& board) const {
    Bitboard attacks = getMoves(origin, board);

    return (attacks & target) != 0;
}