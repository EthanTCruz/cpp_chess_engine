// BishopValidator.cpp
#include "BishopValidator.hpp"
#include "ChessBoard.hpp"
#include "BitOps.hpp"
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
#include <stdexcept>


Magic bishopMagics[64];



// Returns the bishop occupancy mask for a given square.
// Only the squares along diagonals (excluding the board edges) are included.
Bitboard bishopOccupancyMask(int square) {
    Bitboard mask = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // Upper right diagonal
    for (int r = rank + 1, f = file + 1; r <= 6 && f <= 6; r++, f++)
        mask |= (1ULL << (r * 8 + f));
    // Upper left diagonal
    for (int r = rank + 1, f = file - 1; r <= 6 && f >= 1; r++, f--)
        mask |= (1ULL << (r * 8 + f));
    // Lower right diagonal
    for (int r = rank - 1, f = file + 1; r >= 1 && f <= 6; r--, f++)
        mask |= (1ULL << (r * 8 + f));
    // Lower left diagonal
    for (int r = rank - 1, f = file - 1; r >= 1 && f >= 1; r--, f--)
        mask |= (1ULL << (r * 8 + f));

    return mask;
}

// Generates bishop attacks on the fly given a square and an occupancy bitboard.
Bitboard bishopAttacksOnTheFly(int square, Bitboard occupancy) {
    Bitboard attacks = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // Upper right
    for (int r = rank + 1, f = file + 1; r <= 7 && f <= 7; r++, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupancy & (1ULL << (r * 8 + f)))
            break;
    }
    // Upper left
    for (int r = rank + 1, f = file - 1; r <= 7 && f >= 0; r++, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupancy & (1ULL << (r * 8 + f)))
            break;
    }
    // Lower right
    for (int r = rank - 1, f = file + 1; r >= 0 && f <= 7; r--, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupancy & (1ULL << (r * 8 + f)))
            break;
    }
    // Lower left
    for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupancy & (1ULL << (r * 8 + f)))
            break;
    }

    return attacks;
}



// ================= Magic Number Finder for Bishops =================

// Finds a magic number for the bishop on a given square.
Bitboard findMagicBishop(int square, int relevantBits) {
    Bitboard mask = bishopOccupancyMask(square);
    std::vector<Bitboard> occupancies = generateOccupancies(mask);
    std::vector<Bitboard> attacks(occupancies.size());
    for (size_t i = 0; i < occupancies.size(); i++)
        attacks[i] = bishopAttacksOnTheFly(square, occupancies[i]);

    std::mt19937_64 rng(42);
    Bitboard magicCandidate;
    int occupancyVariations = 1 << relevantBits;
    std::vector<Bitboard> used(occupancyVariations);
    for (int trial = 0; trial < 100000000; trial++) {
        magicCandidate = randomUint64FewBits(rng);
        // Heuristic check similar to the rook version; adjust the threshold as needed.
        if (__popcnt64((magicCandidate * mask) & 0xFF00000000000000ULL) < 6)
            continue;

        std::fill(used.begin(), used.end(), 0ULL);
        bool collision = false;
        for (size_t i = 0; i < occupancies.size(); i++) {
            int magicIndex = (int)((occupancies[i] * magicCandidate) >> (64 - relevantBits));
            if (used[magicIndex] == 0ULL) {
                used[magicIndex] = attacks[i];
            }
            else if (used[magicIndex] != attacks[i]) {
                collision = true;
                break;
            }
        }
        if (!collision)
            return magicCandidate;
    }
    throw std::runtime_error("Failed to find a suitable bishop magic number");
}

// ================= BishopValidator Class Implementation =================

BishopValidator::BishopValidator() {
    initBishopMagics();
}

BishopValidator::~BishopValidator() {
    for (int square = 0; square < 64; square++) {
        delete[] bishopMagics[square].attacks;
    }
}

// Initializes bishop magic numbers and attack tables for each square.
void BishopValidator::initBishopMagics() {
    for (int square = 0; square < 64; square++) {
        Magic& m = bishopMagics[square];
        m.mask = bishopOccupancyMask(square);
        int relevantBits = __popcnt64(m.mask);
        int occupancyVariations = 1 << relevantBits;
        m.shift = 64 - relevantBits;
        m.attacks = new Bitboard[occupancyVariations];
        m.magic = findMagicBishop(square, relevantBits);
        for (int index = 0; index < occupancyVariations; index++) {
            Bitboard occupancy = setOccupancy(index, relevantBits, m.mask);
            int magicIndex = (int)((occupancy * m.magic) >> m.shift);
            m.attacks[magicIndex] = bishopAttacksOnTheFly(square, occupancy);
        }
    }
}

Bitboard BishopValidator::getAttacks(int square, const ChessBoard& board) const {
    Bitboard occupancy = board.getAllPieces();        // Must return a bitboard with all pieces.
    Bitboard friendlyPieces = board.getFriendlyPieces(); // Must return a bitboard with friendly pieces.
    // Ensure the moving piece is a friendly bishop.
    //if (((1ULL << square) & friendlyPieces) == 0) return false;
    Bitboard possibleMoves = getBishopAttacks(square, occupancy);
    possibleMoves &= ~friendlyPieces;
    return possibleMoves;
}


// Retrieves bishop attack bitboard from precomputed magic table.
inline Bitboard BishopValidator::getBishopAttacks(int square, Bitboard occupancy) const {
    occupancy &= bishopMagics[square].mask;
    occupancy *= bishopMagics[square].magic;
    occupancy >>= bishopMagics[square].shift;
    return bishopMagics[square].attacks[occupancy];
}

// Validates whether moving a bishop from from_idx to to_idx is legal.
bool BishopValidator::validate(int from_idx, int to_idx, const ChessBoard& board) const {
    Bitboard attacks = getAttacks(from_idx, board);
    return (attacks & (1ULL << to_idx)) != 0;
}

bool BishopValidator::validate(int from_idx, Bitboard target, const ChessBoard& board) const {
    Bitboard attacks = getAttacks(from_idx, board);
    return (attacks & target) != 0;
}