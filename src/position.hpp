#pragma once

#include <string>
#include <cassert>
#include <stdexcept>
#include <sstream>
#include <string_view>
#include "type.hpp"
#include "move.hpp"
#include "bitboard.hpp"

#include <iostream>



namespace Eyra {

struct GameInfo {
   uint8_t rule_50 = 0;
   Square ep_square = NO_SQUARE;
   CastlingRights castling = 0;
};

class Position {
 public:
    // Constructors
    Position();
    Position(std::string_view fen);

    

    // Lookups
    Bitboard GetBitboard(Piece piece) const;
    Bitboard GetBitboard(PieceType pt, Color color) const;
    Bitboard GetBitboard(Color color) const;
    Bitboard GetOccupancy() const;
    Piece GetPiece(Square square) const;
    Color SideToMove() const;
    CastlingRights GetCastlingRights () const;
    Square GetEPSquare() const;
    int GetRuleFifty() const;

    void ParseFEN(std::string_view fen);
    std::string ToString() const;

    

    void MakeMove (Move move);
    void UndoMove ();

    bool IsAttacked (Square square, Color c) const;
    bool IsInCheck  (Color c) const;
    bool IsInCheck  () const;

    bool CanCastleKingside () const;
    bool CanCastleQueenside() const;

    

 private:
    // Board and Pieces
    Piece pieces[64];
    Bitboard bitboards[12];
    Bitboard color_bitboards[2];
    Bitboard occupancy;

    // Game States
    Color side_to_move;
    
    GameInfo info;

    // Game History
    Move move_history[256];
    GameInfo info_history[256];
    uint64_t hash_history[256];
    uint8_t counter = 0;

    // Editting Functions
    void ClearPosition();
    void UpdateOccupancy();
    void ClearSquare (Square square);
    void SetSquare (Square square, Piece piece);


    
    

};

FORCE_INLINE std::ostream& operator<< (std::ostream& os, const Position& pos) {
   os << pos.ToString();

   return os;
}

} // namespace Eyra
