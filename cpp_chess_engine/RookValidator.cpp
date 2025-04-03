// RookValidator.cpp
#include "RookValidator.hpp"
#include "ChessBoard.hpp"
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <random>
#include <intrin.h>
#include <iostream>
#include <stdexcept>


typedef uint64_t Bitboard;

void printBitboard(uint64_t bitboard) {
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int index = rank * 8 + file;
            if (bitboard & (1ULL << index)) {
                std::cout << "1 ";
            }
            else {
                std::cout << "0 ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void printBitset(const std::bitset<64>& bitset) {
    for (int i = 63; i >= 0; --i) {
        std::cout << bitset[i] << " ";
    }
    std::cout << std::endl;
}

Bitboard rookMasks[64];
Bitboard rookAttacks[64][4096]; // largest possible rook occupancy table size is 4096

#pragma intrinsic(__popcnt64, _BitScanForward64)

// This function scans the bitboard from least significant bit to most significant bit and returns the index of the first set bit.
int bitScanForward(uint64_t bb) {
    unsigned long index;
#ifdef _WIN64
    _BitScanForward64(&index, bb);
#else
    if ((uint32_t)bb != 0) {
        _BitScanForward(&index, (uint32_t)bb);
    }
    else {
        _BitScanForward(&index, (uint32_t)(bb >> 32));
        index += 32;
    }
#endif
    return (int)index;
}


Bitboard randomUint64FewBits(std::mt19937_64& rng) {
    return rng() & rng() & rng(); // Generates sparse candidates (fewer bits set)
}
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

std::vector<Bitboard> generateOccupancies(Bitboard mask) {
    int bits = __popcnt64(mask);
    int occupancyVariations = 1 << bits;
    std::vector<Bitboard> occupancies(occupancyVariations);
    for (int index = 0; index < occupancyVariations; index++) {
        Bitboard occupancy = 0ULL;
        Bitboard bitsSet = mask;
        for (int i = 0; i < bits; i++) {
            int square = bitScanForward(bitsSet);
            bitsSet &= bitsSet - 1;
            if (index & (1 << i))
                occupancy |= (1ULL << square);
        }
        occupancies[index] = occupancy;
    }
    return occupancies;
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



Bitboard setOccupancy(int index, int bitsInMask, Bitboard mask) {
    Bitboard occupancy = 0ULL;
    for (int i = 0; i < bitsInMask; i++) {
        int square = bitScanForward(mask);
        mask &= mask - 1;
        if (index & (1 << i))
            occupancy |= (1ULL << square);
    }
    return occupancy;
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
inline Bitboard RookValidator::getRookAttacks(int square, Bitboard occupancy) const {
    occupancy &= rookMagics[square].mask;
    occupancy *= rookMagics[square].magic;
    occupancy >>= rookMagics[square].shift;
    return rookMagics[square].attacks[occupancy];
}
bool RookValidator::validate(int from_idx, int to_idx, const ChessBoard& board) const {
    Bitboard occupancy = board.getAllPieces(); // ensure you have this method
	Bitboard friendlyPieces = board.getFriendlyPieces(); 
	if (((1ULL << from_idx) & friendlyPieces) == 0) return false;
    Bitboard possibleMoves = getRookAttacks(from_idx, occupancy);
	possibleMoves &= ~friendlyPieces; // ensure you have this method

    return (possibleMoves & (1ULL << to_idx)) > 0;
}