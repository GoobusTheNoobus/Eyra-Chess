#pragma once

#include <string>
#include <cassert>
#include <stdexcept>
#include <sstream>
#include "type.hpp"

namespace Beans {
    
class Position {
 public:
    // Constructors
    Position();
    Position(std::string fen);

    

    // Lookups
    Bitboard GetBitboard(Piece piece) const;
    Bitboard GetBitboard(PieceType piece, Color color) const;
    Piece GetPiece(Square square) const;
    Color SideToMove() const;
    CastlingRights GetCastlingRights () const;
    Square GetEPSquare() const;
    int GetRuleFifty() const;

    void ParseFEN(std::string fen);

    // Editting Functions are not needed publicly


    std::string ToString() const;


    bool HasMatingMaterial() const;

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

    // Editting Functions
    void ClearPosition();
    void UpdateOccupancy();
    void ClearSquare (Square square);
    void SetSquare (Square square, Piece piece);
    

};

} // namespace Beans
