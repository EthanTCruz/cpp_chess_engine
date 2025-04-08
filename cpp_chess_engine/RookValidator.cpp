// RookValidator.cpp
#include "RookValidator.hpp"
#include "ChessBoard.hpp"
#include <cstdint>
#include "BitOps.hpp" 
#include <cstdlib>
#include <ctime>
#include <vector>
#include <random>

#include <iostream>
#include <stdexcept>


#include <array>
#include <intrin.h>



Bitboard rookMasks[64];
Bitboard rookAttacks[64][4096]; // largest possible rook occupancy table size is 4096




// returns bitboard of actual attack path from square with occupancy
Bitboard rookAttacksOnTheFly(int square, Bitboard occupancy) {
    Bitboard attacks = 0ULL;
    int rank = square / 8;
    int file = square % 8;
    // Vertical moves
    for (int r = rank + 1; r <= 7; ++r) {
        attacks |= (1ULL << (r * 8 + file));
        if (occupancy & (1ULL << (r * 8 + file))) break;
    }
    for (int r = rank - 1; r >= 0; --r) {
        attacks |= (1ULL << (r * 8 + file));
        if (occupancy & (1ULL << (r * 8 + file))) break;
    }
    // Horizontal moves
    for (int f = file + 1; f <= 7; ++f) {
        attacks |= (1ULL << (rank * 8 + f));
        if (occupancy & (1ULL << (rank * 8 + f))) break;
    }
    for (int f = file - 1; f >= 0; --f) {
        attacks |= (1ULL << (rank * 8 + f));
        if (occupancy & (1ULL << (rank * 8 + f))) break;
    }
    return attacks;
}


Bitboard rookOccupancyMask(int square) {
    Bitboard mask = 0ULL;
    int rank = square / 8;
    int file = square % 8;
    for (int r = rank + 1; r <= 6; ++r) mask |= (1ULL << (r * 8 + file));
    for (int r = rank - 1; r >= 1; --r) mask |= (1ULL << (r * 8 + file));
    for (int f = file + 1; f <= 6; ++f) mask |= (1ULL << (rank * 8 + f));
    for (int f = file - 1; f >= 1; --f) mask |= (1ULL << (rank * 8 + f));
    return mask;
}



Bitboard findMagic(int square, int relevantBits, bool isRook) {
    std::vector<Bitboard> occupancies = generateOccupancies(rookOccupancyMask(square));
    std::vector<Bitboard> attacks(occupancies.size());
    for (size_t i = 0; i < occupancies.size(); i++)
        attacks[i] = rookAttacksOnTheFly(square, occupancies[i]);
    std::mt19937_64 rng(42);
    Bitboard magicCandidate;
    std::vector<Bitboard> used(4096); // max possible size for rook is 4096
    for (int trial = 0; trial < 100000000; trial++) {
        magicCandidate = randomUint64FewBits(rng);
        if (__popcnt64((magicCandidate * rookOccupancyMask(square)) & 0xFF00000000000000ULL) < 6)
            continue;
        std::fill(used.begin(), used.end(), 0ULL);
        bool collision = false;
        for (size_t i = 0; i < occupancies.size(); i++) {
            int magicIndex = (occupancies[i] * magicCandidate) >> (64 - relevantBits);
            if (used[magicIndex] == 0ULL) {
                used[magicIndex] = attacks[i];
            }
            else if (used[magicIndex] != attacks[i]) {
                collision = true;
                break;
            }
        }
        if (!collision) {
            // Found a suitable magic number!
            return magicCandidate;
        }
    }
    throw std::runtime_error("Failed to find a suitable magic number");
}



RookValidator::RookValidator() {
    initRookMagics();
}
RookValidator::~RookValidator() {
    for (int square = 0; square < 64; square++) {
        delete[] rookMagics[square].attacks;
    }
}
void RookValidator::initRookMagics() {
    for (int square = 0; square < 64; square++) {
        Magic& m = rookMagics[square];
        m.mask = rookOccupancyMask(square);
        int relevantBits = __popcnt64(m.mask);
        int occupancyVariations = 1 << relevantBits;
        m.shift = 64 - relevantBits;
        m.attacks = new Bitboard[occupancyVariations];
        m.magic = findMagic(square, relevantBits, true);
        for (int index = 0; index < occupancyVariations; index++) {
            Bitboard occupancy = setOccupancy(index, relevantBits, m.mask);
            int magicIndex = (occupancy * m.magic) >> m.shift;
            m.attacks[magicIndex] = rookAttacksOnTheFly(square, occupancy);
        }
    }
}

Bitboard RookValidator::getAttacks(int square, const ChessBoard& board) const {
    Bitboard occupancy = board.getAllPieces(); // ensure you have this method
    Bitboard friendlyPieces = board.getFriendlyPieces();
    if (((1ULL << square) & friendlyPieces) == 0) return false;
    Bitboard possibleMoves = getRookAttacks(square, occupancy);
    possibleMoves &= ~friendlyPieces; // ensure you have this method

    return possibleMoves;
}

inline Bitboard RookValidator::getRookAttacks(int square, Bitboard occupancy) const {
    occupancy &= rookMagics[square].mask;
    occupancy *= rookMagics[square].magic;
    occupancy >>= rookMagics[square].shift;
    return rookMagics[square].attacks[occupancy];
}
bool RookValidator::validate(int from_idx, int to_idx, const ChessBoard& board) const {
	Bitboard attacks = getAttacks(from_idx, board);
    return (attacks & (1ULL << to_idx)) > 0;
}