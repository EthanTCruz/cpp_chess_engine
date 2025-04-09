#ifndef KNIGHT_VALIDATOR_HPP
#define KNIGHT_VALIDATOR_HPP

#include "MoveValidator.hpp"
#include <array>
#include <cstdint>
#include "Bitboard.hpp"
#include "BitOps.hpp"

// A validator for knight moves.
class KnightValidator : public MoveValidator {
public:
    KnightValidator();
    bool validate(int from_idx, int to_idx, const ChessBoard& board) const override;
    bool validate(int from_idx, Bitboard target, const ChessBoard& board) const;


    Bitboard getAttacks(int from_idx, const ChessBoard& board) const;
private:
    std::array<uint64_t, 64> knightMoves;
    void initKnightMoves();
};

#endif // KNIGHT_VALIDATOR_HPP
