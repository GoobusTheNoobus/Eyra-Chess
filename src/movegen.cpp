#include "movegen.hpp"




namespace Eyra {

namespace {


template <Color us>
void GeneratePawnMoves(const Position& pos, MoveList& list) {

    
    constexpr Color them = Opposite(us);
    constexpr bool is_white = us == WHITE;

    
    constexpr Piece moving = MakePiece(PAWN, us);

    
    Bitboard pieces = pos.GetBitboard(moving);
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
        list.push(CreateNormalMove(Square(square - push_dir), Square(square), moving, NO_PIECE));

        // std::cout << "Single Normal" <<MoveToString(CreateNormalMove(Square(square - push_dir), Square(square), moving, NO_PIECE)) << "\n";
    }
    
    

    while (double_push) {
        int square = ctz(double_push);
        double_push &= double_push - 1;

        list.push(CreateDoublePush(Square(square - push_dir * 2), Square(square), moving));
    }



    while (left_capture_normal) {
        int square = ctz(left_capture_normal);
        left_capture_normal &= left_capture_normal - 1;
        list.push(CreateNormalMove(Square(square - left_capture_dir), Square(square), moving, pos.GetPiece(Square(square))));
    }

    

    while (right_capture_normal) {
        int square = ctz(right_capture_normal);
        right_capture_normal &= right_capture_normal - 1;
        list.push(CreateNormalMove(Square(square - right_capture_dir), Square(square), moving, pos.GetPiece(Square(square))));
    }

    while (single_promo) {
        int square = ctz(single_promo);
        single_promo &= single_promo - 1;
        Move base = CreateNormalMove(Square(square - push_dir), Square(square), moving, NO_PIECE);
        list.push(CreatePromoMove<QUEEN>(base));
        list.push(CreatePromoMove<KNIGHT>(base));
        list.push(CreatePromoMove<ROOK>(base));
        list.push(CreatePromoMove<BISHOP>(base));
    }

    while (left_capture_promo) {
        int square = ctz(left_capture_promo);
        left_capture_promo &= left_capture_promo - 1;
        Move base = CreateNormalMove(Square(square - left_capture_dir), Square(square), moving, pos.GetPiece(Square(square)));
        list.push(CreatePromoMove<QUEEN>(base));
        list.push(CreatePromoMove<KNIGHT>(base));
        list.push(CreatePromoMove<ROOK>(base));
        list.push(CreatePromoMove<BISHOP>(base));
    }

    while (right_capture_promo) {
        int square = ctz(right_capture_promo);
        right_capture_promo &= right_capture_promo - 1;
        Move base = CreateNormalMove(Square(square - right_capture_dir), Square(square), moving, pos.GetPiece(Square(square)));
        list.push(CreatePromoMove<QUEEN>(base));
        list.push(CreatePromoMove<KNIGHT>(base));
        list.push(CreatePromoMove<ROOK>(base));
        list.push(CreatePromoMove<BISHOP>(base));
    }

    if (ep != NO_SQUARE) {
        // The opposite color bitbouard contains the squares that our pawns have to be to perform en croissant
        Bitboard ep_bb = Bitboards::GetPawnAttacks<them>(ep) & pieces;

        while (ep_bb) {
            int square = ctz(ep_bb);
            ep_bb &= ep_bb - 1;
            list.push(CreateEnPassant(Square(square), ep, moving, is_white ? B_PAWN : W_PAWN));
        }
    }

}


template <Color us, PieceType pt>
void GeneratePieceMoves(const Position& pos, MoveList& list) {

    // Stockfish does this so I will too
    static_assert(pt != PAWN, "Use GeneratePawnMoves for pawns");

    const Bitboard friendlies = pos.GetBitboard(us);
    const Bitboard occ = pos.GetOccupancy();
    const Piece moving = MakePiece(pt, us);

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
            list.push(CreateNormalMove(from, to, moving, pos.GetPiece(to)));
        }
    }
    // TO-DO: Castling
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

