#ifndef ROOK_VALIDATOR_HPP
#define ROOK_VALIDATOR_HPP

#include "MoveValidator.hpp"
#include <array>
#include <cstdint>


typedef uint64_t Bitboard;

struct Magic {
    Bitboard mask;
    Bitboard magic;
    Bitboard* attacks;
    int shift;
};

// A validator for knight moves.
class RookValidator : public MoveValidator {
public:
    RookValidator();
    ~RookValidator();
    bool validate(int from_idx, int to_idx, const ChessBoard& board) const override;

private:
    std::array<uint64_t, 64> rookMoves;
    Magic rookMagics[64];
    void initRookMoves();
    void initRookMagics();
    inline Bitboard getRookAttacks(int square, Bitboard occupancy) const;
};

#endif // ROOK_VALIDATOR_HPP
