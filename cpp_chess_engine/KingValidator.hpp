#ifndef KING_VALIDATOR_HPP
#define KING_VALIDATOR_HPP

#include "MoveValidator.hpp"
#include <array>
#include <cstdint>
#include "Bitboard.hpp"
#include "BitOps.hpp"

// A validator for king moves.
class KingValidator : public MoveValidator {
public:
    KingValidator();
    bool validate(int from_idx, int to_idx, const ChessBoard& board) const override;
    bool validate(int from_idx, Bitboard target, const ChessBoard& board) const;
    //bool validate(Bitboard origin, Bitboard target, const ChessBoard& board) const;

    Bitboard getAttacks(Bitboard origin, const ChessBoard& board) const;
    Bitboard getAttacks(int square, const ChessBoard& board) const;
private:
    std::array<uint64_t, 64> kingMoves;
    void initKingMoves();
};

#endif // KNIGHT_VALIDATOR_HPP
