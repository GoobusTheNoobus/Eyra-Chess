#pragma once

#include <string>
#include <cassert>
#include <stdexcept>
#include <sstream>
#include <string_view>
#include "type.hpp"

namespace Eyra {
    
class Position {
 public:
    // Constructors
    Position();
    Position(std::string_view fen);

    

    // Lookups
    Bitboard GetBitboard(Piece piece) const;
    Bitboard GetBitboard(PieceType pt, Color color) const;
    Piece GetPiece(Square square) const;
    Color SideToMove() const;
    CastlingRights GetCastlingRights () const;
    Square GetEPSquare() const;
    int GetRuleFifty() const;

    void ParseFEN(std::string_view fen);
    std::string ToString() const;

    void MakeMove (Move move);
    void UndoMove ();

    

 private:
    // Board and Pieces
    Piece pieces[64];
    Bitboard bitboards[12];
    Bitboard color_bitboards[2];
    Bitboard occupancy;

    // Game States
    Color side_to_move;
    CastlingRights castling_rights;
    Square ep_square;
    int rule_fifty;

    // Game History
    MoveList move_history;

    // Editting Functions
    void ClearPosition();
    void UpdateOccupancy();
    void ClearSquare (Square square);
    void SetSquare (Square square, Piece piece);
    

};

} // namespace Eyra
