// BitOps.hpp
#ifndef BITOPS_HPP
#define BITOPS_HPP

#include "Bitboard.hpp"
#include <vector>
#include <random>

// Scans the bitboard from least significant bit to most significant bit
// and returns the index of the first set bit.
int bitScanForward(Bitboard bb);

// Counts the number of set bits in a bitboard.
int popcount(Bitboard bb);

// Sets occupancy given an index into the masks bits.
Bitboard setOccupancy(int index, int bitsInMask, Bitboard mask);

// Generates all occupancy variations for a given mask.
std::vector<Bitboard> generateOccupancies(Bitboard mask);

// Generates a random 64-bit number with few bits set.
Bitboard randomUint64FewBits(std::mt19937_64& rng);

#endif // BITOPS_HPP
