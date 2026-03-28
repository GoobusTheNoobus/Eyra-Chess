#include "engine/engine.hpp"
#include "core/bitboard.hpp"




namespace Eyra 
{

// ======================= Transposition Table Functions =======================

TranspositionTable::TranspositionTable(size_t mb) 
{
    static constexpr size_t entry_size = 16;

    size_t bytes = mb * 1024 * 1024;
    count = bytes / entry_size;

    table.resize(count);

}

void TranspositionTable::Clear() 
{
    std::fill(table.begin(), table.end(), TranspositionEntry{}); // Fill the entire tables memeory chunk with empty entries

    store_count = 0;
}

void TranspositionTable::Store(Key key, int eval, int depth, TTFlag flag, Move best_move) 
{
    TranspositionEntry& entry = table[key % count];
    /*
    */
    // Replacement conditions:
    // - If depth is deeper
    // - If key is different



    if (entry.key != key || (entry.depth < depth && depth > 4)) {

        if (entry.flag == TTFlag::NONE) {
            store_count++;
        }

        entry = {key, (int16_t)eval, (uint8_t)depth, flag, best_move};
    }

}

TranspositionEntry* TranspositionTable::Probe(Key key) 
{
    TranspositionEntry* entry = &table[key % count];

    probe_count++;

    return (entry->key == key) ? entry: nullptr;
}

int TranspositionTable::Hashfull() {

    // Sample the first 1000 entries
    int counter = 0;
    for (int i = 0; i < 1000; ++i) {
        if (table[i].flag != TTFlag::NONE) {
            ++counter;
        }
    }

    return counter;
}

    // ======================= Evaluation =======================

namespace 
{

    Square FlipIndex (Square square) { return Square(square ^ 56); }

    constexpr int mg_value[6] = {82, 337, 365, 477, 1025, 0};
    constexpr int eg_value[6] = {94, 281, 297, 512,  936, 0};

    constexpr int mg_pst[6][64] =
    {
    // PAWN
    {
         0,   0,   0,   0,   0,   0,   0,   0,
       -35,  -1, -20, -23, -34,  24,  38, -22,
       -26,  -4,  -4,   0,   3,   3,  33, -12,
       -27,  -2,  -5,  20,  27,   6,  10, -25,
       -14,  13,   6,  30,  33,  12,  17, -23,
        -6,   7,  26,  46,  55,  56,  25, -20,
        98, 134,  61, 105, 147, 126,  34, -11,
         0,   0,   0,   0,   0,   0,   0,   0
    },
    // KNIGHT
    {
      -105, -25, -58, -33, -17, -28, -13, -23,
       -29, -53, -12,  -3,  -1,  18, -14, -19,
       -23,  -9,  16,  10,  10,  14,  25, -16,
       -13,   4,  16,  13,  28,  19,  21,  -8,
        -9,  17,  19,  53,  37,  69,  18,  22,
       -47,  60,  37,  65,  84, 129,  73,  44,
       -73, -41,  72,  36,  23,  62,   7, -17,
      -167, -89, -34, -49,  61, -97, -15, -107
    },
    // BISHOP
    {
       -33,  -3, -14, -21, -13,   0, -39, -21,
         4,  15,  16,   0,   7,  21,  33,   1,
         0,  15,  15,  15,  14,  27,  18,  10,
        -6,  13,  13,  26,  34,  12,  10,   4,
        -4,   5,  19,  50,  37,  37,   7,  -2,
       -16,  37,  43,  40,  35,  50,  37,  -2,
       -26,  16, -18, -13,  30,  59,  18, -47,
       -29,   4, -82, -37, -25, -42,   7,  -8
    },
    // ROOK
    {
       -19, -13,   1,  17,  16,   7, -37, -26,
       -44, -16, -20,  -9,  -1,  11,  -6, -71,
       -45, -25, -16, -17,   3,   0,  -5, -33,
       -36, -26, -12,  -1,   9,  -7,   6, -23,
       -24, -11,   7,  26,  24,  35,  -8, -20,
        -5,  19,  26,  36,  17,  45,  61,  16,
        27,  32,  58,  62,  80,  67,  26,  44,
        32,  42,  32,  51,  63,   9,  31,  43
    },
    // QUEEN
    {
        -1, -18,  -9,  10, -15, -25, -31, -50,
       -35,  -8,  11,   2,   0,  15,  -3,   1,
       -14,   2, -11,  -2,  -5,   0,  14,   5,
        -9, -26,  -9, -10,  -2,  -4,   3,  -3,
       -27, -27, -16, -16,  -1,  17,  -2,   1,
       -13, -17,   7,   8,  29,  56,  47,  57,
       -24, -39,  -5,   1, -16,  57,  28,  54,
       -28,   0,  29,  12,  59,  44,  43,  45
    },
    // KING
    {
       -15,  36,  12, -54,   8, -28,  24,  14,
         1,   7,  -8, -64, -43, -16,   9,   8,
       -14, -14, -22, -46, -44, -30, -15, -27,
       -49,  -1, -27, -39, -46, -44, -33, -51,
       -17, -20, -12, -27, -30, -25, -14, -36,
        -9,  24,   2, -16, -20,   6,  22, -22,
        29,  -1, -20,  -7,  -8,  -4, -38, -29,
       -65,  23,  16, -15, -56, -34,   2,  13
    }
    };

