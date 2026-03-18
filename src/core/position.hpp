#pragma once

#include <string>
#include <cassert>
#include <stdexcept>
#include <sstream>
#include <string_view>
#include "core/type.hpp"
#include "core/move.hpp"
#include "util/misc.hpp"
#include "core/bitboard.hpp"
#include <iostream>

namespace Eyra
{
    struct GameInfo
    {
        uint8_t rule_50 = 0;
        Square ep_square = NO_SQUARE;
        CastlingRights castling = 0;
    };

    struct UndoInfo
    {
        uint8_t rule_50 = 0;
        Square ep_square = NO_SQUARE;
        CastlingRights castling = 0;
        Piece captured;
    };

    class Position
    {
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
        CastlingRights GetCastlingRights() const;
        Square GetEPSquare() const;
        int GetRuleFifty() const;

        void ParseFEN(std::string_view fen);
        std::string ToString() const;

        // Moves
        void MakeMove(Move move);
        void MakeMove(const std::string& string_move);
        void UndoMove();

        // Checks / attacks
        bool IsAttacked(Square square, Color c) const;
        bool IsInCheck(Color c) const;
        bool IsInCheck() const;

        // Castling
        bool CanCastleKingside() const;
        bool CanCastleQueenside() const;

    private:
        // Game history
        Move move_history[256];
        UndoInfo info_history[256];
        uint64_t hash_history[256];
        uint8_t ply;

        // Board and pieces
        Piece pieces[64];
        Bitboard bitboards[12];
        Bitboard color_bitboards[2];
        Bitboard occupancy;

        // Game state
        Color side_to_move;
        GameInfo info;

        // Internal helpers
        void ClearPosition();
        void UpdateOccupancy();
        void ClearSquare(Square square);
        void SetSquare(Square square, Piece piece);
    };

    FORCE_INLINE std::ostream& operator<<(std::ostream& os, const Position& pos)
    {
        os << pos.ToString();
        return os;
    }

} // namespace Eyra