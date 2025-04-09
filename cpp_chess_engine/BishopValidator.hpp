#ifndef BISHOP_VALIDATOR_HPP
#define BISHOP_VALIDATOR_HPP
#include "Bitboard.hpp"
#include "MoveValidator.hpp"
#include <array>
#include <cstdint>




// A validator for knight moves.
class BishopValidator : public MoveValidator {
public:
    BishopValidator();
    ~BishopValidator();
    bool validate(int from_idx, int to_idx, const ChessBoard& board) const override;
    bool validate(int from_idx, Bitboard target, const ChessBoard& board) const;

    Bitboard getAttacks(int square, const ChessBoard& board) const;
private:
    std::array<Bitboard, 64> bishopMoves;
    Magic bishopMagics[64];
    void initBishopMoves();
    void initBishopMagics();
    inline Bitboard getBishopAttacks(int square, Bitboard occupancy) const;
};

#endif // BISHOP_VALIDATOR_HPP
