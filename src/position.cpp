#include "position.hpp"

namespace Eyra {

Position::Position() {
    // Set starting position
    ParseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

Position::Position(std::string_view fen) {
    ParseFEN(fen);
}

// Gets a certain piece's bitboard
Bitboard Position::GetBitboard(Piece piece) const {
    return bitboards[piece];
}

Bitboard Position::GetBitboard(PieceType pt, Color color) const {
    return bitboards[MakePiece(pt, color)];
}

Bitboard Position::GetBitboard(Color color) const {
    return color_bitboards[color];
}

Bitboard Position::GetOccupancy() const {
    return occupancy;
}

Piece Position::GetPiece (Square square) const {
    return pieces[square];
}

Color Position::SideToMove () const {
    return side_to_move;
}

CastlingRights Position::GetCastlingRights () const {
    return info.castling;
}

Square Position::GetEPSquare () const {
    return info.ep_square;
}

int Position::GetRuleFifty () const {
    return info.rule_50;
}


// Parses a Forsyth-Edwards Notation string
void Position::ParseFEN(std::string_view fen) {
    // Start from empty board
    ClearPosition();

    int rank = 7;
    int file = 0;

    size_t i = 0;

    while (i < fen.size() && fen[i] != ' ') {
        char c = fen[i];

        // New Rank
        if (c == '/') {
            assert(file == 8);
            file = 0;
            --rank;
        }

        // Empty Squares
        else if (std::isdigit(c)) {
            file += c - '0';
        }

        // Piece
        else {
            assert(file < 8 && rank >= 0);

            Piece p = NO_PIECE;

            switch (c) {
                case 'P': p = W_PAWN; break;
                case 'N': p = W_KNIGHT; break;
                case 'B': p = W_BISHOP; break;
                case 'R': p = W_ROOK; break;
                case 'Q': p = W_QUEEN; break;
                case 'K': p = W_KING; break;

                case 'p': p = B_PAWN; break;
                case 'n': p = B_KNIGHT; break;
                case 'b': p = B_BISHOP; break;
                case 'r': p = B_ROOK; break;
                case 'q': p = B_QUEEN; break;
                case 'k': p = B_KING; break;

                default: throw std::invalid_argument("Invalid Piece in Board field");
            }

            SetSquare(Square(rank << 3 | file), p);
            ++file;
        }
        ++i;

    }

    assert (rank == 0 && file == 8);

    fen.remove_prefix(i + 1);

    side_to_move = (fen[0] == 'w') ? WHITE : BLACK;

    fen.remove_prefix(2);

    size_t j = 0;
    while (j < fen.size() && fen[j] != ' ') {
        switch (fen[j]) {
        case 'K':
            info.castling |= 1;
            break;
        case 'Q':
            info.castling |= 2;
            break;
        case 'k':
            info.castling |= 4;
            break;
        case 'q':
            info.castling |= 8;
            break;
        
        default:
            break;
        }
        ++j;
    }

    fen.remove_prefix(j + 1);

    if (fen[0] == '-') {
        info.ep_square = NO_SQUARE;
        fen.remove_prefix(2);
    } else {
        char file_char = fen[0];
        char rank_char = fen[1];
        info.ep_square = Square((rank_char - '1') << 3 | (file_char - 'a'));
        
        fen.remove_prefix(3);
    }

    size_t space = fen.find(' ');
    info.rule_50 = std::stoi(std::string(fen.substr(0, space)));
    fen.remove_prefix(space + 1);


    UpdateOccupancy();
}


std::string Position::ToString() const {

    const char* PIECE_TO_CHAR = "PNBRQKpnbrqk.";

    std::ostringstream string;

    string << "\n  +-----------------+\n";
    for (int rank = 7; rank >= 0; rank--) {
        string << rank + 1 << " | ";
        for (int file = 0; file < 8; file++) {
            
            string << PIECE_TO_CHAR[GetPiece(Square(rank << 3 | file))] << " ";
        }
        string << "|\n";
    }
    string << "  +-----------------+\n";
    string << "    a b c d e f g h\n\n";


    string << "\nSide to move: " << (side_to_move == WHITE ? "White": "Black") << "\n";
    string << "Castling rights: \n";
    string << ((info.castling & 1) != 0 ? "White Kingside\n" : "") << ((info.castling & 2) != 0 ? "White Queenside\n" : "") << ((info.castling & 4) != 0 ? "Black Kingside\n" : "") << ((info.castling & 8) != 0 ? "Black Queenside\n" : "") << "\n\n";   
    string << "En Passant Square: " << (info.ep_square == NO_SQUARE ? "-" : SquareToString(info.ep_square)) << "\n";
    
    return string.str();
}



void Position::ClearPosition() {
    for (int i = 0; i < 64; ++i) {
        pieces[i] = NO_PIECE;
    }

    for (int i = 0; i < 12; ++i) {
        bitboards[i] = 0ULL;
    }

    color_bitboards[0] = 0ULL;
    color_bitboards[1] = 0ULL;
    occupancy = 0ULL;

    info.castling = 0;
    info.ep_square = NO_SQUARE;
    info.rule_50 = 0;
    side_to_move = WHITE;
}

void Position::UpdateOccupancy() {
    color_bitboards[WHITE] = GetBitboard(W_PAWN) | GetBitboard(W_KNIGHT) | GetBitboard(W_BISHOP) | GetBitboard(W_ROOK) | GetBitboard(W_QUEEN) | GetBitboard(W_KING);
    color_bitboards[BLACK] = GetBitboard(B_PAWN) | GetBitboard(B_KNIGHT) | GetBitboard(B_BISHOP) | GetBitboard(B_ROOK) | GetBitboard(B_QUEEN) | GetBitboard(B_KING);

    occupancy = color_bitboards[WHITE] | color_bitboards[BLACK];
}

void Position::ClearSquare (Square square) {
    if (pieces[square] == NO_PIECE) return;

    bitboards[GetPiece(square)] &= ~(1ULL << square);
    pieces[square] = NO_PIECE;
}

void Position::SetSquare (Square square, Piece piece) {
    if (piece == NO_PIECE) {
        ClearSquare(square);
        return;
    }

    pieces[square] = piece;

    bitboards[piece] |= 1ULL << square;

    
}

// Assumes all moves are legal
void Position::MakeMove(Move move) {
    
    const Color us = side_to_move;
    const MoveFlag flag  = GetFlag(move);
    const Square from    = GetFrom(move);
    const Square to      = GetTo(move);
    const Piece moving   = pieces[from];
    const Piece captured = flag == EN_PASSANT ? MakePiece(PAWN, Opposite(us)): pieces[to];

    
    

    // Append all info before making move
    move_history[ply] = move;
    info_history[ply] = UndoInfo{info.rule_50, info.ep_square, info.castling, pieces[to]};
    // TO-DO: Add Hash
    ply++;

    side_to_move = Opposite(side_to_move);
    
    info.ep_square = NO_SQUARE;

    ClearSquare(from);

    switch (flag) {
        case CASTLING: {
            const Square rook_from = (to == G1 || to == G8) ?
                Square(to + 1):
                Square(to - 2);
            const Square rook_to = (to == G1 || to == G8) ?
                Square(to - 1):
                Square(to + 1);

            // Move Rook
            ClearSquare(rook_from);
            SetSquare(rook_to, MakePiece(ROOK, us));

            // Move King
            // We already cleared the from square earlier
            SetSquare(to, moving);

            if (us == WHITE) {
                info.castling &= ~0b0011;
            } else {
                info.castling &= ~0b1100;
            }

            break;
        }

        case EN_PASSANT: {
            const Square ep_capture = Square(to + (us == WHITE ? -8: 8));
            ClearSquare(ep_capture);
            SetSquare(to, moving);

            break;
        }

        case DOUBLE_PUSH: {
            SetSquare(to, moving);
            
            info.ep_square = Square(to + (us == WHITE ? -8 : 8));

            break;
        }

        case NPROMO:
        case BPROMO:
        case RPROMO:
        case QPROMO: {
            
            const Piece promoted_piece = MakePiece(PieceType(flag - NPROMO + 1), us);

            
            ClearSquare(to);
            

            SetSquare(to, promoted_piece);
            break;
        }

        default: {
            ClearSquare(to);

            SetSquare(to, moving);

            break;
        }
    }

    static constexpr uint8_t castling_mask[64] = {
    //  a  b  c  d  e  f  g  h
        2, 0, 0, 0, 0, 0, 0, 1, // 1
        0, 0, 0, 0, 0, 0, 0, 0, // 2                    
        0, 0, 0, 0, 0, 0, 0, 0, // 3                   
        0, 0, 0, 0, 0, 0, 0, 0, // 4               
        0, 0, 0, 0, 0, 0, 0, 0, // 5               
        0, 0, 0, 0, 0, 0, 0, 0, // 6               
        0, 0, 0, 0, 0, 0, 0, 0, // 7           
        8, 0, 0, 0, 0, 0, 0, 4  // 8
    };


    // If rook moves from its starting square, remove right
    if (castling_mask[from] && TypeOf(moving) == ROOK) {
        info.castling &= ~castling_mask[from];
    }

    // If piece captures rook, remove right
    if (castling_mask[to] && captured != NO_PIECE && TypeOf(captured) == ROOK) {
        info.castling &= ~castling_mask[to];
    }

    // King Moved
    if (TypeOf(moving) == KING) {
        const uint8_t king_mask = us == WHITE ? (0b0001 | 0b0010) : (0b0100 | 0b1000);
        info.castling &= ~king_mask;
    }

    // Update 50 move rule
    const bool is_pawn_move = TypeOf(moving) == PAWN;
    const bool is_capture = captured != NO_PIECE;

    if (is_pawn_move || is_capture) {
        info.rule_50 = 0;
    } else {
        ++info.rule_50;
    }

    UpdateOccupancy();

    
}

void Position::UndoMove() {
    side_to_move = Opposite(side_to_move);

    --ply;
    Move move = move_history[ply];
    UndoInfo undo_info = info_history[ply];

    info.castling = undo_info.castling;
    info.ep_square = undo_info.ep_square;
    info.rule_50 = undo_info.rule_50;

    const Color us = side_to_move;

    const MoveFlag flag = GetFlag(move);
    const Square to = GetTo(move);
    const Square from = GetFrom(move);

    const Piece moving = flag >= NPROMO ? MakePiece(PAWN, us): pieces[to];
    const Piece captured = undo_info.captured;

    

    switch (flag){
        case CASTLING: {
            const Square rook_from = (to == G1 || to == G8) ?
                Square(to + 1):
                Square(to - 2);
            const Square rook_to = (to == G1 || to == G8) ?
                Square(to - 1):  
                Square(to + 1); 

            ClearSquare(to);
            SetSquare(from, moving);

            ClearSquare(rook_to);
            SetSquare(rook_from, MakePiece(ROOK, us));
            break;
        }

        case EN_PASSANT: {
            ClearSquare(to);
            SetSquare(from, moving);
            SetSquare(Square(to + (us == WHITE ? -8: 8)), MakePiece(PAWN, Opposite(us)));

            break;
        }

        case DOUBLE_PUSH: {
            ClearSquare(to);
            SetSquare(from, moving);
            break;
        }

        case NPROMO:
        case BPROMO:
        case RPROMO:
        case QPROMO: {
            // Remove promo
            ClearSquare(to);
            
            // Restore original pawn
            
            SetSquare(from, moving);
            
            
            SetSquare(to, captured);
            
            break;
        }

        default: {
            ClearSquare(to);
            SetSquare(from, moving);
            SetSquare(to, captured);

            break;
        }
    }

    UpdateOccupancy();

}

// Use for checks and stuff
bool Position::IsAttacked(Square square, Color c) const {
    // Knights first: cheapest
    if (Bitboards::GetKnightAttacks(square) & GetBitboard(KNIGHT, c)) {
        // std::cout << "For Square " << SquareToString(square) << " Knight Attacked\n";
        return true;
    
    }

    // Pawns
    if (Bitboards::GetPawnAttacks(square, Opposite(c)) & GetBitboard(PAWN, c)){
        // std::cout << "For Square " << SquareToString(square) << " Pawn Attacked\n";
        return true;
    
    }

    // Kings
    if (Bitboards::GetKingAttacks(square) & GetBitboard(KING, c)) {
        // std::cout << "For Square " << SquareToString(square) << " King Attacked\n";
        return true;
    
    }

    // Sliders 
    Bitboard bishops_queens = GetBitboard(BISHOP, c) | GetBitboard(QUEEN, c);
    if (Bitboards::GetBishopAttacks(square, occupancy) & bishops_queens) {
        // std::cout << "For Square " << SquareToString(square) << " Bishop/Queen Attacked\n";
        return true;
    
    }

    Bitboard rooks_queens = GetBitboard(ROOK, c) | GetBitboard(QUEEN, c);
    if (Bitboards::GetRookAttacks(square, occupancy) & rooks_queens) {
        // std::cout << "For Square " << SquareToString(square) << " Rook/Queen Attacked\n";
        return true;
    
    }

    return false;
}

bool Position::IsInCheck (Color c) const {
    Square king_square = Square(ctz(GetBitboard(KING, c)));
    return IsAttacked (king_square, Opposite(c));
}

bool Position::IsInCheck () const {
    return IsInCheck(SideToMove());
}

bool Position::CanCastleKingside () const {
    if (IsInCheck()) return false;

    if (SideToMove() == WHITE) {
        if ((GetCastlingRights() & 0b0001) == 0) {
            
            return false;
        }

        constexpr Bitboard between_squares = (1ULL << F1) | (1ULL << G1);
        
        if ((occupancy & between_squares) != 0ULL) return false;

        if (IsAttacked(F1, BLACK)) return false;

        return true;
    }

    else {
        if ((GetCastlingRights() & 0b0100) == 0) return false;

        constexpr Bitboard between_squares = (1ULL << F8) | (1ULL << G8);

        if ((occupancy & between_squares) != 0ULL) return false;

        if (IsAttacked(F8, WHITE)) return false;

        return true;
    }
}

bool Position::CanCastleQueenside () const {
    if (IsInCheck()) return false;

    if (SideToMove() == WHITE) {
        if ((GetCastlingRights() & 0b0010) == 0) return false;

        constexpr Bitboard between_squares = (1ULL << D1) | (1ULL << C1) | (1ULL << B1);
        
        if ((occupancy & between_squares) != 0ULL) return false;

        if (IsAttacked(D1, BLACK) || IsAttacked(C1, BLACK)) return false;

        return true;
    }

    else {
        if ((GetCastlingRights() & 0b1000) == 0) return false;

        constexpr Bitboard between_squares = (1ULL << D8) | (1ULL << C8) | (1ULL << B8);

        if ((occupancy & between_squares) != 0ULL) return false;

        if (IsAttacked(D8, WHITE) || IsAttacked(C8, WHITE)) return false;

        return true;
    }
}







} // namespace Eyra