    static constexpr int eg_pst[6][64] =
    {
    // PAWN
    {
         0,   0,   0,   0,   0,   0,   0,   0,
        13,   8,   8,  10,  13,   0,   2,  -7,
         4,   7,  -6,   1,   0,  -5,  -1,  -8,
        13,   9,  -3,  -7,  -7,  -8,   3,  -1,
        32,  24,  13,   5,  -2,   4,  17,  17,
        94, 100,  85,  67,  56,  53,  82,  84,
       178, 173, 158, 134, 147, 132, 165, 187,
         0,   0,   0,   0,   0,   0,   0,   0
    },
    // KNIGHT
    {
       -29, -51, -23, -15, -22, -18, -50, -64,
       -42, -20, -10,  -5,  -2, -20, -23, -44,
       -23,  -3,  -1,  15,  10,  -3, -20, -22,
       -18,  -6,  16,  25,  16,  17,   4, -18,
       -17,   3,  22,  22,  22,  11,   8, -18,
       -24, -20,  10,   9,  -1,  -9, -19, -41,
       -25,  -8, -25,  -2,  -9, -25, -24, -52,
       -58, -38, -13, -28, -31, -27, -63, -99
    },
    // BISHOP
    {
       -23,  -9, -23,  -5,  -9, -16,  -5, -17,
       -14, -18,  -7,  -1,   4,  -9, -15, -27,
       -12,  -3,   8,  10,  13,   3,  -7, -15,
        -6,   3,  13,  19,   7,  10,  -3,  -9,
        -3,   9,  12,   9,  14,  10,   3,   2,
         2,  -8,   0,  -1,  -2,   6,   0,   4,
        -8,  -4,   7, -12,  -3, -13,  -4, -14,
       -14, -21, -11,  -8,  -7,  -9, -17, -24
    },
    // ROOK
    {
        -9,   2,   3,  -1,  -5, -13,   4, -20,
        -6,  -6,   0,   2,  -9,  -9, -11,  -3,
        -4,   0,  -5,  -1,  -7, -12,  -8, -16,
         3,   5,   8,   4,  -5,  -6,  -8, -11,
         4,   3,  13,   1,   2,   1,  -1,   2,
         7,   7,   7,   5,   4,  -3,  -5,  -3,
        11,  13,  13,  11,  -3,   3,   8,   3,
        13,  10,  18,  15,  12,  12,   8,   5
    },
    // QUEEN
    {
       -33, -28, -22, -43,  -5, -32, -20, -41,
       -22, -23, -30, -16, -16, -23, -36, -32,
       -16, -27,  15,   6,   9,  17,  10,   5,
       -18,  28,  19,  47,  31,  34,  39,  23,
         3,  22,  24,  45,  57,  40,  57,  36,
       -20,   6,   9,  49,  47,  35,  19,   9,
       -17,  20,  32,  41,  58,  25,  30,   0,
        -9,  22,  22,  27,  27,  19,  10,  20
    },
    // KING
    {
       -53, -34, -21, -11, -28, -14, -24, -43,
       -27, -11,   4,  13,  14,   4,  -5, -17,
       -19,  -3,  11,  21,  23,  16,   7,  -9,
       -18,  -4,  21,  24,  27,  23,   9, -11,
        -8,  22,  24,  27,  26,  33,  26,   3,
        10,  17,  23,  15,  20,  45,  44,  13,
       -12,  17,  14,  17,  17,  38,  23,  11,
       -74, -35, -18, -18, -11,  15,   4, -17
    }
    };
} // namespace anonymous


namespace Engine 
{


