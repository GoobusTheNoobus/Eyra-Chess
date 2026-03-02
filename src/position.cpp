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
    if (GetPiece(square) == NO_PIECE) return;

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

void Position::MakeMove(Move move) {
    // Append all info before making move
    move_history[counter] = move;
    info_history[counter] = GameInfo{info.rule_50, info.ep_square, info.castling};
    // TO-DO: Add Hash
    counter++;
    


    
}

// Use for checks and stuff
bool Position::IsAttacked(Square square, Color c) const {
    // Knights first: cheapest
    if (Bitboards::GetKnightAttacks(square) & GetBitboard(KNIGHT, c)) return true;

    // Pawns
    if (Bitboards::GetPawnAttacks(square, c) & GetBitboard(PAWN, c)) return true;

    // Kings
    if (Bitboards::GetKingAttacks(square) & GetBitboard(KING, c)) return true;

    // Sliders 
    Bitboard bishops_queens = GetBitboard(BISHOP, c) | GetBitboard(QUEEN, c);
    if (Bitboards::GetBishopAttacks(square, occupancy) & bishops_queens) return true;

    Bitboard rooks_queens = GetBitboard(ROOK, c) | GetBitboard(QUEEN, c);
    if (Bitboards::GetRookAttacks(square, occupancy) & rooks_queens) return true;

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

        if (IsAttacked(F1, BLACK) || IsAttacked(G1, BLACK)) return false;

        return true;
    }

    else {
        if ((GetCastlingRights() & 0b0100) == 0) return false;

        constexpr Bitboard between_squares = (1ULL << F8) | (1ULL << G8);

        if ((occupancy & between_squares) != 0ULL) return false;

        if (IsAttacked(F8, WHITE) || IsAttacked(G8, WHITE)) return false;

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
