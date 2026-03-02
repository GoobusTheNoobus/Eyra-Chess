#include "movegen.hpp"




namespace Eyra {

namespace {


template <Color us>
void GeneratePawnMoves(const Position& pos, MoveList& list) {

    
    constexpr Color them = Opposite(us);
    constexpr bool is_white = us == WHITE;

    
    Bitboard pieces = pos.GetBitboard(PAWN, us);
    Bitboard enemies = pos.GetBitboard(them);
    Bitboard occ = pos.GetOccupancy();

    // Early Exit
    if (!pieces) return;

    
    constexpr Bitboard rank_3_from_bottom = is_white ? Bitboards::rank3 : Bitboards::rank6;
    constexpr Bitboard promo_rank         = is_white ? Bitboards::rank8 : Bitboards::rank1;
    
    Square ep = pos.GetEPSquare();

    
    constexpr int push_dir = is_white ? 8: -8;
    constexpr int left_capture_dir = is_white ? 7 : -9;
    constexpr int right_capture_dir = is_white ? 9 : -7;

    
    Bitboard single_push = (is_white ? pieces << 8: pieces >> 8) & ~occ;
    Bitboard left_capture = (is_white ? 
        (pieces & ~Bitboards::file_a) << 7: 
        (pieces & ~Bitboards::file_a) >> 9) 
    & enemies;
    Bitboard right_capture = (is_white ?
    (pieces & ~Bitboards::file_h) << 9:
    (pieces & ~Bitboards::file_h) >> 7)
    & enemies;

    Bitboard double_push = (is_white ? ((single_push & rank_3_from_bottom) << 8): ((single_push & rank_3_from_bottom) >> 8)) & ~occ;

    // Split them into promotion or not. 
    // Note that double push doesn't need them, since its impossible for a double push to be promotion
    Bitboard single_promo = single_push & promo_rank;
    Bitboard single_normal = single_push & ~promo_rank;

    Bitboard left_capture_promo = left_capture & promo_rank;
    Bitboard left_capture_normal = left_capture & ~promo_rank;

    Bitboard right_capture_promo = right_capture & promo_rank;
    Bitboard right_capture_normal = right_capture & ~promo_rank;

    // Put them into the list
    while (single_normal) {
        int square = ctz(single_normal);
        single_normal &= single_normal - 1;
        list.Push(CreateNormalMove(Square(square - push_dir), Square(square)));

        // std::cout << "Single Normal" <<MoveToString(CreateNormalMove(Square(square - push_dir), Square(square), moving, NO_PIECE)) << "\n";
    }
    
    

    while (double_push) {
        int square = ctz(double_push);
        double_push &= double_push - 1;

        list.Push(CreateDoublePush(Square(square - push_dir * 2), Square(square)));
    }



    while (left_capture_normal) {
        int square = ctz(left_capture_normal);
        left_capture_normal &= left_capture_normal - 1;
        list.Push(CreateNormalMove(Square(square - left_capture_dir), Square(square)));
    }

    

    while (right_capture_normal) {
        int square = ctz(right_capture_normal);
        right_capture_normal &= right_capture_normal - 1;
        list.Push(CreateNormalMove(Square(square - right_capture_dir), Square(square)));
    }

    while (single_promo) {
        int square = ctz(single_promo);
        single_promo &= single_promo - 1;
        Move base = CreateNormalMove(Square(square - push_dir), Square(square));
        list.Push(CreatePromoMove<QUEEN>(base));
        list.Push(CreatePromoMove<KNIGHT>(base));
        list.Push(CreatePromoMove<ROOK>(base));
        list.Push(CreatePromoMove<BISHOP>(base));
    }

    while (left_capture_promo) {
        int square = ctz(left_capture_promo);
        left_capture_promo &= left_capture_promo - 1;
        Move base = CreateNormalMove(Square(square - left_capture_dir), Square(square));
        list.Push(CreatePromoMove<QUEEN>(base));
        list.Push(CreatePromoMove<KNIGHT>(base));
        list.Push(CreatePromoMove<ROOK>(base));
        list.Push(CreatePromoMove<BISHOP>(base));
    }

    while (right_capture_promo) {
        int square = ctz(right_capture_promo);
        right_capture_promo &= right_capture_promo - 1;
        Move base = CreateNormalMove(Square(square - right_capture_dir), Square(square));
        list.Push(CreatePromoMove<QUEEN>(base));
        list.Push(CreatePromoMove<KNIGHT>(base));
        list.Push(CreatePromoMove<ROOK>(base));
        list.Push(CreatePromoMove<BISHOP>(base));
    }

    if (ep != NO_SQUARE) {
        // The opposite color bitbouard contains the squares that our pawns have to be to perform en croissant
        Bitboard ep_bb = Bitboards::GetPawnAttacks<them>(ep) & pieces;

        while (ep_bb) {
            int square = ctz(ep_bb);
            ep_bb &= ep_bb - 1;
            list.Push(CreateEnPassant(Square(square), ep));
        }
    }

}


template <Color us, PieceType pt>
void GeneratePieceMoves(const Position& pos, MoveList& list) {

    // Stockfish does this so I will too
    static_assert(pt != PAWN, "Use GeneratePawnMoves for pawns");

    const Bitboard friendlies = pos.GetBitboard(us);
    const Bitboard occ = pos.GetOccupancy();

    Bitboard pieces = pos.GetBitboard(pt, us);

    while (pieces) {
        Square from = Square(ctz(pieces));
        pieces &= pieces - 1;

        Bitboard attacks;
        if constexpr (pt == KNIGHT) attacks = Bitboards::GetKnightAttacks(from);
        else if constexpr (pt == BISHOP) attacks = Bitboards::GetBishopAttacks(from, occ);
        else if constexpr (pt == ROOK)   attacks = Bitboards::GetRookAttacks(from, occ);
        else if constexpr (pt == QUEEN)  attacks = Bitboards::GetBishopAttacks(from, occ) | Bitboards::GetRookAttacks(from, occ);
        else if constexpr (pt == KING)   attacks = Bitboards::GetKingAttacks(from);

        attacks &= ~friendlies;

        while (attacks) {
            Square to = Square(ctz(attacks));
            attacks &= attacks - 1;
            list.Push(CreateNormalMove(from, to));
        }
    }
    
    
    if (pt == KING) {
        if (us == WHITE) {
            if (pos.CanCastleKingside()) {
                
                list.Push(CreateCastling(E1, G1));
            } if (pos.CanCastleQueenside()) {
                list.Push(CreateCastling(E1, C1));
            }
        } 
        if (us == BLACK) {
            if (pos.CanCastleKingside()) {
                list.Push(CreateCastling(E8, G8));
            } if (pos.CanCastleQueenside()) {
                list.Push(CreateCastling(E8, C8));
            }
        }
        
    }
    
    
}

template <Color Us>
void GenerateAll(const Position& pos, MoveList& list) {
    GeneratePawnMoves<Us>(pos, list);
    GeneratePieceMoves<Us, KNIGHT>(pos, list);
    GeneratePieceMoves<Us, BISHOP>(pos, list);
    GeneratePieceMoves<Us, ROOK>(pos, list);
    GeneratePieceMoves<Us, QUEEN>(pos, list);
    GeneratePieceMoves<Us, KING>(pos, list);
}


} // namespace anonymous


// Generate moves into prexisting movelist
void MoveGen::GenerateMoves(const Position& pos, MoveList& list) {
    pos.SideToMove() == WHITE ? GenerateAll<WHITE>(pos, list)
                              : GenerateAll<BLACK>(pos, list);
}

// Creates new movelist
MoveList MoveGen::GenerateMoves(const Position& pos) {
    MoveList list;
    pos.SideToMove() == WHITE ? GenerateAll<WHITE>(pos, list) 
                              : GenerateAll<BLACK>(pos, list);
    return list;
}






} // namespace Eyra

