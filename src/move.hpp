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
inline Move CreateMove(Square from, Square to, Piece moved, Piece captured, MoveFlag flag) {
    return
    (((from) & 0x3F) | 
    (((to) & 0x3F) << 6) | 
    (((moved) & 0xF) << 12) | 
    (((captured) & 0xF) << 16) | 
    (((flag) & 0xF) << 20));
}

// Inline functions for specific moves: only use these
inline Move CreateNormalMove (Square from, Square to, Piece moved, Piece captured) { return CreateMove(from, to, moved, captured, NORMAL); }
inline Move CreateDoublePush (Square from, Square to, Piece moved)                 { return CreateMove(from, to, moved, NO_PIECE, DOUBLE_PUSH); }
inline Move CreateCastling   (Square from, Square to, Piece moved)                 { return CreateMove(from, to, moved, NO_PIECE, CASTLING); }
inline Move CreateEnPassant  (Square from, Square to, Piece moved, Piece captured) { return CreateMove(from, to, moved, captured, EN_PASSANT); }
template <PieceType pt>
inline Move CreatePromoMove  (Move base) { return base | (NPROMO + (pt - KNIGHT)) << 20; }

// Move Lookup
inline Square GetFrom     (Move move) { return Square(move & 0x3F); }
inline Square GetTo       (Move move) { return Square((move >> 6) & 0x3F); }
inline Piece  GetMoved    (Move move) { return Piece((move >> 12) & 0xF); }
inline Piece  GetCaptured (Move move) { return Piece((move >> 16) & 0xF); }
inline MoveFlag GetFlag   (Move move) { return MoveFlag((move >> 20) & 0xF); }

inline std::string MoveToString(Move move) {
    char flag_promo_char[] = "    nbrq";

    return SquareToString(GetFrom(move)) + SquareToString(GetTo(move)) + flag_promo_char[GetFlag(move)];
}

struct MoveList {
    Move moves[256];
    int count = 0;

    inline void push(Move move) { moves[count++] = move; }
    inline Move pop() { return moves[--count]; }

    Move operator[] (int i) const { return moves[i]; }

    // Iteratable Support
    inline Move* begin() { return moves; }
    inline Move* end() { return moves + count; }
    inline const Move* begin() const { return moves; }
    inline const Move* end() const { return moves + count; }

    inline std::string ToString () const {
        std::string result;
        for (Move m: *this) {
            result += MoveToString(m) + "\n";
        }

        return result;
    }

    
};

inline std::ostream& operator<< (std::ostream& os, const MoveList& list) {
    os << (list.ToString());
    return os;
}

} // namespace Eyra


