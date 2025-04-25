// constants.h
#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include "Bitboard.hpp"


namespace Files {
	const Bitboard A = 0x0101010101010101ULL;
	const Bitboard B = 0x0202020202020202ULL;
	const Bitboard C = 0x0404040404040404ULL;
	const Bitboard D = 0x0808080808080808ULL;
	const Bitboard E = 0x1010101010101010ULL;
	const Bitboard F = 0x2020202020202020ULL;
	const Bitboard G = 0x4040404040404040ULL;
	const Bitboard H = 0x8080808080808080ULL;


}

namespace Rank{
	const Bitboard RANK_1 = 0x00000000000000FFULL;
	const Bitboard RANK_2 = 0x000000000000FF00ULL;
	const Bitboard RANK_3 = 0x0000000000FF0000ULL;
	const Bitboard RANK_4 = 0x00000000FF000000ULL;
	const Bitboard RANK_5 = 0x000000FF00000000ULL;
	const Bitboard RANK_6 = 0x0000FF0000000000ULL;
	const Bitboard RANK_7 = 0x00FF000000000000ULL;
	const Bitboard RANK_8 = 0xFF00000000000000ULL;
}

namespace InitialPositions {
	const Bitboard w_king = 1ULL << 4;
	const Bitboard b_king = 1ULL << 60;

}

namespace CastlingTrigger {
	const Bitboard b_king = 1ULL << 62;
	const Bitboard b_queen = 1ULL << 58;
	const Bitboard w_king = 1ULL << 6;
	const Bitboard w_queen = 1ULL << 2;

}

#endif