    // Finds how close to the endgame the position is
    // 1 means pawn and king only
    // 0 means there are lots of pieces on te board still
    float EGWeight (Position& pos) 
    {
        static constexpr int KNIGHT_GAME_PHASE = 2;
        static constexpr int BISHOP_GAME_PHASE = 3;
        static constexpr int ROOK_GAME_PHASE = 4;
        static constexpr int QUEEN_GAME_PHASE = 5;

        static constexpr int MAX_GAME_PHASE = 23;

        // Include every piece except for kings
        int phase = 
            popcount(pos.GetBitboard(W_KNIGHT) | pos.GetBitboard(B_KNIGHT)) * KNIGHT_GAME_PHASE +
            popcount(pos.GetBitboard(W_BISHOP) | pos.GetBitboard(B_BISHOP)) * BISHOP_GAME_PHASE +
            popcount(pos.GetBitboard(W_ROOK)   | pos.GetBitboard(B_ROOK)) * ROOK_GAME_PHASE +
            std::min(popcount(pos.GetBitboard(W_QUEEN)  | pos.GetBitboard(B_QUEEN)), 1) * QUEEN_GAME_PHASE; // If a pawn promotes and there are multiple queens, only count 1

        return 1 - std::min (1.0f, static_cast<float> (phase) / MAX_GAME_PHASE);
    }

    int Evaluate(Position& pos)
    {
        static constexpr int knight_mobility_bonus = 2;
        static constexpr int bishop_mobility_bonus = 2;
        static constexpr int rook_mobility_bonus   = 1;
        static constexpr int queen_mobility_bonus  = 1;

        if (pos.GetRuleFifty() >= 100)
            return 0;

        float weight = EGWeight(pos);
        int score = 40 - (1 - weight) * 30;

        // Evaluates Material AND Mobility
        auto eval_piece = [&](Bitboard bb, PieceType pt, bool white)
        {
            while (bb)
            {
                int sq = ctz(bb);
                int pst_index = white ? sq: FlipIndex(Square(sq));
                bb &= bb - 1;

                // Material
                int mg = mg_pst[pt][pst_index] + mg_value[pt];
                int eg = eg_pst[pt][pst_index] + eg_value[pt];

                int val = static_cast<int>(weight * eg + (1 - weight) * mg);

                
                /*
                // Mobility
                if (pt == KNIGHT) 
                    val += popcount(Bitboards::GetKnightAttacks(Square(sq))) * knight_mobility_bonus;

                else if (pt == BISHOP)
                    val += popcount(Bitboards::GetBishopAttacks(Square(sq), pos.GetOccupancy())) * bishop_mobility_bonus;

                else if (pt == ROOK)
                    val += popcount(Bitboards::GetRookAttacks(Square(sq), pos.GetOccupancy())) * rook_mobility_bonus;

                else if (pt == QUEEN && weight > 0.5)
                    val += popcount(Bitboards::GetBishopAttacks(Square(sq), pos.GetOccupancy()) | Bitboards::GetRookAttacks(Square(sq), pos.GetOccupancy())) * queen_mobility_bonus;

                */
                if (white) score += val;
                else       score -= val;
                
                
            }
        };

        // White pieces
        eval_piece(pos.GetBitboard(W_PAWN),   PAWN,   true);
        eval_piece(pos.GetBitboard(W_KNIGHT), KNIGHT, true);
        eval_piece(pos.GetBitboard(W_BISHOP), BISHOP, true);
        eval_piece(pos.GetBitboard(W_ROOK),   ROOK,   true);
        eval_piece(pos.GetBitboard(W_QUEEN),  QUEEN,  true);
        eval_piece(pos.GetBitboard(W_KING),   KING,   true);

        // Black pieces
        eval_piece(pos.GetBitboard(B_PAWN),   PAWN,   false);
        eval_piece(pos.GetBitboard(B_KNIGHT), KNIGHT, false);
        eval_piece(pos.GetBitboard(B_BISHOP), BISHOP, false);
        eval_piece(pos.GetBitboard(B_ROOK),   ROOK,   false);
        eval_piece(pos.GetBitboard(B_QUEEN),  QUEEN,  false);
        eval_piece(pos.GetBitboard(B_KING),   KING,   false);

        // Pawn structure
        for (int file = 0; file < 8; ++file)
        {
            static constexpr int double_pawn_deduction[] = {-30, -2, -7, -21, -16, -19, -3, -32};

            Bitboard white_pawns = pos.GetBitboard(W_PAWN);
            Bitboard black_pawns = pos.GetBitboard(B_PAWN);

            // Double Pawns
            if (popcount(Bitboards::files[file] & white_pawns) >= 2)
            {
                score += double_pawn_deduction[file];

                // TODO: Isolated Pawns
            }

            if (popcount(Bitboards::files[file] & black_pawns) >= 2)
            {
                score -= double_pawn_deduction[file];

                // TODO: Isolated Pawns
            }
        }

        if (weight < 0.4 && !(pos.GetBitboard(W_QUEEN) & Bitboards::SquareBB(D1)))
        {
            score -= 12;
        }

        if (weight < 0.4 && !(pos.GetBitboard(B_QUEEN) & Bitboards::SquareBB(D8)))
        {
            score += 12;
        }
        
        // Normalize score if early in the game
        score = score / (2 - weight);

        score = std::clamp(score, -MAX_CP, MAX_CP);

        if (pos.SideToMove() == BLACK)
            score = -score;

        return score;
    }

