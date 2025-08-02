// BitOps.cpp
#include "BitOps.hpp"
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <bitset>
#if defined(_MSC_VER)
#include <intrin.h>
#endif

// Scans the bitboard from least significant bit to most significant bit
// and returns the index of the first set bit.
int bitScanForward(Bitboard bb) {
#if defined(_MSC_VER)
#if defined(_WIN64)
    unsigned long index;
    _BitScanForward64(&index, bb);
    return static_cast<int>(index);
#else
    unsigned long index;
    if (static_cast<uint32_t>(bb) != 0) {
        _BitScanForward(&index, static_cast<uint32_t>(bb));
        return static_cast<int>(index);
    } else {
        _BitScanForward(&index, static_cast<uint32_t>(bb >> 32));
        return static_cast<int>(index + 32);
    }
#endif
#else
    return static_cast<int>(__builtin_ctzll(bb));
#endif
}

// Counts the number of set bits in a bitboard.
int popcount(Bitboard bb) {
#if defined(_MSC_VER)
    return static_cast<int>(__popcnt64(bb));
#else
    return __builtin_popcountll(bb);
#endif
}

// Sets occupancy given an index into the masks bit count.
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

// Generates all occupancy variations for a given mask.
std::vector<Bitboard> generateOccupancies(Bitboard mask) {
    int bits = popcount(mask);
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

// Generates a random 64-bit number with few bits set.
Bitboard randomUint64FewBits(std::mt19937_64& rng) {
    return rng() & rng() & rng();
}

void printBitboard(Bitboard bitboard) {
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
