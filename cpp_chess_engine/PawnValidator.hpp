#ifndef PAWN_VALIDATOR_HPP
#define PAWN_VALIDATOR_HPP

#include "MoveValidator.hpp"
#include <array>
#include <cstdint>

// A validator for knight moves.
class PawnValidator : public MoveValidator {
public:
    PawnValidator();
    bool validate(int from_idx, int to_idx, const ChessBoard& board) const override;

private:
    std::array<uint64_t, 64> knightMoves;
    void initPawnMoves();
};

#endif // KNIGHT_VALIDATOR_HPP