    int NNUEEval(Position& pos)
    {
        // Maps how pieces are stored in EyraChess to how NNUE processes it
        // White Pawn = 1 White Knight = 2.....
        // Black Pawn = 9 Black Knight = 10....
        // 0 is empty
        static constexpr int nnue_pieces[] = {1, 2, 3, 4, 5, 6, 9, 10, 11, 12, 13, 14, 0};

        int pieces[32] = {};
        int squares[32] = {};
        int piece_count = 2;

        for (Square square = A1; square < NO_SQUARE; ++square)
        {
            Piece piece = pos.GetPiece(square);

            
            if (piece != NO_PIECE && TypeOf(piece) != KING)
            {
                pieces[piece_count] = nnue_pieces[piece];
                squares[piece_count] = square;
                ++piece_count;
            }

            else if (piece == W_KING)
            {
                pieces[0] = nnue_pieces[piece];
                squares[0] = square;
            }

            else if (piece == B_KING)
            {
                pieces[1] = nnue_pieces[piece];
                squares[1] = square;
            }
            
            
            
        }

        bool side = pos.SideToMove() == WHITE;
        int rule50 = pos.GetRuleFifty();

        /// return Stockfish::Probe::eval(pieces, squares, piece_count, side, rule50);
        // return Stockfish::Probe::eval(pieces, squares, piece_count, side, rule50);

        return Stockfish::Probe::eval(pieces, squares, piece_count, side, rule50);


    }
    

    // ======================= Engine States =======================

    SearchInfo search_info;
    Position position;
    Move killers[MAX_DEPTH][KILLERS_PER_DEPTH];
    int history[COLORS][BOARD_SIZE][BOARD_SIZE];

    TranspositionTable tt(64);

    // ======================= Helper Functions =======================

    namespace 
    {


        void Stop() {
            search_info.stop.store(true, std::memory_order_relaxed);
        }

        bool ShouldStop() {
            if (search_info.stop.load(std::memory_order_relaxed)) return true;

            if (search_info.max_time_ms > 0) {
                auto now = steady_clock::now();
                auto elapsed = duration_cast<std::chrono::milliseconds>(now - search_info.start_time).count();

                if (elapsed >= search_info.max_time_ms) {
                    return true;
                }
            }
            return false;
        }

