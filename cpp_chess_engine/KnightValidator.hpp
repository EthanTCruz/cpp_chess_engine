#ifndef KNIGHT_VALIDATOR_HPP
#define KNIGHT_VALIDATOR_HPP

#include "MoveValidator.hpp"
#include <array>
#include <cstdint>
#include "Bitboard.hpp"

// A validator for knight moves.
class KnightValidator : public MoveValidator {
public:
    KnightValidator();
    bool validate(int from_idx, int to_idx, const ChessBoard& board) const override;
    Bitboard getAttacks(int square, const ChessBoard& board) const;
private:
    std::array<uint64_t, 64> knightMoves;
    void initKnightMoves();
};

#endif // KNIGHT_VALIDATOR_HPP
