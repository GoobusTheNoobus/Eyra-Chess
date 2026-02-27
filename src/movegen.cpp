#include "movegen.hpp"

namespace Eyra {
    
void MoveGen::GeneratePawnMoves(const Position& pos, MoveList& list) {
    Color us = pos.SideToMove();
    Color them = pos.SideToMove();

    bool is_white = us == WHITE;

    Piece moving = MakePiece(PAWN, us);
    Bitboard pieces = pos.GetBitboard(moving);

    // Early Exit
    if (!pieces) return;

    Bitboard rank_3_from_bottom = is_white ? Bitboards::rank3 : Bitboards::rank6;
    Bitboard promo_rank         = is_white ? Bitboards::rank8 : Bitboards::rank1;
    
    Square ep = pos.GetEPSquare();

    int push_dir = is_white ? 8: -8;
    int left_capture_dir = is_white ? 7 : -9;
    int right_capture_dir = is_wctype ? 9 : -7;

    Bitboard enemies = pos.GetBitboard(them);
    Bitboard occ = pos.GetOccupancy();

    Bitboard single_push = (is_white ? pieces << 8: pieces >> 8) & ~occ;
    Bitboard double_push = (is_white ? ((single_push & rank_3_from_bottom) << 8): ((single_push & rank_3_from_bottom) >> 8)) & ~occ;
    
}

} // namespace Eyra