        int ScoreMove (const Position& pos, Move move, Move pv, Move killer_a, Move killer_b, Move tt_move) {
            if (move == pv)      return 10'000'000;
            if (move == tt_move) return 9'999'999;

            Piece captured = pos.GetPiece(GetTo(move));
            Piece moved    = pos.GetPiece(GetFrom(move));
            MoveFlag flag = GetFlag(move);

            if (pos.GetPiece(GetTo(move)) != NO_PIECE) {
                int victim = mg_value[TypeOf(captured)];
                int attacker = mg_value[TypeOf(moved)];

                return 1'000'000 + (10'000 * victim) + (1000 - attacker);
            }

            else if (flag >= NPROMO) {
                static constexpr int promo_bonus[] = {200, 220, 400, 800};

                return 900'000 + promo_bonus[flag - NPROMO];
            }

            if (move == killer_a) return 800'000;
            if (move == killer_b) return 799'999;

            if (captured == NO_PIECE)
            {
                return 600'000 + history[PieceColor(moved)][GetFrom(move)][GetTo(move)];
            }

            return 0;

        }

        Move PickBestLookingMove (const Position& pos, MoveList& list, Move* current, Move pv, Move killer_a, Move killer_b, Move tt_move = 0) {
            Move* best = current;

            for (Move* m = current + 1; m != list.end(); m++) {
                if (ScoreMove(pos, *m, pv, killer_a, killer_b, tt_move) > ScoreMove(pos, *best, pv, killer_a, killer_b, tt_move)) {
                    best = m;
                }
            }

            std::swap(*current, *best);

            return *current;
        }

        // Call every 100k nodes
        void DecayHistoryTable() {
            for (int c = 0; c < COLORS; ++c) 
            for (int from = 0; from < 64; ++from)
            for (int to = 0; to < 64; ++to)
            history[c][from][to] /= 2;
        }


    } // namespace anonymous

    // ======================= Search Function Definitions =======================

    // Quiescence Search only searches captures and promotions
    int QSearch (Position& pos, int depth, int alpha, int beta) 
    {
        search_info.nodes++;

        if (search_info.stop.load(std::memory_order_relaxed)) 
            return 0;
        
        if (depth == 0) 
            return NNUEEval(pos);
        

        int standpat = NNUEEval(pos);

        // Position is too good
        if (standpat >= beta) return beta;

        alpha = std::max(alpha, standpat);

        Color side_moving = pos.SideToMove();

        MoveList moves;
        MoveGen::GenerateMoves(pos, moves);

        for (Move move: moves) 
        {
            bool is_noisy = (pos.GetPiece(GetTo(move)) != NO_PIECE || GetFlag(move) >= NPROMO);
            if (!is_noisy) continue;

            int gain = std::abs(mg_value[TypeOf(pos.GetPiece(GetTo(move)))])  - std::abs(mg_value[TypeOf(pos.GetPiece(GetFrom(move)))] + 100);
            if (standpat + gain < alpha) continue;

            pos.MakeMove(move);

            if (pos.IsInCheck(side_moving)) 
            {
                pos.UndoMove();
                continue;
            }

            int score = -QSearch(pos, depth - 1, -beta, -alpha);

            pos.UndoMove();

            if (score >= beta) {
                return beta;
            }

            alpha = std::max(alpha, score);
        }

        return alpha;

    }

