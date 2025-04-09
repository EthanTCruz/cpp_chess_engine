#ifndef PAWN_VALIDATOR_HPP
#define PAWN_VALIDATOR_HPP

#include "MoveValidator.hpp"
#include <array>
#include <cstdint>
#include "Bitboard.hpp"

// A validator for knight moves.
class PawnValidator : public MoveValidator {
public:
    PawnValidator();

    bool validate(int from_idx, int to_idx, const ChessBoard& board) const override;
    bool validate(Bitboard origin, Bitboard target, const ChessBoard& board) const;

    
    Bitboard getAttacks(Bitboard origin, const ChessBoard& board) const;
private:

    void initPawnMoves();
};

#endif // KNIGHT_VALIDATOR_HPP
