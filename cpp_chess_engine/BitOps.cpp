// BitOps.cpp
#include "BitOps.hpp"
#include <cstdint>
#include <cstdlib>
#include <ctime>
#ifdef _WIN64
#include <intrin.h>
#endif

// Scans the bitboard from least significant bit to most significant bit
// and returns the index of the first set bit.
int bitScanForward(Bitboard bb) {
    unsigned long index;
#ifdef _WIN64
    _BitScanForward64(&index, bb);
#else
    if (static_cast<uint32_t>(bb) != 0) {
        // Note: on non-Windows systems you might use __builtin_ctzll if available.
        _BitScanForward(&index, static_cast<uint32_t>(bb));
    }
    else {
        _BitScanForward(&index, static_cast<uint32_t>(bb >> 32));
        index += 32;
    }
#endif
    return static_cast<int>(index);
}

// Sets occupancy given an index into the mask’s bit count.
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
    int bits = __popcnt64(mask);  // Use __popcnt64 on MSVC or __builtin_popcountll on GCC/Clang.
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
