#pragma once

#include "type.hpp"
#include "position.hpp"
#include "bitboard.hpp"


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

namespace MoveGen {

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
inline Move CreatePromoMove  (Square from, Square to, Piece moved, Piece captured) { return CreateMove(from, to, moved, captured, MoveFlag(NPROMO + (pt - KNIGHT))); }

// Move Lookup
inline Square GetFrom     (Move move) { return Square(move & 0x3F); }
inline Square GetTo       (Move move) { return Square((move >> 6) & 0x3F); }
inline Piece  GetMoved    (Move move) { return Piece((move >> 12) & 0xF); }
inline Piece  GetCaptured (Move move) { return Piece((move >> 16) & 0xF); }
inline MoveFlag GetFlag   (Move move) { return MoveFlag((move >> 20) & 0xF); }

    
MoveList GenerateMoves (const Position& pos);
void GenerateMoves (const Position& pos, MoveList& list);

void GeneratePawnMoves   (const Position& pos, MoveList& list);
void GenerateKnightMoves (const Position& pos, MoveList& list);
void GenerateBishopMoves (const Position& pos, MoveList& list);
void GenerateRookMoves   (const Position& pos, MoveList& list);
void GenerateQueenMoves  (const Position& pos, MoveList& list);
void GenerateKingMoves   (const Position& pos, MoveList& list);

} // namespace MoveGen
} // namespace Eyra

