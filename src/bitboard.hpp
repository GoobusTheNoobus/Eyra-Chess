#pragma once

#include "type.hpp"

#ifdef _MSC_VER
#include <intrin.h>
#define popcount(x) __popcnt64 (x)
#elif defined(__GNUC__) || defined(__clang__)
#define popcount(x) __builtin_popcountll (x)
#else
#include <bit>
#define popcount(x) std::popcount(x)
#endif

namespace Eyra::Bitboards {

void init();

constexpr Bitboard rank1 = 0xFF;
constexpr Bitboard rank2 = rank1 << 8;
constexpr Bitboard rank3 = rank2 << 8;
constexpr Bitboard rank4 = rank3 << 8;
constexpr Bitboard rank5 = rank4 << 8;
constexpr Bitboard rank6 = rank5 << 8;
constexpr Bitboard rank7 = rank6 << 8;
constexpr Bitboard rank8 = rank7 << 8;

constexpr Bitboard file_a = 0x0101010101010101ULL;
constexpr Bitboard file_h = 0x8080808080808080ULL;

Bitboard SquareBB (Square square);

Bitboard GetRookAttacks   (Square square, Bitboard occupancy);
Bitboard GetBishopAttacks (Square square, Bitboard occupancy);

template <Color C>
Bitboard GetPawnAttacks   (Square square);
Bitboard GetKnightAttacks (Square square);
Bitboard GetKingAttacks   (Square square);


void generate_bishop_mask (Square square);
void generate_rook_mask (Square square);

} // namespace Eyra::Bitboards
