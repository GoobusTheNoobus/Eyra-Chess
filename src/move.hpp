#pragma once

#include <ostream>
#include "type.hpp"

namespace Eyra {
enum MoveFlag : uint8_t {
    NORMAL,
    DOUBLE_PUSH,
    CASTLING,
    EN_PASSANT,
    NPROMO,
    BPROMO,
    RPROMO,
    QPROMO
};

// Basic Move Function
FORCE_INLINE Move CreateMove(Square from, Square to, MoveFlag flag) {
    return
    (((from) & 0x3F) | 
    (((to) & 0x3F) << 6) |  
    (((flag) & 0xF) << 12));
}

// Inline functions for specific moves: only use these
FORCE_INLINE Move CreateNormalMove (Square from, Square to) { return CreateMove(from, to, NORMAL); }
FORCE_INLINE Move CreateDoublePush (Square from, Square to) { return CreateMove(from, to, DOUBLE_PUSH); }
FORCE_INLINE Move CreateCastling   (Square from, Square to) { return CreateMove(from, to, CASTLING); }
FORCE_INLINE Move CreateEnPassant  (Square from, Square to) { return CreateMove(from, to, EN_PASSANT); }
template <PieceType pt>
FORCE_INLINE Move CreatePromoMove  (Move base) { return base | (NPROMO + (pt - KNIGHT)) << 12; }

// Move Lookup
FORCE_INLINE Square GetFrom     (Move move) { return Square(move & 0x3F); }
FORCE_INLINE Square GetTo       (Move move) { return Square((move >> 6) & 0x3F); }
FORCE_INLINE MoveFlag GetFlag   (Move move) { return MoveFlag((move >> 12) & 0xF); }

FORCE_INLINE std::string MoveToString(Move move) {
    char flag_promo_char[] = "    nbrq";

    return SquareToString(GetFrom(move)) + SquareToString(GetTo(move)) + flag_promo_char[GetFlag(move)];
}

struct MoveList {
    Move moves[256];
    int count = 0;

    FORCE_INLINE void Push(Move move) { moves[count++] = move; }
    FORCE_INLINE Move Pop() { return moves[--count]; }

    Move operator[] (int i) const { return moves[i]; }

    // Iteratable Support
    FORCE_INLINE Move* begin() { return moves; }
    FORCE_INLINE Move* end() { return moves + count; }
    FORCE_INLINE const Move* begin() const { return moves; }
    FORCE_INLINE const Move* end() const { return moves + count; }

    FORCE_INLINE std::string ToString () const {

        std::string result;

        result.reserve(200);
        for (Move m: *this) {
            result += MoveToString(m) + "\n";
        }

        result += "There are " + std::to_string(count) + " moves in the movelist. \n";

        return result;
    }

    
};

FORCE_INLINE std::ostream& operator<< (std::ostream& os, const MoveList& list) {
    os << (list.ToString());
    return os;
}

} // namespace Eyra


