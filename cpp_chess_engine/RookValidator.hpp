#ifndef ROOK_VALIDATOR_HPP
#define ROOK_VALIDATOR_HPP

#include "MoveValidator.hpp"
#include <array>
#include <cstdint>

// A validator for knight moves.
class RookValidator : public MoveValidator {
public:
    RookValidator();
    bool validate(int from_idx, int to_idx, const ChessBoard& board) const override;

private:
    std::array<uint64_t, 64> rookMoves;
    void initRookMoves();
};

#endif // ROOK_VALIDATOR_HPP
