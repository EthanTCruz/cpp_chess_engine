// RookValidator.cpp
#include "RookValidator.hpp"
#include "ChessBoard.hpp"
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <vector>


#ifdef _MSC_VER
#include <intrin.h>

// -----------------------------------------------------------------------------
// Helper functions used in magic bitboard generation
// -----------------------------------------------------------------------------

// Returns a random 64-bit integer.
// (We combine several calls to rand() to get 64 random bits.)
static uint64_t random64() {
    return (((uint64_t)rand() & 0xFFFFULL)) |
        ((((uint64_t)rand() & 0xFFFFULL)) << 16) |
        ((((uint64_t)rand() & 0xFFFFULL)) << 32) |
        ((((uint64_t)rand() & 0xFFFFULL)) << 48);
}

// -----------------------------------------------------------------------------
// Functions from the previous implementation (adapted as static helpers)
// -----------------------------------------------------------------------------


static int countBits(uint64_t bb) {
    return __popcnt64(bb);
}
#else
static int countBits(uint64_t bb) {
    return __builtin_popcountll(bb);
}
#endif


// Generate the rook mask for a given square (exclude edge squares).
static uint64_t generateRookMask(int square) {
    uint64_t mask = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // North (exclude last rank)
    for (int r = rank + 1; r <= 6; r++) {
        mask |= (1ULL << (file + r * 8));
    }
    // South (exclude first rank)
    for (int r = rank - 1; r >= 1; r--) {
        mask |= (1ULL << (file + r * 8));
    }
    // East (exclude last file)
    for (int f = file + 1; f <= 6; f++) {
        mask |= (1ULL << (f + rank * 8));
    }
    // West (exclude first file)
    for (int f = file - 1; f >= 1; f--) {
        mask |= (1ULL << (f + rank * 8));
    }

    return mask;
}

// Generate the rook’s attack bitboard for a given occupancy.
static uint64_t generateRookAttacksForOccupancy(int square, uint64_t occupancy) {
    uint64_t attacks = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // North
    for (int r = rank + 1; r < 8; r++) {
        int sq = file + r * 8;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq))
            break;
    }
    // South
    for (int r = rank - 1; r >= 0; r--) {
        int sq = file + r * 8;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq))
            break;
    }
    // East
    for (int f = file + 1; f < 8; f++) {
        int sq = f + rank * 8;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq))
            break;
    }
    // West
    for (int f = file - 1; f >= 0; f--) {
        int sq = f + rank * 8;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq))
            break;
    }

    return attacks;
}

// Given a mask and an index, generate an occupancy variation.
static uint64_t setOccupancy(int index, int bitsInMask, uint64_t mask) {
    uint64_t occupancy = 0ULL;
    for (int count = 0; count < bitsInMask; count++) {
        // Isolate the least-significant 1 bit in mask.
        uint64_t squareBit = mask & ~mask;
        mask &= mask - 1;

        // If the corresponding bit in index is set, add the square.
        if (index & (1 << count))
            occupancy |= squareBit;
    }
    return occupancy;
}

// -----------------------------------------------------------------------------
// Magic number generation for rooks
// -----------------------------------------------------------------------------

// For a given square, try to find a magic number that maps all occupancy variations
// (based on the generated mask) to unique attack bitboards.
static uint64_t findMagicNumber(int square, int relevantBits, bool isRook) {
    int occupancyVariations = 1 << relevantBits;
    std::vector<uint64_t> occupancies(occupancyVariations);
    std::vector<uint64_t> attacks(occupancyVariations);

    uint64_t mask = generateRookMask(square);
    for (int index = 0; index < occupancyVariations; index++) {
        occupancies[index] = setOccupancy(index, relevantBits, mask);
        attacks[index] = generateRookAttacksForOccupancy(square, occupancies[index]);
    }

    // Ensure the random generator is seeded (only once)
    static bool seedInitialized = false;
    if (!seedInitialized) {
        srand((unsigned)time(nullptr));
        seedInitialized = true;
    }

    while (true) {
        // Generate a candidate magic number (using bitwise AND to produce a sparse candidate).
        uint64_t candidate = random64() & random64() & random64();

        // The candidate is only acceptable if a simple heuristic holds (this heuristic may vary).
        if (__popcnt64((mask * candidate) & 0xFF00000000000000ULL) < 6) {
            std::vector<uint64_t> used(occupancyVariations, 0ULL);
            bool fail = false;
            for (int i = 0; i < occupancyVariations; i++) {
                int index = (int)((occupancies[i] * candidate) >> (64 - relevantBits));
                if (used[index] == 0ULL)
                    used[index] = attacks[i];
                else if (used[index] != attacks[i]) {
                    fail = true;
                    break;
                }
            }
            if (!fail)
                return candidate;
        }
    }
}

// -----------------------------------------------------------------------------
// Global arrays to store magic numbers, masks, and attack tables.
// -----------------------------------------------------------------------------

// For each square, store the generated magic number.
static uint64_t rookMagics[64];
// For each square, store the mask of relevant sliding squares.
static uint64_t rookMasks[64];
// For each square, store the lookup table for attacks.
static std::vector<uint64_t> rookAttackTables[64];

// -----------------------------------------------------------------------------
// RookValidator Implementation
// -----------------------------------------------------------------------------

RookValidator::RookValidator() {
    initRookMoves();
}

void RookValidator::initRookMoves() {
    // For each square on the board…
    for (int square = 0; square < 64; square++) {
        // Compute the mask for sliding occupancy (only non-edge squares matter).
        rookMasks[square] = generateRookMask(square);
        int relevantBits = countBits(rookMasks[square]);
        int occupancyVariations = 1 << relevantBits;

        // Generate the magic number for this square.
        rookMagics[square] = findMagicNumber(square, relevantBits, true);

        // Resize the attack table for this square.
        rookAttackTables[square].resize(occupancyVariations);

        // Enumerate every possible occupancy variation for the mask.
        for (int index = 0; index < occupancyVariations; index++) {
            uint64_t occupancy = setOccupancy(index, relevantBits, rookMasks[square]);
            uint64_t attacks = generateRookAttacksForOccupancy(square, occupancy);
            // Compute the magic index.
            int magicIndex = (int)((occupancy * rookMagics[square]) >> (64 - relevantBits));
            rookAttackTables[square][magicIndex] = attacks;
        }

        // For convenience, store the attack with an empty occupancy in the member array.
        uint64_t emptyOccupancy = 0ULL;
        int magicIndex = (int)((emptyOccupancy * rookMagics[square]) >> (64 - relevantBits));
        rookMoves[square] = rookAttackTables[square][magicIndex];
    }
}

bool RookValidator::validate(int from_idx, int to_idx, const ChessBoard& board) const {
    // Retrieve the board's overall occupancy.
    // (This implementation assumes ChessBoard provides a method getOccupancy().)
    uint64_t boardOccupancy = board.getAllPieces();

    int square = from_idx;
    int relevantBits = countBits(rookMasks[square]);
    // Mask the board occupancy so that only squares relevant for this rook move are considered.
    uint64_t occupancy = boardOccupancy & rookMasks[square];
    int magicIndex = (int)((occupancy * rookMagics[square]) >> (64 - relevantBits));
    uint64_t legalAttacks = rookAttackTables[square][magicIndex];

    // Validate that the destination square is within the legal attacks.
    return (legalAttacks & (1ULL << to_idx)) != 0;
}
