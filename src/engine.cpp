#include "engine.hpp"



namespace Eyra {

// Evaluation
namespace {


Square FlipIndex (Square square) { return Square(square ^ 56); }

constexpr int mg_value[6] = {82, 337, 365, 477, 1025, 0};
constexpr int eg_value[6] = {94, 281, 297, 512,  936, 0};

constexpr int mg_pst[6][64] = {

    // PAWN
    {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10,-30,-30, 10, 10,  5,
    5, -5,-10,  0,  0,-10, -5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5,  5, 10, 25, 25, 10,  5,  5,
    10, 10, 20, 30, 30, 20, 10, 10,
    50, 50, 50, 50, 50, 50, 50, 50,
    0,  0,  0,  0,  0,  0,  0,  0,
    },

    // KNIGHT
    {
    -50,-20,-30,-30,-30,-30,-20,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50,
    },

    // BISHOP
    {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10,-10,-10,-10,-10,-20,
    },

    // ROOK
    {
    0,  0,  5, 10, 10,  5,  0,  0,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    5, 10, 10, 10, 10, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0,
    },

    // QUEEN
    {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -10,  5,  5,  5,  5,  5,  0,-10,
    0,  0,  5,  5,  5,  5,  0, -5,
    -5,  0,  5,  5,  5,  5,  0, -5,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20,
    },

    // KING
    // Punish king from leaving first rank 
    {
     20, 30, 10,  0,  0, 10, 30, 20,
     20, 20,  0,  0,  0,  0, 20, 20,
    -10,-20,-20,-20,-20,-20,-20,-10,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    
    
    
    
    
    
    
    }
};

static constexpr int eg_pst[6][64] = {

    // PAWN
    {
     0,  0,  0,  0,  0,  0,  0,  0,
    10, 10, 10, 10, 10, 10, 10, 10,
    20, 20, 20, 20, 20, 20, 20, 20,
    30, 30, 30, 30, 30, 30, 30, 30,
    40, 40, 40, 40, 40, 40, 40, 40,
    60, 60, 60, 60, 60, 60, 60, 60,
    80, 80, 80, 80, 80, 80, 80, 80,
     0,  0,  0,  0,  0,  0,  0,  0
    },

    // KNIGHT
    {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
    },

    // BISHOP
    {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  5, 10, 10, 10, 10,  5,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5, 10, 10, 10, 10,  5,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
    },

    // ROOK
    {
    0, 0, 5, 5, 5, 5, 0, 0,
    0, 0, 5, 5, 5, 5, 0, 0,
    0, 0, 5, 5, 5, 5, 0, 0,
    0, 0, 5, 5, 5, 5, 0, 0,
    0, 0, 5, 5, 5, 5, 0, 0,
    0, 0, 5, 5, 5, 5, 0, 0,
    5,10,10,10,10,10,10, 5,
    0, 0, 0, 0, 0, 0, 0, 0
    },

    // QUEEN
    {
    -20,-10,-10,-5,-5,-10,-10,-20,
    -10, 0, 0, 0, 0, 0, 0,-10,
    -10, 0, 5, 5, 5, 5, 0,-10,
    -5, 0, 5, 5, 5, 5, 0, -5,
    0, 0, 5, 5, 5, 5, 0, -5,
    -10, 5, 5, 5, 5, 5, 0,-10,
    -10, 0, 5, 0, 0, 0, 0,-10,
    -20,-10,-10,-5,-5,-10,-10,-20
    },

    // KING 
    // King is safe to leave
    {
    -50,-40,-30,-20,-20,-30,-40,-50,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-30,  0,  0,  0,  0,-30,-30,
    -50,-30,-30,-30,-30,-30,-30,-50
    }

};

} // namspace anonymous


namespace Engine {

namespace {


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

} // namespace anonymous
    

}

SearchInfo Engine::search_info;
Position Engine::position;
Move Engine::killers[32][2];


float Engine::EGWeight (Position& pos) {
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

int Engine::Evaluate (Position& pos) {
    // Draw by 50 move rule
    if (pos.GetRuleFifty() >= 100) {
        return 0;
    }

    float weight = EGWeight(pos);

    int score = 0;

    for (Square square = A1; square < NO_SQUARE; ++square) {
        Piece piece = pos.GetPiece(square);

        if (piece == NO_PIECE) continue;

        PieceType pt = TypeOf(piece);
        Color color = PieceColor(piece);

        if (color == WHITE) {
            score += static_cast<int>(weight * (eg_pst[pt][square] + eg_value[pt]) + (1 - weight) * (mg_pst[pt][square] + mg_value[pt]));
            
        } else {
            score -= static_cast<int>(weight * (eg_pst[pt][FlipIndex(square)] + eg_value[pt]) + (1 - weight) * (mg_pst[pt][FlipIndex(square)] + mg_value[pt]));
        }
    }

    // Clamp the score so it doesn't get interpreted as a mate
        score = std::max(-MAX_CP, std::min(score, MAX_CP));

    // Negamax
    score = (pos.SideToMove() == WHITE) ? score: -score;

    return score;
}

namespace {

int ScoreMove (const Position& pos, Move move, Move pv, Move killer_a, Move killer_b) {
    if (move == pv) return 10'000'000;

    Piece captured = pos.GetPiece(GetTo(move));
    MoveFlag flag = GetFlag(move);

    if (pos.GetPiece(GetTo(move)) != NO_PIECE) {
        int victim = mg_value[TypeOf(captured)];
        int attacker = mg_value[TypeOf(pos.GetPiece(GetFrom(move)))];

        return 1'000'000 + (10'000 * victim) + (1000 - attacker);
    }

    else if (flag >= NPROMO) {
        static constexpr int promo_bonus[] = {200, 220, 400, 800};

        return 900'000 + promo_bonus[flag - NPROMO];
    }

    if (move == killer_a) return 800'000;
    if (move == killer_b) return 799'999;

    return 0;

}

Move PickBestLookingMove (const Position& pos, MoveList& list, Move* current, Move pv, Move killer_a, Move killer_b) {
    Move* best = current;

    for (Move* m = current + 1; m != list.end(); m++) {
        if (ScoreMove(pos, *m, pv, killer_a, killer_b) > ScoreMove(pos, *best, pv, killer_a, killer_b)) {
            best = m;
        }
    }

    std::swap(*current, *best);

    return *current;
}


} // namespace anonymous



int Engine::QSearch (Position& pos, int depth, int alpha, int beta) {
    search_info.nodes++;

    if (search_info.stop.load(std::memory_order_relaxed)) {
        return 0;
    }
    if (depth == 0) {
        return Evaluate(pos);
    }

    int standpat = Evaluate(pos);

    if (standpat >= beta) return beta;

    alpha = std::max(alpha, standpat);

    Color side_moving = pos.SideToMove();
    MoveList moves;

    MoveGen::GenerateMoves(pos, moves);
    // Sort(pos, moves, 0);

    for (Move move: moves) {
        bool is_noisy = (pos.GetPiece(GetTo(move)) != NO_PIECE || GetFlag(move) >= NPROMO);

        if (!is_noisy) continue;

        int gain = std::abs(mg_value[TypeOf(pos.GetPiece(GetTo(move)))])  - std::abs(mg_value[TypeOf(pos.GetPiece(GetFrom(move)))] + 100);

        if (standpat + gain + 200 < alpha) continue;

        pos.MakeMove(move);

        if (pos.IsInCheck(side_moving)) {
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

int Engine::Search (Position& pos, int depth, int alpha, int beta, bool can_null_prune) {
    search_info.nodes++;

    if (search_info.stop.load(std::memory_order_relaxed)) {
        return 0;
    }

    if (depth <= 0) {
        return QSearch(pos, 16, alpha, beta);
    }

    int plies_from_root = search_info.depth - depth;

    Color side_moving = pos.SideToMove();
    MoveList moves;
    MoveGen::GenerateMoves(pos, moves);

    // Sort(pos, moves, 0, killers[plies_from_root][0], killers[plies_from_root][1]);

    int legal_moves = 0;
    int best_score = -INF;

    bool in_check = pos.IsInCheck();

    // Do not use null prune when:
    // Cannot null prune (being called during null prunee)
    // In Check (We can't skip turn in check)
    // Depth is less than 3 (there is no need to do null prune when there is barely any depth left)
    // When endgame weight is high (High chance of zugswang, where skipping your turn would actually be better than playing a move)
    if (can_null_prune && !in_check && depth > 3 && EGWeight(pos) < 0.67) {
        pos.MakeMove(0);

        int null_score = -Search(pos, depth - 3, -beta, -beta + 1, false);

        pos.UndoMove();

        if (null_score >= beta && std::abs(null_score) < MAX_CP) {
            return beta;
        }
    }
    
    for (Move* m = moves.begin(); m != moves.end(); ++m) {
        PickBestLookingMove(pos, moves, m, 0, killers[plies_from_root][0], killers[plies_from_root][1]);
        
        Move move = *m;
        Piece captured = pos.GetPiece(GetTo(move));
        pos.MakeMove(move);

        // Filter Illegal Immigrants
        if (pos.IsInCheck(side_moving)) {
            pos.UndoMove();
            continue;
        }

        

        int score;

        // Late Move Reduction
        if (legal_moves > 3 && depth >= 3 && !in_check && captured == NO_PIECE && GetFlag(move) < NPROMO) {
            // Search at a reduced depth
            int reduction = std::max(depth / 2, 1);
            score = -Search(pos, depth - 1 - reduction, -alpha - 1, -alpha, true);

            // If move is good (raises alpha) research at full depth
            if (score > alpha && score < beta) {
                score = -Search(pos, depth - 1, -beta, -alpha, true);
            }
        } else {
            score = -Search(pos, depth - 1, -beta, -alpha, true);
        }

        

        pos.UndoMove();

        if (score > best_score) {
            best_score = score;
        }

        ++legal_moves;

        alpha = std::max(alpha, score);

        if (alpha >= beta) {

            if (captured == NO_PIECE) {
                StoreKiller(move, plies_from_root);
            }

            break;
        }

        if (search_info.nodes % 1024 == 0 && ShouldStop()) {
            search_info.stop.store(true, std::memory_order_relaxed);
            return 0;
        }

        
    }

     if (legal_moves == 0) {
        // Checkmate
        if (pos.IsInCheck(side_moving)) return - (MATE_EVAL - plies_from_root);
        
        // Stalemate
        return 0;
    }

    return best_score;
}

SearchResults Engine::GetBestMove(Position& pos, int depth, Move pv) {
    Move best_move = 0;
    int best_score = -INF;

    Color side_moving = pos.SideToMove();

    search_info.depth = depth;

    MoveList moves;
    MoveGen::GenerateMoves(pos, moves);

    // Sort(pos, moves, pv);

    for (Move* m = moves.begin(); m != moves.end(); ++m) {

        PickBestLookingMove(pos, moves, m, pv, 0, 0);

        Move move = *m;

        pos.MakeMove(move);

        if (pos.IsInCheck(side_moving)) {
            pos.UndoMove();
            continue;
        }

        int score = -Search(pos, depth - 1, -INF, INF, true);

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

void Engine::Go(int depth, int movetime) {
    search_info.Reset();
    ResetKillers();

    search_info.start_time = steady_clock::now();
    search_info.max_time_ms = movetime;

    bool mate_found = false;

    Move best_move = 0;
    int eval = 0;

    for (int current_depth = 1; current_depth <= depth; ++current_depth) {

        if (search_info.stop.load(std::memory_order_relaxed)) break;

        if (mate_found) break;

        SearchResults result = GetBestMove(position, current_depth, best_move);


        if (result.best_move != 0) {
            eval = result.score;
            best_move = result.best_move;
        } else {
            break;
        }
        

        if (std::abs(eval) > MAX_CP) {
            mate_found = true;
        }

        auto elapsed = duration_cast<milliseconds> (steady_clock::now() - search_info.start_time).count();

        std::vector<Move> pv = {best_move};

        UCI::InfoDepth (
            current_depth, 
            eval,
            search_info.nodes,
            elapsed,
            pv
        );

        // std::cout << "info depth " << current_depth << " cp score " << eval << " nps " << (search_info.nodes * 1000 / std::max<uint64_t>(elapsed, 1) ) << " pv " << MoveToString(best_move) << std::endl;

    }

    std::cout << "bestmove " << MoveToString(best_move) << std::endl;

}

} // namespace Eyra