    // Negamax Search where the score is for the current side to move
    int Search (Position& pos, int depth, int alpha, int beta, bool can_null_prune) 
    {
        search_info.nodes++;

        if (search_info.stop.load(std::memory_order_relaxed)) {
            return 0;
        }

        if (depth <= 0) {
            return QSearch(pos, 32, alpha, beta);
        }

        if (pos.GetRuleFifty() == 100) {
            // Drawn Position due to 50 move rule
            return 0;
        }

        if (pos.IsRepetition()) {
            return 0;
        }

        int plies_from_root = search_info.depth - depth;
        Color side_moving = pos.SideToMove();

        TranspositionEntry* entry = tt.Probe(pos.Hash());
        Move tt_move = 0;

        if (entry != nullptr && entry->key == pos.Hash() && entry->depth >= depth) 
        {
            tt_move = entry->best_move;

            const int tt_score = entry->eval;

            // Un-adjust mate scores from TT, since we store as MATE_EVAL
            // if (tt_score > MAX_CP)  tt_score -= plies_from_root;
            // if (tt_score < -MAX_CP) tt_score += plies_from_root;

            if (entry->flag == TTFlag::EXACT)      return tt_score;
            if (entry->flag == TTFlag::LOWERBOUND) alpha = std::max<int>(alpha, tt_score);
            if (entry->flag == TTFlag::UPPERBOUND) beta  = std::min<int>(beta, tt_score);

            // if (alpha >= beta && entry->flag == TTFlag::EXACT) return tt_score;
        } 
        
        int static_eval = NNUEEval(pos);
        

        MoveList moves;
        MoveGen::GenerateMoves(pos, moves);

        int legal_moves = 0;
        int best_score = -INF;
        Move best_move = 0;

        TTFlag flag = TTFlag::UPPERBOUND;

        bool in_check = pos.IsInCheck();

        
        if (can_null_prune && !in_check && depth > 3 && EGWeight(pos) < 0.67) 
        {
            pos.MakeMove(0);

            int null_score = -Search(pos, depth - 3, -beta, -beta + 1, false);

            pos.UndoMove();

            if (null_score >= beta && std::abs(null_score) < MAX_CP) {
                return beta;
            }
        }
        
        // Do not use null prune when:
        // Cannot null prune (being called during null prunee)
        // In Check (We can't skip turn in check)
        // Depth is less than 3 (there is no need to do null prune when there is barely any depth left)
        // When endgame weight is high (High chance of zugswang, where skipping your turn would actually be better than playing a move)
        
        
        for (Move* m = moves.begin(); m != moves.end(); ++m) 
        {
            PickBestLookingMove(pos, moves, m, 0, killers[plies_from_root][0], killers[plies_from_root][1], tt_move);
            
            Move move = *m;
            Piece captured = pos.GetPiece(GetTo(move));
            pos.MakeMove(move);

            // Filter moves that put the moving side into check
            if (pos.IsInCheck(side_moving)) {
                pos.UndoMove();
                continue;
            }

            // LMP
            if (legal_moves > 5 && depth < 3 && static_eval + 120 < alpha)
            {
                pos.UndoMove();
                continue;
            }

            int score;

            // Late Move Reduction
            if (legal_moves > 3 && depth >= 3 && !in_check && captured == NO_PIECE && GetFlag(move) < NPROMO) 
            {
                // Search at a reduced depth
                int reduction = 1;
                score = -Search(pos, depth - 1 - reduction, -alpha - 1, -alpha, true);

                // If move is good (raises alpha) research at full depth
                if (score > alpha && score < beta) 
                {
                    score = -Search(pos, depth - 1, -beta, -alpha, true);
                }
            } 
            
            else 
            {
                score = -Search(pos, depth - 1, -beta, -alpha, true);
            }

            

            pos.UndoMove();

            if (score > best_score) 
            {
                best_score = score;
                best_move = move;
            }

            ++legal_moves;


            if (captured == NO_PIECE && score < alpha) {
                history[side_moving][GetFrom(move)][GetTo(move)] -= depth * depth;
            }

            if (score > alpha) 
            {
                alpha = score;
                flag = TTFlag::EXACT;
            }

            if (alpha >= beta) 
            {
                flag = TTFlag::LOWERBOUND;

                if (captured == NO_PIECE) {
                    StoreKiller(move, plies_from_root);
                    history[side_moving][GetFrom(move)][GetTo(move)] += depth * depth;
                }

                break;
            }

            if (search_info.nodes % 1024 == 0 && ShouldStop()) {
                search_info.stop.store(true, std::memory_order_relaxed);
                return 0;
            }

            if (search_info.nodes % 1'000'000 == 0)
            {
                DecayHistoryTable();
            }

            
        }

        // All pseudo legal moves filtered out and no legal move
        if (legal_moves == 0) 
        {
            // Checkmate
            if (pos.IsInCheck(side_moving)) return - (MATE_EVAL - plies_from_root);
            
            // Stalemate
            return 0;
        }

        // Store in TT

        // When storing in TT, treat the current position like root
        // When storing, store mate score
        // When retrieving, adjust based on plies from root
        
        
        int store_score = best_score;
        if (store_score > MAX_CP) 
        {
            // std::cout << "The score is a mate score. Plies from root: " << plies_from_root << std::endl; 
            store_score = MAX_CP + 1;
            flag = TTFlag::LOWERBOUND;
        }
            
        if (store_score < -MAX_CP) 
        {
            // std::cout << "The score is a negative mate score. Plies from root: " << plies_from_root << std::endl; 
            store_score = -MAX_CP - 1;
            flag = TTFlag::UPPERBOUND;
        }
            
        
        
        
        tt.Store(pos.Hash(), store_score, depth, flag, best_move);
        
        
        // assert(std::abs(best_score) < MATE_EVAL);

        return best_score;
    }



