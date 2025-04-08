#ifndef ROOK_VALIDATOR_HPP
#define ROOK_VALIDATOR_HPP

#include "MoveValidator.hpp"
#include <array>
#include <cstdint>
#include "Bitboard.hpp"

#include <cstdint>


// A validator for knight moves.
class RookValidator : public MoveValidator {
public:
    RookValidator();
    ~RookValidator();
    bool validate(int from_idx, int to_idx, const ChessBoard& board) const override;
	Bitboard getAttacks(int square, const ChessBoard& board) const;

private:
    std::array<Bitboard, 64> rookMoves;
    Magic rookMagics[64];
    void initRookMoves();
    void initRookMagics();
    inline Bitboard getRookAttacks(int square, Bitboard occupancy) const;
};

#endif // ROOK_VALIDATOR_HPP
