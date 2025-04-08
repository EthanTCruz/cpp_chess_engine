// Bitboard.hpp
#ifndef BITBOARD_HPP
#define BITBOARD_HPP

#include <cstdint>

// Define a 64-bit bitboard type.
typedef uint64_t Bitboard;

// Magic structure to hold magic numbers, the attack table, and mask data.
struct Magic {
    Bitboard mask;
    Bitboard magic;
    Bitboard* attacks;
    int shift;
};

#endif // BITBOARD_HPP