    SearchResults GetBestMove(Position& pos, int depth, Move pv, int alpha, int beta) 
    {
        Move best_move = 0;
        int best_score = -INF;

        Color side_moving = pos.SideToMove();

        search_info.depth = depth;

        MoveList moves;
        MoveGen::GenerateMoves(pos, moves);

        // Sort(pos, moves, pv);

        for (Move* m = moves.begin(); m != moves.end(); ++m) 
        {

            PickBestLookingMove(pos, moves, m, pv, 0, 0);

            Move move = *m;

            pos.MakeMove(move);

            if (pos.IsInCheck(side_moving)) {
                pos.UndoMove();
                continue;
            }

            int score = -Search(pos, depth - 1, -beta, -alpha, true);



            pos.UndoMove();

            if (search_info.nodes % 1024 == 0 && ShouldStop()) {
                Stop();
            }

            if (search_info.stop.load(std::memory_order_relaxed)) return {0, 0};

            if (score > best_score) {
                best_score = score;
                best_move = move;
            }
        }

        return {best_move, best_score};
    }

    void Go(int depth, int movetime) 
    {
        search_info.Reset();
        ResetKillers();

        // tt.Clear();

        search_info.start_time = steady_clock::now();
        search_info.max_time_ms = movetime;

        bool mate_found = false;

        Move best_move = 0;
        int eval = 0;

        int aspiration_window = 50;
        

        for (int current_depth = 1; current_depth <= depth; ++current_depth) 
        {

            if (search_info.stop.load(std::memory_order_relaxed)) break;


            SearchResults result;

            int window = aspiration_window;

            while (true)
            {
                int alpha = eval - window;
                int beta  = eval + window;

                result = GetBestMove(position, current_depth, best_move, alpha, beta);

                if (result.score <= alpha)
                {
                    window *= 2;
                }
                else if (result.score >= beta)
                {
                    window *= 2;
                }
                else break;
            }

            


            if (result.best_move != 0) 
            {
                eval = result.score;
                best_move = result.best_move;
            } else break;

            if (std::abs(eval) > MAX_CP) 
                mate_found = true;
            

            auto elapsed = duration_cast<milliseconds> (steady_clock::now() - search_info.start_time).count();

            std::vector<Move> pv = {best_move};
            pv.reserve(depth - 2);

            position.MakeMove(best_move);

            int i = 0;
            for (i = 0; i < depth - 2; ++i) 
            {
                TranspositionEntry* entry = tt.Probe(position.Hash());

                if (entry != nullptr && position.IsLegal(entry->best_move) && entry->flag == TTFlag::EXACT) 
                {
                    position.MakeMove(entry->best_move);
                    pv.push_back(entry->best_move);
                }

                else
                {
                    // UCI::InfoString("No PV");
                    break;
                } 
            }

            for (int j = 0; j <= i; ++j) 
            {
                position.UndoMove();
            }

            UCI::InfoDepth(
                current_depth, 
                eval,
                search_info.nodes,
                elapsed,
                pv,
                tt.Hashfull()
            );
        }

        UCI::BestMove(best_move);
        // UCI::InfoString("Probe count: " + std::to_string(tt.probe_count));



    }

    // Perft test, returns number of nodes a in depth moves
    int Perft (int depth) 
    {
        if (depth == 0) 
            return 1;
        

        Color side_moving = position.SideToMove();
        MoveList moves;
        MoveGen::GenerateMoves(position, moves);

        int num_moves = 0;

        for (Move move: moves) 
        {
            position.MakeMove(move);

            if (position.IsInCheck(side_moving)) {
                position.UndoMove();
                continue;
            }

            num_moves += Perft(depth - 1);

            position.UndoMove();
        }

        return num_moves;

        
    }

    // Prints the perft result of each legal move 
    void PerftDivide(int depth) 
    {
        Color side_moving = position.SideToMove();
        MoveList moves;
        MoveGen::GenerateMoves(position, moves);

        uint64_t total = 0;

        auto start = std::chrono::high_resolution_clock::now();

        for (Move move : moves) 
        {
            position.MakeMove(move);

            if (position.IsInCheck(side_moving)) {
                position.UndoMove();
                continue;
            }

            uint64_t count = Perft(depth - 1);
            total += count;

            std::cout << MoveToString(move) << ": " << count << "\n";

            position.UndoMove();
        }

        auto end = std::chrono::high_resolution_clock::now();
        uint64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        elapsed = std::max<uint64_t>(elapsed, 1); // prevent division by zero

        std::cout << "\nTotal: " << total << "\n";
        std::cout << "Time:  " << elapsed << "ms\n";
        std::cout << "NPS:   " << (total * 1000 / elapsed) << "\n";
    }
        

    } // namespace Engine


} // namespace Eyra




